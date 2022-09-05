/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_IPFS_IPFS_JSON_PARSER_H_
#define BRAVE_COMPONENTS_IPFS_IPFS_JSON_PARSER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "brave/components/ipfs/addresses_config.h"
#include "brave/components/ipfs/import/imported_data.h"
#include "brave/components/ipfs/node_info.h"
#include "brave/components/ipfs/pin/ipfs_pin_rpc_types.h"
#include "brave/components/ipfs/repo_stats.h"

class IPFSJSONParser {
 public:
  static bool GetRemovePinsResultFromJSON(
      const std::string& json,
      ipfs::RemovePinResult* add_pin_result);
  static bool GetPinsResultFromJSON(const std::string& json,
                                    ipfs::GetPinsResult* add_pin_result);
  static bool GetPeersFromJSON(const std::string& json,
                               std::vector<std::string>* peers);
  static bool GetAddressesConfigFromJSON(const std::string& json,
                                         ipfs::AddressesConfig* config);
  static bool GetRepoStatsFromJSON(const std::string& json,
                                   ipfs::RepoStats* config);
  static bool GetNodeInfoFromJSON(const std::string& json,
                                  ipfs::NodeInfo* info);
  static bool GetGarbageCollectionFromJSON(const std::string& json,
                                           std::string* error);
  static bool GetImportResponseFromJSON(const std::string& json,
                                        ipfs::ImportedData* data);
  static bool GetParseKeysFromJSON(
      const std::string& json,
      std::unordered_map<std::string, std::string>* keys);
  static bool GetParseSingleKeyFromJSON(const std::string& json,
                                        std::string* name,
                                        std::string* value);
  static bool GetPeersFromConfigJSON(const std::string& json,
                                     std::vector<std::string>* peers);
  static std::string PutNewPeerToConfigJSON(const std::string& json,
                                            const std::string& peer);
  static std::string RemovePeerFromConfigJSON(const std::string& json,
                                              const std::string& peer_id,
                                              const std::string& address);

  // Remote pin services
  static bool GetGetRemotePinServicesResult(
      const std::string& json,
      ipfs::GetRemotePinServicesResult* result);

  // Local pins
  static bool GetAddPinsResultFromJSON(const std::string& json,
                                       ipfs::AddPinResult* add_pin_result);
  static bool GetGetPinsResultFromJSON(const std::string& json,
                                       ipfs::GetPinsResult* result);

  // Remote pins
  static bool GetAddRemotePinResultFromJson(const std::string& json,
                                            ipfs::AddRemotePinResult* result);
  static bool GetGetRemotePinsResultFromJson(const std::string& json,
                                             ipfs::GetRemotePinResult* result);
};

#endif  // BRAVE_COMPONENTS_IPFS_IPFS_JSON_PARSER_H_
