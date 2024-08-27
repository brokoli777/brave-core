/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "build/build_config.h"

#if BUILDFLAG(IS_MAC)

// The code below replaces Ecdsa::Create(kKeyVersion, kKeyPubBytesBase64) by
// Ecdsa::Create(kBraveKeyVersion, kBraveKeyPubBytesBase64) in upstream's
// request_sender.cc when ShouldUseOmaha4() returns true.

#include "base/base64.h"
#include "brave/components/update_client/features.h"
#include "components/client_update_protocol/ecdsa.h"
#include "components/update_client/request_sender.h"

namespace {

// If you change the following, then you will likely also need to update
// RequestSenderTest::UsesBraveCUPKey.
constexpr int kBraveKeyVersion = 1;
constexpr char kBraveKeyPubBytesBase64[] =
    "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEMZENJfFz9Jph//JXTejVdn5U+ALz"
    "NT/Bht/fvkf2hZ5RionWCLzcxmjV3uh0R3MKLfsgI3w7ukou7m8VhkFQSg==";

}  // namespace

namespace client_update_protocol {

class BraveEcdsa : public Ecdsa {
 public:
  static std::unique_ptr<Ecdsa> Create(int key_version,
                                       const std::string_view& public_key) {
    if (brave::ShouldUseOmaha4()) {
      std::string base64_decoded = GetKey(kBraveKeyPubBytesBase64);
      return Ecdsa::Create(kBraveKeyVersion, base64_decoded);
    } else {
      return Ecdsa::Create(key_version, public_key);
    }
  }

 private:
  static std::string GetKey(const char* key_bytes_base64) {
    // This method is a copy of upstream's RequestSender::GetKey, which is
    // unfortunately private.
    std::string result;
    return base::Base64Decode(std::string(key_bytes_base64), &result)
               ? result
               : std::string();
  }
};

}  // namespace client_update_protocol

#define Ecdsa BraveEcdsa

#endif  // BUILDFLAG(IS_MAC)

#include "src/components/update_client/request_sender.cc"

#if BUILDFLAG(IS_MAC)
#undef Ecdsa
#endif
