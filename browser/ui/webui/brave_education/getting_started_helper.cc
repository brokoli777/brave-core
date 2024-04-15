/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/brave_education/getting_started_helper.h"

#include <utility>

#include "brave/components/brave_education/common/features.h"

namespace brave_education {

namespace {

std::optional<EducationPageType> GetEducationPageType() {
  if (base::FeatureList::IsEnabled(features::kShowGettingStartedPage)) {
    return EducationPageType::kGettingStarted;
  }
  return std::nullopt;
}

}  // namespace

GettingStartedHelper::GettingStartedHelper(Profile* profile)
    : server_checker_(profile) {}

GettingStartedHelper::~GettingStartedHelper() = default;

void GettingStartedHelper::GetEducationURL(GetEducationURLCallback callback) {
  auto page_type = GetEducationPageType();
  if (!page_type) {
    std::move(callback).Run(std::nullopt);
    return;
  }

  server_checker_.IsServerPageAvailable(
      *page_type, base::BindOnce(&GettingStartedHelper::OnCheckerResult,
                                 base::Unretained(this), std::move(callback)));
}

void GettingStartedHelper::OnCheckerResult(GetEducationURLCallback callback,
                                           EducationPageType page_type,
                                           bool available) {
  if (available) {
    std::move(callback).Run(GetEducationPageBrowserURL(page_type));
  } else {
    std::move(callback).Run(std::nullopt);
  }
}

}  // namespace brave_education
