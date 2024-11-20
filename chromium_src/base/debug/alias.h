/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_BASE_DEBUG_ALIAS_H_
#define BRAVE_CHROMIUM_SRC_BASE_DEBUG_ALIAS_H_

#include "base/containers/span.h"
#include "base/memory/raw_ptr_exclusion.h"
#include "base/memory/stack_allocated.h"
#include "base/ranges/algorithm.h"

#include "src/base/debug/alias.h"  // IWYU pragma: export

namespace base::debug {

// StackObjectCopy creates a byte-for-byte copy of an object on the stack,
// allowing the object to be inspected in crash dumps even if the original
// object is optimized away by the compiler or is stored in protected memory.
// The copy maintains proper alignment and can be viewed as the original type in
// a debugger.
template <typename T>
class StackObjectCopy {
  STACK_ALLOCATED();

 public:
  explicit StackObjectCopy(const T* original) {
    if (original) {
      // SAFETY: `original` is a valid pointer to a T object, and `kSize` is the
      // size of T.
      auto original_object_memory = UNSAFE_BUFFERS(
          base::make_span(reinterpret_cast<const uint8_t*>(original), kSize));
      base::span(buffer_).copy_from(std::move(original_object_memory));
    } else {
      base::ranges::fill(buffer_, 0);
    }
  }

  StackObjectCopy(const StackObjectCopy&) = delete;
  StackObjectCopy& operator=(const StackObjectCopy&) = delete;

 private:
  constexpr static size_t kSize = sizeof(T);

  alignas(T) uint8_t buffer_[kSize];
  RAW_PTR_EXCLUSION const T* const typed_view_ =
      reinterpret_cast<const T*>(buffer_);
};

}  // namespace base::debug

// DEBUG_ALIAS_FOR_OBJECT creates a stack copy of an object and ensures it
// remains in memory for crash dumps. This is useful when you need to ensure an
// object's state is captured in crash reports, especially when the object might
// otherwise be optimized away or is not directly accessible.
//
// Usage: DEBUG_ALIAS_FOR_OBJECT(alias_name, pointer_to_object);
#define DEBUG_ALIAS_FOR_OBJECT(var_name, object)         \
  const ::base::debug::StackObjectCopy var_name(object); \
  ::base::debug::Alias(&var_name)

#endif  // BRAVE_CHROMIUM_SRC_BASE_DEBUG_ALIAS_H_
