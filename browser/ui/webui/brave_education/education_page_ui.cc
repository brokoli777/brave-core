// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/webui/brave_education/education_page_ui.h"

#include <string>
#include <utility>

#include "brave/browser/ui/webui/brave_education/education_page_delegate_desktop.h"
#include "brave/browser/ui/webui/brave_education/education_page_handler.h"
#include "brave/browser/ui/webui/brave_webui_source.h"
#include "brave/components/brave_education/common/education_urls.h"
#include "brave/components/brave_education/resources/grit/brave_education_generated_map.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/tabs/public/tab_interface.h"
#include "chrome/browser/ui/webui/webui_util.h"
#include "chrome/grit/branded_strings.h"
#include "components/grit/brave_components_resources.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui.h"
#include "content/public/browser/web_ui_data_source.h"
#include "services/network/public/mojom/content_security_policy.mojom.h"

namespace brave_education {

EducationPageUI::EducationPageUI(content::WebUI* web_ui, const GURL& url)
    : ui::MojoWebUIController(web_ui) {
  content::WebUIDataSource* source = content::WebUIDataSource::CreateAndAdd(
      Profile::FromWebUI(web_ui), url.host());

  webui::SetupWebUIDataSource(source, kBraveEducationGenerated,
                              IDR_BRAVE_EDUCATION_HTML);

  AddBackgroundColorToSource(source, web_ui->GetWebContents());

  static constexpr webui::LocalizedString kStrings[] = {
      {"headerText", IDS_WELCOME_HEADER}};
  source->AddLocalizedStrings(kStrings);

  // Allow embedding of iframe content from allowed domains.
  source->OverrideContentSecurityPolicy(
      network::mojom::CSPDirectiveName::ChildSrc,
      std::string(EducationPageHandler::kChildSrcDirective));
}

EducationPageUI::~EducationPageUI() = default;

void EducationPageUI::BindInterface(
    mojo::PendingReceiver<EducationPageHandlerFactory> pending_receiver) {
  if (page_factory_receiver_.is_bound()) {
    page_factory_receiver_.reset();
  }
  page_factory_receiver_.Bind(std::move(pending_receiver));
}

void EducationPageUI::CreatePageHandler(
    mojo::PendingReceiver<mojom::EducationPageHandler> handler) {
  auto* web_contents = web_ui()->GetWebContents();
  auto* tab = tabs::TabInterface::GetFromContents(web_contents);
  CHECK(tab);

  page_handler_ = std::make_unique<EducationPageHandler>(
      std::move(handler), Profile::FromWebUI(web_ui()),
      EducationPageTypeFromBrowserURL(web_contents->GetVisibleURL()),
      std::make_unique<EducationPageDelegateDesktop>(*tab));
}

WEB_UI_CONTROLLER_TYPE_IMPL(EducationPageUI)

}  // namespace brave_education
