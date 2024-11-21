// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/brave_wallet/browser/internal/orchard_test_utils.h"

#include "base/memory/ptr_util.h"
#include "brave/components/brave_wallet/browser/zcash/rust/orchard_decoded_blocks_bundle.h"

namespace brave_wallet {

// static
OrchardBlockScanner::Result OrchardTestUtils::CreateResultForTesting(
    const OrchardTreeState& tree_state,
    const std::vector<OrchardCommitment>& commitments) {
  auto builder = orchard::OrchardTestUtils::CreateTestingBuilder();
  for (const auto& commitment : commitments) {
    builder->AddCommitment(commitment);
  }
  builder->SetPriorTreeState(tree_state);
  return OrchardBlockScanner::Result{{}, {}, builder->Complete()};
}

OrchardTestUtils::OrchardTestUtils() {
  orchard_test_utils_impl_ = orchard::OrchardTestUtils::Create();
}

OrchardTestUtils::~OrchardTestUtils() {}

OrchardCommitmentValue OrchardTestUtils::CreateMockCommitmentValue(
    uint32_t position,
    uint32_t rseed) {
  return orchard_test_utils_impl_->CreateMockCommitmentValue(position, rseed);
}

}  // namespace brave_wallet
