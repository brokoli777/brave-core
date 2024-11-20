// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/brave_component_updater/browser/component_contents_verifier.h"

#include <utility>

#include "base/check_is_test.h"
#include "base/files/file.h"
#include "base/no_destructor.h"
#include "extensions/buildflags/buildflags.h"

namespace brave_component_updater {

ComponentContentsVerifier::ComponentLock::ComponentLock() = default;
ComponentContentsVerifier::ComponentLock::ComponentLock(
    ComponentContentsVerifier::ComponentLock&&) = default;

ComponentContentsVerifier::ComponentLock::~ComponentLock() {
  for (auto& f : locked_files_) {
    f.Unlock();
  }
}

bool ComponentContentsVerifier::ComponentLock::LockFile(
    const base::FilePath& path) {
  base::File f(path, base::File::FLAG_OPEN | base::File::FLAG_WRITE);
  if (f.IsValid() &&
      (f.Lock(base::File::LockMode::kShared) == base::File::FILE_OK)) {
    locked_files_.push_back(std::move(f));
    return true;
  }
  return false;
}

ComponentContentsVerifier::ComponentContentsVerifier() = default;
ComponentContentsVerifier::~ComponentContentsVerifier() = default;

// static
void ComponentContentsVerifier::SetupVerifier(
    ComponentContentsVerifier::ComponentVerifier verifier) {
  if (GetInstance()->verifier_) {
    CHECK_IS_TEST();
  }
  GetInstance()->verifier_ = std::move(verifier);
}

// static
ComponentContentsVerifier* ComponentContentsVerifier::GetInstance() {
  static base::NoDestructor<ComponentContentsVerifier> instance;
  return instance.get();
}

// Must be called on the MAY_BLOCK sequence.
std::unique_ptr<const ComponentContentsVerifier::ComponentLock>
ComponentContentsVerifier::VerifyAndLock(const base::FilePath& component_root) {
#if !BUILDFLAG(ENABLE_EXTENSIONS)
  return std::make_unique<ComponentLock>();
#else
  CHECK(verifier_);
  return verifier_.Run(component_root);
#endif
}

}  // namespace brave_component_updater
