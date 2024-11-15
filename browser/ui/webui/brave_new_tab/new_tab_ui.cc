// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/webui/brave_new_tab/new_tab_ui.h"

#include <utility>

#include "brave/browser/ntp_background/brave_ntp_custom_background_service_factory.h"
#include "brave/browser/ntp_background/custom_background_file_manager.h"
#include "brave/browser/ntp_background/view_counter_service_factory.h"
#include "brave/browser/ui/webui/brave_new_tab/custom_image_chooser.h"
#include "brave/browser/ui/webui/brave_new_tab/new_tab_page_handler.h"
#include "brave/browser/ui/webui/brave_webui_source.h"
#include "brave/components/brave_new_tab/resources/grit/brave_new_tab_generated_map.h"
#include "brave/components/brave_private_cdn/private_cdn_request_helper.h"
#include "brave/components/l10n/common/localization_util.h"
#include "brave/components/ntp_background_images/browser/ntp_custom_images_source.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/search_engines/template_url_service_factory.h"
#include "chrome/browser/ui/tabs/public/tab_interface.h"
#include "chrome/browser/ui/webui/searchbox/realbox_handler.h"
#include "chrome/browser/ui/webui/webui_util.h"
#include "components/grit/brave_components_resources.h"
#include "components/grit/brave_components_strings.h"
#include "components/strings/grit/components_strings.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui.h"
#include "content/public/browser/web_ui_data_source.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "ui/base/webui/web_ui_util.h"

namespace brave_new_tab {

namespace {

static constexpr webui::LocalizedString kStrings[] = {
    {"backgroundSettingsTitle", IDS_BRAVE_NEW_TAB_BACKGROUND_IMAGE},
    {"braveBackgroundLabel", IDS_BRAVE_NEW_TAB_BRAVE_BACKGROUND},
    {"customBackgroundLabel",
     IDS_BRAVE_NEW_TAB_CUSTOM_BACKGROUND_IMAGE_OPTION_TITLE},
    {"customBackgroundTitle",
     IDS_BRAVE_NEW_TAB_CUSTOM_BACKGROUND_IMAGE_OPTION_TITLE},
    {"customizeSearchEnginesLink",
     IDS_BRAVE_NEW_TAB_SEARCH_CUSTOMIZE_SEARCH_ENGINES},
    {"enabledSearchEnginesLabel",
     IDS_BRAVE_NEW_TAB_SEARCH_ENABLE_SEARCH_ENGINES_TITLE},
    {"gradientBackgroundLabel", IDS_BRAVE_NEW_TAB_GRADIENT_COLOR},
    {"gradientBackgroundTitle", IDS_BRAVE_NEW_TAB_GRADIENT_COLOR},
    {"randomizeBackgroundLabel",
     IDS_BRAVE_NEW_TAB_REFRESH_BACKGROUND_ON_NEW_TAB},
    {"searchAskLeoDescription", IDS_OMNIBOX_ASK_LEO_DESCRIPTION},
    {"searchBoxPlaceholderText",
     IDS_BRAVE_NEW_TAB_SEARCH_NON_BRAVE_PLACEHOLDER},
    {"searchBoxPlaceholderTextBrave",
     IDS_BRAVE_NEW_TAB_SEARCH_BRAVE_PLACEHOLDER},
    {"searchCustomizeEngineListText", IDS_BRAVE_NEW_TAB_SEARCH_CUSTOMIZE_LIST},
    {"searchSettingsTitle", IDS_BRAVE_NEW_TAB_SEARCH},
    {"settingsTitle", IDS_BRAVE_NEW_TAB_DASHBOARD_SETTINGS_TITLE},
    {"showBackgroundsLabel", IDS_BRAVE_NEW_TAB_SHOW_BACKGROUND_IMAGE},
    {"showSearchBoxLabel", IDS_BRAVE_NEW_TAB_SEARCH_SHOW_SETTING},
    {"showSponsoredImagesLabel", IDS_BRAVE_NEW_TAB_BRANDED_WALLPAPER_OPT_IN},
    {"solidBackgroundLabel", IDS_BRAVE_NEW_TAB_SOLID_COLOR},
    {"solidBackgroundTitle", IDS_BRAVE_NEW_TAB_SOLID_COLOR},
    {"uploadBackgroundLabel",
     IDS_BRAVE_NEW_TAB_CUSTOM_BACKGROUND_IMAGE_OPTION_UPLOAD_LABEL},
};

constexpr auto kPcdnImageLoaderTrafficAnnotation =
    net::DefineNetworkTrafficAnnotation("brave_new_tab_pcdn_loader",
                                        R"(
      semantics {
        sender: "Brave New Tab WebUI"
        description: "Fetches resource data from the Brave private CDN."
        trigger: "Loading images on the new tab page."
        data: "No data sent, other than URL of the resource."
        destination: BRAVE_OWNED_SERVICE
      }
      policy {
        cookies_allowed: NO
        setting: "None"
      }
    )");

// Adds support for displaying images stored in the custom background image
// folder.
void AddCustomImageDataSource(Profile* profile) {
  auto* custom_background_service =
      BraveNTPCustomBackgroundServiceFactory::GetForContext(profile);
  if (!custom_background_service) {
    return;
  }
  auto source = std::make_unique<ntp_background_images::NTPCustomImagesSource>(
      custom_background_service);
  content::URLDataSource::Add(profile, std::move(source));
}

}  // namespace

NewTabUI::NewTabUI(content::WebUI* web_ui) : ui::MojoWebUIController(web_ui) {
  auto* profile = Profile::FromWebUI(web_ui);

  auto* source = content::WebUIDataSource::CreateAndAdd(
      Profile::FromWebUI(web_ui), chrome::kChromeUINewTabHost);

  webui::SetupWebUIDataSource(source, kBraveNewTabGenerated,
                              IDR_BRAVE_NEW_TAB_HTML);

  source->OverrideContentSecurityPolicy(
      network::mojom::CSPDirectiveName::ImgSrc,
      "img-src chrome://resources chrome://theme chrome://background-wallpaper "
      "chrome://custom-wallpaper chrome://favicon2 blob: data: 'self';");

  AddBackgroundColorToSource(source, web_ui->GetWebContents());
  AddCustomImageDataSource(profile);

  web_ui->OverrideTitle(
      brave_l10n::GetLocalizedResourceUTF16String(IDS_NEW_TAB_TITLE));

  source->AddLocalizedStrings(kStrings);
}

NewTabUI::~NewTabUI() = default;

void NewTabUI::BindInterface(
    mojo::PendingReceiver<mojom::NewTabPageHandler> pending_receiver) {
  auto* web_contents = web_ui()->GetWebContents();
  auto* navigation_entry =
      web_contents->GetController().GetLastCommittedEntry();
  auto* profile = Profile::FromWebUI(web_ui());

  page_handler_ = std::make_unique<NewTabPageHandler>(
      std::move(pending_receiver),
      std::make_unique<CustomImageChooser>(web_contents),
      std::make_unique<CustomBackgroundFileManager>(profile),
      std::make_unique<brave_private_cdn::PrivateCDNRequestHelper>(
          kPcdnImageLoaderTrafficAnnotation, profile->GetURLLoaderFactory()),
      *tabs::TabInterface::GetFromContents(web_contents), *profile->GetPrefs(),
      *TemplateURLServiceFactory::GetForProfile(profile),
      ntp_background_images::ViewCounterServiceFactory::GetForProfile(profile),
      navigation_entry ? navigation_entry->IsRestored() : false);
}

void NewTabUI::BindInterface(
    mojo::PendingReceiver<searchbox::mojom::PageHandler> pending_receiver) {
  realbox_handler_ = std::make_unique<RealboxHandler>(
      std::move(pending_receiver), Profile::FromWebUI(web_ui()),
      web_ui()->GetWebContents(), /*metrics_reporter=*/nullptr,
      /*lens_searchbox_client=*/nullptr, /*omnibox_controller=*/nullptr);
}

WEB_UI_CONTROLLER_TYPE_IMPL(NewTabUI)

}  // namespace brave_new_tab
