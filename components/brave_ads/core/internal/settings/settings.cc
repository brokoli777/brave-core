/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/settings/settings.h"

#include "brave/components/brave_ads/common/notification_ad_feature.h"
#include "brave/components/brave_ads/common/pref_names.h"
#include "brave/components/brave_ads/core/internal/ads_client_helper.h"
#include "brave/components/brave_news/common/pref_names.h"
#include "brave/components/brave_rewards/common/pref_names.h"
#include "brave/components/ntp_background_images/common/pref_names.h"

namespace brave_ads {

bool UserHasJoinedBraveRewards() {
  return AdsClientHelper::GetInstance()->GetBooleanPref(
      brave_rewards::prefs::kEnabled);
}

bool UserHasOptedInToBraveNewsAds() {
  return AdsClientHelper::GetInstance()->GetBooleanPref(
             brave_news::prefs::kBraveNewsOptedIn) &&
         AdsClientHelper::GetInstance()->GetBooleanPref(
             brave_news::prefs::kNewTabPageShowToday);
}

bool UserHasOptedInToNewTabPageAds() {
  return AdsClientHelper::GetInstance()->GetBooleanPref(
             ntp_background_images::prefs::kNewTabPageShowBackgroundImage) &&
         AdsClientHelper::GetInstance()->GetBooleanPref(
             ntp_background_images::prefs::
                 kNewTabPageShowSponsoredImagesBackgroundImage);
}

bool UserHasOptedInToNotificationAds() {
  return UserHasJoinedBraveRewards() &&
         AdsClientHelper::GetInstance()->GetBooleanPref(
             prefs::kOptedInToNotificationAds);
}

int GetMaximumNotificationAdsPerHour() {
  int ads_per_hour =
      static_cast<int>(AdsClientHelper::GetInstance()->GetInt64Pref(
          prefs::kMaximumNotificationAdsPerHour));

  if (ads_per_hour == -1) {
    ads_per_hour = kDefaultNotificationAdsPerHour.Get();
  }

  return ads_per_hour;
}

}  // namespace brave_ads
