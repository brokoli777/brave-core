/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/ai_chat/content/browser/full_screenshotter.h"

#include "base/base64.h"
#include "base/task/bind_post_task.h"
#include "base/strings/stringprintf.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "base/types/expected.h"
#include "components/paint_preview/browser/compositor_utils.h"
#include "components/paint_preview/browser/paint_preview_base_service.h"
#include "components/paint_preview/common/recording_map.h"
#include "mojo/public/cpp/base/proto_wrapper.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/geometry/rect.h"

constexpr size_t kMaxScreenshotFileSize = 50 * 1000L * 1000L;  // 50 MB.

namespace ai_chat {
namespace {

void WriteBitmapToPng(const SkBitmap& bitmap, FullScreenshotter::CaptureScreenshotCallback callback) {
  std::vector<unsigned char> data;
  bool encoded = gfx::PNGCodec::EncodeBGRASkBitmap(bitmap, false, &data);
  if (!encoded) {
    std::move(callback).Run(base::unexpected("Failed to encode the bitmap"));
    return;
  }
  std::move(callback).Run(base::ok(base::Base64Encode(data)));
}

}  // namespace

FullScreenshotter::FullScreenshotter()
    : paint_preview::PaintPreviewBaseService(
          /*file_mixin=*/nullptr,  // in-memory captures
          /*policy=*/nullptr,      // all content is deemed amenable
          /*is_off_the_record=*/true),
      paint_preview_compositor_service_(nullptr,
                                        base::OnTaskRunnerDeleter(nullptr)),
      paint_preview_compositor_client_(nullptr,
                                       base::OnTaskRunnerDeleter(nullptr)) {
  paint_preview_compositor_service_ =
      paint_preview::StartCompositorService(base::BindOnce(
          &FullScreenshotter::OnCompositorServiceDisconnected,
          weak_ptr_factory_.GetWeakPtr()));
  CHECK(paint_preview_compositor_service_);
}

FullScreenshotter::~FullScreenshotter() = default;

void FullScreenshotter::CaptureScreenshot(
    const raw_ptr<content::WebContents> web_contents,
    CaptureScreenshotCallback callback) {
  if (!web_contents) {
    std::move(callback).Run(base::unexpected("The given web contents no longer valid"));
    return;
  }

  // Start capturing via Paint Preview.
  CaptureParams capture_params;
  capture_params.web_contents = web_contents;
  capture_params.max_per_capture_size = kMaxScreenshotFileSize;
  capture_params.skip_accelerated_content = true;
  capture_params.persistence =
      paint_preview::RecordingPersistence::kMemoryBuffer;
  CapturePaintPreview(
      capture_params,
      base::BindOnce(&FullScreenshotter::OnScreenshotCaptured,
                     weak_ptr_factory_.GetWeakPtr(), std::move(callback)));
}

void FullScreenshotter::OnScreenshotCaptured(
    CaptureScreenshotCallback callback,
    paint_preview::PaintPreviewBaseService::CaptureStatus status,
    std::unique_ptr<paint_preview::CaptureResult> result) {
  if (status != PaintPreviewBaseService::CaptureStatus::kOk ||
      !result->capture_success) {
    std::move(callback).Run(base::unexpected(base::StringPrintf(
        "Failed to capture a screenshot (CaptureStatus=%d)",
        static_cast<int>(status))));
    return;
  }
  if (!paint_preview_compositor_client_) {
    paint_preview_compositor_client_ =
        paint_preview_compositor_service_->CreateCompositor(
            base::BindOnce(&FullScreenshotter::SendCompositeRequest,
                           weak_ptr_factory_.GetWeakPtr(),
                           std::move(callback),
                           PrepareCompositeRequest(std::move(result))));
  } else {
    SendCompositeRequest(std::move(callback), PrepareCompositeRequest(std::move(result)));
  }
}

paint_preview::mojom::PaintPreviewBeginCompositeRequestPtr
FullScreenshotter::PrepareCompositeRequest(
    std::unique_ptr<paint_preview::CaptureResult> capture_result) {
  paint_preview::mojom::PaintPreviewBeginCompositeRequestPtr
      begin_composite_request =
          paint_preview::mojom::PaintPreviewBeginCompositeRequest::New();
  std::pair<paint_preview::RecordingMap, paint_preview::PaintPreviewProto>
      map_and_proto = paint_preview::RecordingMapFromCaptureResult(
          std::move(*capture_result));
  begin_composite_request->recording_map = std::move(map_and_proto.first);
  if (begin_composite_request->recording_map.empty()) {
    VLOG(2) << "Captured an empty screenshot";
    return nullptr;
  }
  begin_composite_request->preview =
      mojo_base::ProtoWrapper(std::move(map_and_proto.second));
  return begin_composite_request;
}

void FullScreenshotter::SendCompositeRequest(
    CaptureScreenshotCallback callback,
    paint_preview::mojom::PaintPreviewBeginCompositeRequestPtr
        begin_composite_request) {
  if (!begin_composite_request) {
    std::move(callback).Run(base::unexpected("Invalid begin_composite_request"));
    return;
  }

  CHECK(paint_preview_compositor_client_);
  paint_preview_compositor_client_->BeginMainFrameComposite(
      std::move(begin_composite_request),
      base::BindOnce(&FullScreenshotter::OnCompositeFinished,
                     weak_ptr_factory_.GetWeakPtr(),
                     std::move(callback)));
}

void FullScreenshotter::OnCompositorServiceDisconnected() {
  VLOG(2) << "Compositor service is disconnected";
  paint_preview_compositor_client_.reset();
  paint_preview_compositor_service_.reset();
}

void FullScreenshotter::OnCompositeFinished(
    CaptureScreenshotCallback callback,
    paint_preview::mojom::PaintPreviewCompositor::BeginCompositeStatus status,
    paint_preview::mojom::PaintPreviewBeginCompositeResponsePtr response) {
  if (status != paint_preview::mojom::PaintPreviewCompositor::
                    BeginCompositeStatus::kSuccess &&
      status != paint_preview::mojom::PaintPreviewCompositor::
                    BeginCompositeStatus::kPartialSuccess) {
    std::move(callback).Run(base::unexpected(base::StringPrintf(
        "Failed to composite (BeginCompositeStatus=%d)",
        static_cast<int>(status))));
    return;
  }
  // Start converting to a bitmap.
  RequestBitmapForMainFrame(std::move(callback));
}

void FullScreenshotter::RequestBitmapForMainFrame(CaptureScreenshotCallback callback) {
  // Passing an empty `gfx::Rect` allows us to get a bitmap for the full page.
  paint_preview_compositor_client_->BitmapForMainFrame(
      gfx::Rect(), /*scale_factor=*/1.0,
      base::BindOnce(&FullScreenshotter::OnBitmapReceived,
                     weak_ptr_factory_.GetWeakPtr(), std::move(callback)));
}

void FullScreenshotter::OnBitmapReceived(
      CaptureScreenshotCallback callback,
    paint_preview::mojom::PaintPreviewCompositor::BitmapStatus status,
    const SkBitmap& bitmap) {
  if (status != paint_preview::mojom::PaintPreviewCompositor::BitmapStatus::
                    kSuccess ||
      bitmap.empty()) {
    std::move(callback).Run(base::unexpected(base::StringPrintf("Failed to get bitmap (BitmapStatus=%d)",
                                  static_cast<int>(status))));
    return;
  }

  base::ThreadPool::PostTask(
      FROM_HERE, {base::TaskPriority::BEST_EFFORT, base::MayBlock()},
      base::BindOnce(&WriteBitmapToPng, bitmap, base::BindPostTaskToCurrentDefault(std::move(callback))));
}

} // namespace ai_chat
