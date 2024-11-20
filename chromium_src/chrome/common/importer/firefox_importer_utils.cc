/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "base/files/file_path.h"
// Clang format wants to place this header after the .cc file for some reason.
// clang-format off
#include "brave/grit/brave_generated_resources.h"
#include "build/build_config.h"

#if BUILDFLAG(IS_ANDROID)
// We don't use firefox importer on Android, so just return an empty path to avoid linker error. 
base::FilePath GetProfilesINI() {
  return base::FilePath();
}
#endif // !BUILDFLAG(IS_ANDROID)

#include "src/chrome/common/importer/firefox_importer_utils.cc"
// clang-format on
