/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/infobars/request_otr_infobar_delegate.h"

#include <memory>

#include "base/check_op.h"
#include "brave/browser/request_otr/request_otr_service_factory.h"
#include "brave/components/l10n/common/localization_util.h"
#include "brave/components/request_otr/browser/request_otr_service.h"
#include "build/build_config.h"
#include "chrome/browser/infobars/confirm_infobar_creator.h"
#include "chrome/grit/generated_resources.h"
#include "components/grit/brave_components_resources.h"
#include "components/grit/brave_components_strings.h"
#include "components/infobars/content/content_infobar_manager.h"
#include "components/infobars/core/infobar.h"
#include "components/vector_icons/vector_icons.h"
#include "content/public/browser/web_contents.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/views/vector_icons.h"

// static
void RequestOTRInfoBarDelegate::Create(
    infobars::ContentInfoBarManager* infobar_manager,
    const GURL& url) {
  infobar_manager->AddInfoBar(
      CreateConfirmInfoBar(std::unique_ptr<ConfirmInfoBarDelegate>(
          new RequestOTRInfoBarDelegate(url))));
}

RequestOTRInfoBarDelegate::RequestOTRInfoBarDelegate(const GURL& url)
    : ConfirmInfoBarDelegate(), url_(url) {}

RequestOTRInfoBarDelegate::~RequestOTRInfoBarDelegate() {}

infobars::InfoBarDelegate::InfoBarIdentifier
RequestOTRInfoBarDelegate::GetIdentifier() const {
  return BRAVE_REQUEST_OTR_INFOBAR_DELEGATE;
}

const gfx::VectorIcon& RequestOTRInfoBarDelegate::GetVectorIcon() const {
  return views::kInfoIcon;
}

std::u16string RequestOTRInfoBarDelegate::GetMessageText() const {
  return l10n_util::GetStringUTF16(IDS_REQUEST_OTR_INFOBAR_MESSAGE);
}

int RequestOTRInfoBarDelegate::GetButtons() const {
  return BUTTON_OK;
}

std::u16string RequestOTRInfoBarDelegate::GetButtonLabel(
    InfoBarButton button) const {
  DCHECK_EQ(BUTTON_OK, button);
  return l10n_util::GetStringUTF16(IDS_REQUEST_OTR_INFOBAR_BUTTON);
}

bool RequestOTRInfoBarDelegate::Accept() {
  content::WebContents* web_contents =
      infobars::ContentInfoBarManager::WebContentsFromInfoBar(infobar());
  request_otr::RequestOTRService* request_otr_service =
      request_otr::RequestOTRServiceFactory::GetForBrowserContext(
          web_contents->GetBrowserContext());
  DCHECK(request_otr_service);
  request_otr_service->SetOTR(url_, /*enabled*/ false);
  web_contents->GetController().Reload(content::ReloadType::NORMAL, true);

  return true;
}
