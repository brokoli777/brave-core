/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/components/safe_browsing/core/common/features.cc"

#include "base/feature_override.h"
#include "build/build_config.h"

namespace safe_browsing {

OVERRIDE_FEATURE_DEFAULT_STATES({{
    {kExtensionTelemetryDisableOffstoreExtensions,
     base::FEATURE_DISABLED_BY_DEFAULT},
    {kExtensionTelemetryForEnterprise, base::FEATURE_DISABLED_BY_DEFAULT},
    {kExtensionTelemetryTabsApiSignal, base::FEATURE_DISABLED_BY_DEFAULT},
    {kGooglePlayProtectInApkTelemetry, base::FEATURE_DISABLED_BY_DEFAULT},
#if BUILDFLAG(IS_ANDROID)
    {kSafeBrowsingNewGmsApiForBrowseUrlDatabaseCheck,
     base::FEATURE_DISABLED_BY_DEFAULT},
#endif  // BUILDFLAG(IS_ANDROID)
}});

}  // namespace safe_browsing
