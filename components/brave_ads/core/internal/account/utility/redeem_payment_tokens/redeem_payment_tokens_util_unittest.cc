/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/account/utility/redeem_payment_tokens/redeem_payment_tokens_util.h"

#include "base/time/time.h"
#include "brave/components/brave_ads/common/pref_names.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_base.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_time_util.h"

// npm run test -- brave_unit_tests --filter=BraveAds*

namespace brave_ads {

class BraveAdsRedeemPaymentTokensUtilTest : public UnitTestBase {};

TEST_F(BraveAdsRedeemPaymentTokensUtilTest, SetNextTokenRedemptionAt) {
  // Arrange
  SetNextTokenRedemptionAt(DistantFuture());

  // Act

  // Assert
  EXPECT_FALSE(
      ads_client_mock_.GetTimePref(prefs::kNextTokenRedemptionAt).is_null());
}

TEST_F(BraveAdsRedeemPaymentTokensUtilTest, ScheduleNextTokenRedemptionAt) {
  // Arrange

  // Act

  // Assert
  EXPECT_FALSE(ScheduleNextTokenRedemptionAt().is_null());
}

TEST_F(BraveAdsRedeemPaymentTokensUtilTest,
       CalculateDelayBeforeRedeemingTokens) {
  // Arrange
  SetNextTokenRedemptionAt(Now() + base::Days(1));

  // Act

  // Assert
  EXPECT_EQ(base::Days(1), CalculateDelayBeforeRedeemingTokens());
}

TEST_F(BraveAdsRedeemPaymentTokensUtilTest,
       CalculateDelayBeforeRedeemingTokensIfHasNotPreviouslyRedeemedTokens) {
  // Arrange
  SetNextTokenRedemptionAt({});

  // Act

  // Assert
  EXPECT_FALSE(ScheduleNextTokenRedemptionAt().is_null());
}

TEST_F(BraveAdsRedeemPaymentTokensUtilTest,
       CalculateDelayBeforeRedeemingTokensIfShouldHaveRedeemedTokensInThePast) {
  // Arrange
  SetNextTokenRedemptionAt(DistantPast());

  // Act
  EXPECT_EQ(base::Minutes(1), CalculateDelayBeforeRedeemingTokens());
}

TEST_F(BraveAdsRedeemPaymentTokensUtilTest,
       CalculateMinimumDelayBeforeRedeemingTokens) {
  // Arrange
  SetNextTokenRedemptionAt(Now() + base::Milliseconds(1));

  // Act

  // Assert
  EXPECT_EQ(base::Minutes(1), CalculateDelayBeforeRedeemingTokens());
}

}  // namespace brave_ads
