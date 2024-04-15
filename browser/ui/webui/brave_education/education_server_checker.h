/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_BRAVE_EDUCATION_EDUCATION_SERVER_CHECKER_H_
#define BRAVE_BROWSER_UI_WEBUI_BRAVE_EDUCATION_EDUCATION_SERVER_CHECKER_H_

#include <memory>
#include <optional>
#include <string>

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "brave/components/brave_education/common/education_urls.h"

class Profile;

namespace network {
class SimpleURLLoader;
}

namespace brave_education {

// A helper for determining the whether an education server URL is currently
// returning a valid response.
class EducationServerChecker {
 public:
  explicit EducationServerChecker(Profile* profile);
  ~EducationServerChecker();

  EducationServerChecker(const EducationServerChecker&) = delete;
  EducationServerChecker& operator=(const EducationServerChecker&) = delete;

  using IsServerPageAvailableCallback =
      base::OnceCallback<void(EducationPageType, bool)>;

  // Asynchronously fetches content from an education server URL and returns a
  // value indicating whether a successful response was received.
  void IsServerPageAvailable(EducationPageType page_type,
                             IsServerPageAvailableCallback callback);

 private:
  void OnURLResponse(EducationPageType page_type,
                     std::unique_ptr<network::SimpleURLLoader> url_loader,
                     IsServerPageAvailableCallback callback,
                     std::optional<std::string> body);

  raw_ptr<Profile> profile_;
  base::WeakPtrFactory<EducationServerChecker> weak_factory_{this};
};

}  // namespace brave_education

#endif  // BRAVE_BROWSER_UI_WEBUI_BRAVE_EDUCATION_EDUCATION_SERVER_CHECKER_H_
