/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/public/units/new_tab_page_ad/new_tab_page_ad_value_util.h"

#include <optional>
#include <string>
#include <utility>

#include "brave/components/brave_ads/core/public/units/new_tab_page_ad/new_tab_page_ad_constants.h"
#include "brave/components/brave_ads/core/public/units/new_tab_page_ad/new_tab_page_ad_info.h"

namespace brave_ads {

namespace {
constexpr char kTypeKey[] = "type";
}  // namespace

base::Value::Dict NewTabPageAdToValue(const NewTabPageAdInfo& ad) {
  auto dict =
      base::Value::Dict()
          .Set(kTypeKey, ToString(ad.type))
          .Set(kNewTabPageAdPlacementIdKey, ad.placement_id)
          .Set(kNewTabPageAdCreativeInstanceIdKey, ad.creative_instance_id)
          .Set(kNewTabPageAdCreativeSetIdKey, ad.creative_set_id)
          .Set(kNewTabPageAdCampaignIdKey, ad.campaign_id)
          .Set(kNewTabPageAdAdvertiserIdKey, ad.advertiser_id)
          .Set(kNewTabPageAdSegmentKey, ad.segment)
          .Set(kNewTabPageAdCompanyNameKey, ad.company_name)
          .Set(kNewTabPageAdImageUrlKey, ad.image_url.spec())
          .Set(kNewTabPageAdAltKey, ad.alt)
          .Set(kNewTabPageAdTargetUrlKey, ad.target_url.spec());

  base::Value::List wallpapers;
  for (const NewTabPageAdWallpaperInfo& wallpaper : ad.wallpapers) {
    base::Value::Dict wallpaper_dict;
    wallpaper_dict.Set(kNewTabPageAdImageUrlKey, wallpaper.image_url.spec());

    base::Value::Dict focal_point_dict;
    focal_point_dict.Set(kNewTabPageAdFocalPointXKey, wallpaper.focal_point.x);
    focal_point_dict.Set(kNewTabPageAdFocalPointYKey, wallpaper.focal_point.y);
    wallpaper_dict.Set(kNewTabPageAdFocalPointKey, std::move(focal_point_dict));

    wallpapers.Append(std::move(wallpaper_dict));
  }
  dict.Set(kNewTabPageAdWallpapersKey, std::move(wallpapers));

  return dict;
}

NewTabPageAdInfo NewTabPageAdFromValue(const base::Value::Dict& dict) {
  NewTabPageAdInfo ad;

  if (const auto* const value = dict.FindString(kTypeKey)) {
    ad.type = ParseAdType(*value);
  }

  if (const auto* const value = dict.FindString(kNewTabPageAdPlacementIdKey)) {
    ad.placement_id = *value;
  }

  if (const auto* const value =
          dict.FindString(kNewTabPageAdCreativeInstanceIdKey)) {
    ad.creative_instance_id = *value;
  }

  if (const auto* const value =
          dict.FindString(kNewTabPageAdCreativeSetIdKey)) {
    ad.creative_set_id = *value;
  }

  if (const auto* const value = dict.FindString(kNewTabPageAdCampaignIdKey)) {
    ad.campaign_id = *value;
  }

  if (const auto* const value = dict.FindString(kNewTabPageAdAdvertiserIdKey)) {
    ad.advertiser_id = *value;
  }

  if (const auto* const value = dict.FindString(kNewTabPageAdSegmentKey)) {
    ad.segment = *value;
  }

  if (const auto* const value = dict.FindString(kNewTabPageAdCompanyNameKey)) {
    ad.company_name = *value;
  }

  if (const auto* const value = dict.FindString(kNewTabPageAdImageUrlKey)) {
    ad.image_url = GURL(*value);
  }

  if (const auto* const value = dict.FindString(kNewTabPageAdAltKey)) {
    ad.alt = *value;
  }

  if (const auto* const wallpapers_list =
          dict.FindList(kNewTabPageAdWallpapersKey)) {
    for (const auto& item : *wallpapers_list) {
      const auto* const item_dict = item.GetIfDict();
      if (!item_dict) {
        continue;
      }

      const std::string* const image_url =
          item_dict->FindString(kNewTabPageAdImageUrlKey);
      if (!image_url) {
        continue;
      }

      const auto* const focal_point_dict =
          item_dict->FindDict(kNewTabPageAdFocalPointKey);
      if (!focal_point_dict) {
        continue;
      }

      const std::optional<int> focal_point_x =
          focal_point_dict->FindInt(kNewTabPageAdFocalPointXKey);
      if (!focal_point_x) {
        continue;
      }

      const std::optional<int> focal_point_y =
          focal_point_dict->FindInt(kNewTabPageAdFocalPointYKey);
      if (!focal_point_y) {
        continue;
      }

      NewTabPageAdWallpaperInfo wallpaper;
      wallpaper.image_url = GURL(*image_url);
      wallpaper.focal_point.x = *focal_point_x;
      wallpaper.focal_point.y = *focal_point_y;

      ad.wallpapers.push_back(wallpaper);
    }
  }

  if (const auto* const value = dict.FindString(kNewTabPageAdTargetUrlKey)) {
    ad.target_url = GURL(*value);
  }

  return ad;
}

}  // namespace brave_ads
