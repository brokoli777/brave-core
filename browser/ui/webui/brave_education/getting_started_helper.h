/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_BRAVE_EDUCATION_GETTING_STARTED_HELPER_H_
#define BRAVE_BROWSER_UI_WEBUI_BRAVE_EDUCATION_GETTING_STARTED_HELPER_H_

#include <optional>

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "brave/browser/ui/webui/brave_education/education_server_checker.h"
#include "brave/components/brave_education/common/education_urls.h"
#include "url/gurl.h"

class Profile;

namespace brave_education {

// A helper for determining the "getting started" WebUI URL for a given profile.
class GettingStartedHelper {
 public:
  explicit GettingStartedHelper(Profile* profile);
  ~GettingStartedHelper();

  GettingStartedHelper(const GettingStartedHelper&) = delete;
  GettingStartedHelper& operator=(const GettingStartedHelper&) = delete;

  using GetEducationURLCallback = base::OnceCallback<void(std::optional<GURL>)>;

  // Asynchronously returns a "getting started" education WebUI URL. Returns
  // `std::nullopt` if a "getting started" URL is not available (e.g. if
  // the network is not available or the web server is not returning a valid
  // response).
  void GetEducationURL(GetEducationURLCallback callback);

 private:
  void OnCheckerResult(GetEducationURLCallback callback,
                       EducationPageType page_type,
                       bool available);

  void RunCallbacks(std::optional<GURL> webui_url);

  EducationServerChecker server_checker_;
};

}  // namespace brave_education

#endif  // BRAVE_BROWSER_UI_WEBUI_BRAVE_EDUCATION_GETTING_STARTED_HELPER_H_
