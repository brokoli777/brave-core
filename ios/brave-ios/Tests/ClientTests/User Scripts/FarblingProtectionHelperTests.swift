// Copyright 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

import CryptoKit
import XCTest

@testable import Brave

@MainActor class FarblingProtectionHelperTests: XCTestCase {
  func testGivenTheSameRandomManagerThenSameFakePluginData() throws {
    // Given
    // Same random manager
    let sessionKey = SymmetricKey(size: .bits256)
    let randomConfiguration = RandomConfiguration(etld: "example.com", sessionKey: sessionKey)
    let encoder = JSONEncoder()
    encoder.outputFormatting = .sortedKeys  // To ensure stable comparisons
    // Then
    // Same results
    XCTAssertEqual(
      try FarblingProtectionHelper.makeFarblingParams(from: randomConfiguration, encoder: encoder),
      try FarblingProtectionHelper.makeFarblingParams(from: randomConfiguration, encoder: encoder)
    )
  }

  func testGivenDifferentRandomManagerThenDifferentFakePluginData() throws {
    // Given
    // Different random manager
    let sessionKey = SymmetricKey(size: .bits256)
    let firstRandomConfiguration = RandomConfiguration(etld: "example.com", sessionKey: sessionKey)
    let secondRandomConfiguration = RandomConfiguration(etld: "brave.com", sessionKey: sessionKey)
    let encoder = JSONEncoder()
    encoder.outputFormatting = .sortedKeys  // To ensure stable comparisons

    // Then
    // Different results
    XCTAssertNotEqual(
      try FarblingProtectionHelper.makeFarblingParams(
        from: firstRandomConfiguration,
        encoder: encoder
      ),
      try FarblingProtectionHelper.makeFarblingParams(
        from: secondRandomConfiguration,
        encoder: encoder
      )
    )
  }
}
