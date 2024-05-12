/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/creatives/conversions/creative_set_conversion_util.h"

#include "base/time/time.h"
#include "brave/components/brave_ads/core/internal/ad_units/ad_unittest_constants.h"
#include "brave/components/brave_ads/core/internal/ad_units/ad_unittest_util.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_base.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_time_util.h"
#include "brave/components/brave_ads/core/internal/creatives/conversions/creative_set_conversion_unittest_util.h"
#include "brave/components/brave_ads/core/internal/user_engagement/ad_events/ad_event_builder.h"
#include "brave/components/brave_ads/core/internal/user_engagement/conversions/types/verifiable_conversion/verifiable_conversion_unittest_constants.h"
#include "url/gurl.h"

// npm run test -- brave_unit_tests --filter=BraveAds*

namespace brave_ads {

class BraveAdsCreativeSetConversionUtilTest : public UnitTestBase {};

TEST_F(BraveAdsCreativeSetConversionUtilTest,
       GetMatchingCreativeSetConversions) {
  // Arrange
  CreativeSetConversionList creative_set_conversions;

  const CreativeSetConversionInfo creative_set_conversion_1 =
      test::BuildCreativeSetConversion(kCreativeSetId,
                                       /*url_pattern_id=*/"xyzzy-thud",
                                       /*url_pattern=*/"https://foo.com/*",
                                       /*observation_window=*/base::Days(3));
  creative_set_conversions.push_back(creative_set_conversion_1);

  const CreativeSetConversionInfo creative_set_conversion_2 =
      test::BuildCreativeSetConversion(kCreativeSetId,
                                       /*url_pattern_id=*/"xyzzy-thud",
                                       /*url_pattern=*/"https://*.qux.com/fred",
                                       /*observation_window=*/base::Days(7));
  creative_set_conversions.push_back(creative_set_conversion_2);

  const CreativeSetConversionInfo creative_set_conversion_3 =
      test::BuildCreativeSetConversion(kCreativeSetId,
                                       /*url_pattern_id=*/"xyzzy-thud",
                                       /*url_pattern=*/"https://bar.com/foo",
                                       /*observation_window=*/base::Days(30));
  creative_set_conversions.push_back(creative_set_conversion_3);

  const CreativeSetConversionInfo creative_set_conversion_4 =
      test::BuildCreativeSetConversion(kCreativeSetId,
                                       /*url_pattern_id=*/"xyzzy-thud",
                                       /*url_pattern=*/"https://baz.com/",
                                       /*observation_window=*/base::Days(1));
  creative_set_conversions.push_back(creative_set_conversion_4);

  const std::vector<GURL> redirect_chain = {
      GURL("https://baz.com/" /*matches creative_set_conversion_4*/),
      GURL("https://foo.com/bar" /*matches creative_set_conversion_1*/),
      GURL("https://www.qux.com/fred" /*matches creative_set_conversion_2*/),
      GURL("https://quux.com/corge/grault"), GURL("https://garbly.com/waldo")};

  // Act & Assert
  const CreativeSetConversionList expected_matching_creative_set_conversions = {
      creative_set_conversion_1, creative_set_conversion_2,
      creative_set_conversion_4};
  EXPECT_EQ(expected_matching_creative_set_conversions,
            GetMatchingCreativeSetConversions(creative_set_conversions,
                                              redirect_chain));
}

TEST_F(BraveAdsCreativeSetConversionUtilTest, GetCreativeSetConversionCounts) {
  // Arrange
  const AdInfo ad = test::BuildAd(AdType::kNotificationAd,
                                  /*should_use_random_uuids=*/true);

  AdEventList ad_events;

  const AdEventInfo ad_event_1 = BuildAdEvent(
      ad, ConfirmationType::kServedImpression, /*created_at=*/Now());
  ad_events.push_back(ad_event_1);

  AdEventInfo ad_event_2 =
      BuildAdEvent(ad, ConfirmationType::kConversion, /*created_at=*/Now());
  ad_event_2.creative_set_id = "4e83a23c-1194-40f8-8fdc-2f38d7ed75c8";
  ad_events.push_back(ad_event_2);

  const AdEventInfo ad_event_3 = BuildAdEvent(
      ad, ConfirmationType::kViewedImpression, /*created_at=*/Now());
  ad_events.push_back(ad_event_3);

  const AdEventInfo ad_event_4 =
      BuildAdEvent(ad, ConfirmationType::kConversion, /*created_at=*/Now());
  ad_events.push_back(ad_event_4);
  ad_events.push_back(ad_event_4);

  // Act & Assert
  const CreativeSetConversionCountMap expected_creative_set_conversion_counts =
      {{ad_event_2.creative_set_id, 1}, {ad_event_4.creative_set_id, 2}};
  EXPECT_EQ(expected_creative_set_conversion_counts,
            GetCreativeSetConversionCounts(ad_events));
}

TEST_F(BraveAdsCreativeSetConversionUtilTest,
       SortCreativeSetConversionsIntoBuckets) {
  // Arrange
  CreativeSetConversionList creative_set_conversions;

  const CreativeSetConversionInfo creative_set_conversion_1 =
      test::BuildCreativeSetConversion(
          kCreativeSetId,
          /*url_pattern_id=*/"xyzzy-thud",
          /*url_pattern=*/"https://foo.com/*",
          /*observation_window=*/base::Days(3));  // Bucket #1
  creative_set_conversions.push_back(creative_set_conversion_1);

  const CreativeSetConversionInfo creative_set_conversion_2 =
      test::BuildCreativeSetConversion(
          /*creative_set_id=*/"4e83a23c-1194-40f8-8fdc-2f38d7ed75c8",
          /*url_pattern_id=*/"xyzzy-thud",
          /*url_pattern=*/"https://www.qux.com/",
          /*observation_window=*/base::Days(7));  // Bucket #2
  creative_set_conversions.push_back(creative_set_conversion_2);

  const CreativeSetConversionInfo creative_set_conversion_3 =
      test::BuildCreativeSetConversion(
          kCreativeSetId,
          /*url_pattern_id=*/"xyzzy-thud",
          /*url_pattern=*/"https://baz.com/",
          /*observation_window=*/base::Days(30));  // Bucket #1
  creative_set_conversions.push_back(creative_set_conversion_3);

  // Act & Assert
  CreativeSetConversionBucketMap expected_buckets;
  expected_buckets.insert(  // Bucket #1
      {kCreativeSetId, {creative_set_conversion_1, creative_set_conversion_3}});
  expected_buckets.insert(  // Bucket #2
      {creative_set_conversion_2.id, {creative_set_conversion_2}});
  EXPECT_EQ(expected_buckets,
            SortCreativeSetConversionsIntoBuckets(creative_set_conversions));
}

TEST_F(BraveAdsCreativeSetConversionUtilTest,
       SortEmptyCreativeSetConversionsIntoBuckets) {
  // Act & Assert
  EXPECT_THAT(
      SortCreativeSetConversionsIntoBuckets(/*creative_set_conversions=*/{}),
      ::testing::IsEmpty());
}

TEST_F(BraveAdsCreativeSetConversionUtilTest,
       FilterCreativeSetConversionBucketsThatExceedTheCap) {
  // Arrange
  CreativeSetConversionList creative_set_conversions;

  const CreativeSetConversionInfo creative_set_conversion_1 =
      test::BuildCreativeSetConversion(
          kCreativeSetId,
          /*url_pattern_id=*/"xyzzy-thud",
          /*url_pattern=*/"https://foo.com/*",
          /*observation_window=*/base::Days(3));  // Bucket #1
  creative_set_conversions.push_back(creative_set_conversion_1);

  const CreativeSetConversionInfo creative_set_conversion_2 =
      test::BuildCreativeSetConversion(
          /*creative_set_id=*/"4e83a23c-1194-40f8-8fdc-2f38d7ed75c8",
          /*url_pattern_id=*/"xyzzy-thud",
          /*url_pattern=*/"https://baz.com/",
          /*observation_window=*/base::Days(30));  // Bucket #1
  creative_set_conversions.push_back(creative_set_conversion_2);
  creative_set_conversions.push_back(creative_set_conversion_2);

  const CreativeSetConversionCountMap creative_set_conversion_counts = {
      {creative_set_conversion_1.id, 1}, {creative_set_conversion_2.id, 2}};

  CreativeSetConversionBucketMap creative_set_conversion_buckets =
      SortCreativeSetConversionsIntoBuckets(creative_set_conversions);

  // Act
  FilterCreativeSetConversionBucketsThatExceedTheCap(
      creative_set_conversion_counts, /*creative_set_conversion_cap*/ 2,
      creative_set_conversion_buckets);

  // Assert
  CreativeSetConversionBucketMap expected_creative_set_conversion_buckets = {
      {creative_set_conversion_1.id, {creative_set_conversion_1}}};
  EXPECT_EQ(expected_creative_set_conversion_buckets,
            creative_set_conversion_buckets);
}

TEST_F(BraveAdsCreativeSetConversionUtilTest,
       DoNotFilterCreativeSetConversionBucketsIfCapIsZero) {
  // Arrange
  CreativeSetConversionList creative_set_conversions;

  const CreativeSetConversionInfo creative_set_conversion_1 =
      test::BuildCreativeSetConversion(
          kCreativeSetId,
          /*url_pattern_id=*/"xyzzy-thud",
          /*url_pattern=*/"https://foo.com/*",
          /*observation_window=*/base::Days(3));  // Bucket #1
  creative_set_conversions.push_back(creative_set_conversion_1);

  const CreativeSetConversionInfo creative_set_conversion_2 =
      test::BuildCreativeSetConversion(
          /*creative_set_id=*/"4e83a23c-1194-40f8-8fdc-2f38d7ed75c8",
          /*url_pattern_id=*/"xyzzy-thud",
          /*url_pattern=*/"https://baz.com/",
          /*observation_window=*/base::Days(30));  // Bucket #1
  creative_set_conversions.push_back(creative_set_conversion_2);
  creative_set_conversions.push_back(creative_set_conversion_2);

  const CreativeSetConversionCountMap creative_set_conversion_counts = {
      {creative_set_conversion_1.id, 1}, {creative_set_conversion_2.id, 2}};

  CreativeSetConversionBucketMap creative_set_conversion_buckets =
      SortCreativeSetConversionsIntoBuckets(creative_set_conversions);

  // Act
  FilterCreativeSetConversionBucketsThatExceedTheCap(
      creative_set_conversion_counts, /*creative_set_conversion_cap*/ 0,
      creative_set_conversion_buckets);

  // Assert
  CreativeSetConversionBucketMap expected_creative_set_conversion_buckets = {
      {creative_set_conversion_1.id, {creative_set_conversion_1}},
      {creative_set_conversion_2.id,
       {creative_set_conversion_2, creative_set_conversion_2}}};
  EXPECT_EQ(expected_creative_set_conversion_buckets,
            creative_set_conversion_buckets);
}

TEST_F(BraveAdsCreativeSetConversionUtilTest,
       GetCreativeSetConversionsWithinObservationWindow) {
  // Arrange
  const AdInfo ad = test::BuildAd(AdType::kNotificationAd,
                                  /*should_use_random_uuids=*/false);
  const AdEventInfo ad_event =
      BuildAdEvent(ad, ConfirmationType::kConversion, /*created_at=*/Now());

  AdvanceClockBy(base::Days(3) + base::Milliseconds(1));

  CreativeSetConversionList creative_set_conversions;

  const CreativeSetConversionInfo creative_set_conversion_1 =
      test::BuildVerifiableCreativeSetConversion(
          kCreativeSetId,
          /*url_pattern_id=*/"xyzzy-thud",
          /*url_pattern=*/"https://foo.com/*",
          /*observation_window=*/base::Days(7),
          kVerifiableConversionAdvertiserPublicKey);
  creative_set_conversions.push_back(creative_set_conversion_1);

  const CreativeSetConversionInfo creative_set_conversion_2 =
      test::BuildCreativeSetConversion(
          /*creative_set_id=*/"4e83a23c-1194-40f8-8fdc-2f38d7ed75c8",
          /*url_pattern_id=*/"xyzzy-thud",
          /*url_pattern=*/"https://www.qux.com/",
          /*observation_window=*/base::Days(3));
  creative_set_conversions.push_back(creative_set_conversion_2);

  // Act & Assert
  const CreativeSetConversionList expected_unexpired_creative_set_conversions =
      {creative_set_conversion_1};
  EXPECT_EQ(expected_unexpired_creative_set_conversions,
            GetCreativeSetConversionsWithinObservationWindow(
                creative_set_conversions, ad_event));
}

}  // namespace brave_ads
