// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_COMPONENTS_BRAVE_COMPONENT_UPDATER_BROWSER_COMPONENT_CONTENTS_VERIFIER_H_
#define BRAVE_COMPONENTS_BRAVE_COMPONENT_UPDATER_BROWSER_COMPONENT_CONTENTS_VERIFIER_H_

#include <memory>
#include <vector>

#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/functional/callback.h"
#include "base/task/sequenced_task_runner.h"

namespace base {
template <typename T>
class NoDestructor;
}  // namespace base

namespace brave_component_updater {

class ComponentContentsVerifier {
 public:
  // Must be deleted the MAY_BLOCK sequence.
  class ComponentLock {
   public:
    ComponentLock();
    ComponentLock(const ComponentLock&) = delete;
    ComponentLock(ComponentLock&&);
    ComponentLock& operator=(const ComponentLock&) = delete;
    ComponentLock& operator=(ComponentLock&&) = delete;
    ~ComponentLock();

    bool LockFile(const base::FilePath& path);

   private:
    std::vector<base::File> locked_files_;
  };

  using ComponentVerifier =
      base::RepeatingCallback<std::unique_ptr<const ComponentLock>(
          const base::FilePath& component_root)>;

  static void SetupVerifier(ComponentVerifier verifier);
  static ComponentContentsVerifier* GetInstance();

  // Must be called on the MAY_BLOCK sequence.
  std::unique_ptr<const ComponentLock> VerifyAndLock(
      const base::FilePath& component_root);

 private:
  friend base::NoDestructor<ComponentContentsVerifier>;

  ComponentVerifier verifier_;

  ComponentContentsVerifier();
  ~ComponentContentsVerifier();
};

}  // namespace brave_component_updater

#endif  // BRAVE_COMPONENTS_BRAVE_COMPONENT_UPDATER_BROWSER_COMPONENT_CONTENTS_VERIFIER_H_
