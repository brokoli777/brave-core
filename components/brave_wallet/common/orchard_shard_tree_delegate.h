/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_WALLET_COMMON_ORCHARD_SHARD_TREE_DELEGATE_H_
#define BRAVE_COMPONENTS_BRAVE_WALLET_COMMON_ORCHARD_SHARD_TREE_DELEGATE_H_

#include <optional>

#include "brave/components/brave_wallet/common/zcash_utils.h"

namespace brave_wallet {

// Leaf position of checkpoint.
using CheckpointTreeState = std::optional<uint32_t>;

// Checkpointed leafs are not pruned so they could be used
// as anchors for building shielded transactions.
// Last Orchard commitment in a block is used as a checkpoint.
struct OrchardCheckpoint {
  OrchardCheckpoint();
  OrchardCheckpoint(CheckpointTreeState, std::vector<uint32_t>);
  ~OrchardCheckpoint();
  OrchardCheckpoint(const OrchardCheckpoint& other);
  OrchardCheckpoint& operator=(const OrchardCheckpoint& other);
  OrchardCheckpoint(OrchardCheckpoint&& other);
  OrchardCheckpoint& operator=(OrchardCheckpoint&& other);

  bool operator==(const OrchardCheckpoint& other) const = default;

  CheckpointTreeState tree_state_position;
  // List of note positions that were spent at this checkpoint.
  std::vector<uint32_t> marks_removed;
};

struct OrchardCheckpointBundle {
  OrchardCheckpointBundle(uint32_t checkpoint_id, OrchardCheckpoint);
  ~OrchardCheckpointBundle();
  OrchardCheckpointBundle(const OrchardCheckpointBundle& other);
  OrchardCheckpointBundle& operator=(const OrchardCheckpointBundle& other);
  OrchardCheckpointBundle(OrchardCheckpointBundle&& other);
  OrchardCheckpointBundle& operator=(OrchardCheckpointBundle&& other);

  bool operator==(const OrchardCheckpointBundle& other) const = default;

  // The block height serves as the checkpoint identifier.
  uint32_t checkpoint_id = 0;
  OrchardCheckpoint checkpoint;
};

// Address of a subtree in the shard tree.
struct OrchardShardAddress {
  uint8_t level = 0;
  uint32_t index = 0;

  bool operator==(const OrchardShardAddress& other) const = default;
};

// Top part of the shard tree from the root to the shard roots level
// Used for optimization purposes in the shard tree crate.
using OrchardShardTreeCap = std::vector<uint8_t>;

// Subtree with root selected from the shard roots level.
struct OrchardShard {
  OrchardShard();
  OrchardShard(OrchardShardAddress shard_addr,
               std::optional<OrchardShardRootHash> shard_hash,
               std::vector<uint8_t> shard_data);
  ~OrchardShard();

  OrchardShard(const OrchardShard& other);
  OrchardShard& operator=(const OrchardShard& other);
  OrchardShard(OrchardShard&& other);
  OrchardShard& operator=(OrchardShard&& other);

  bool operator==(const OrchardShard& other) const = default;

  // Subtree root address.
  OrchardShardAddress address;
  // Root hash exists only on completed shards.
  std::optional<OrchardShardRootHash> root_hash;
  std::vector<uint8_t> shard_data;
  // Right-most position of the subtree leaf.
  size_t subtree_end_height = 0;
};

struct OrchardCommitment {
  OrchardCommitmentValue cmu;
  bool is_marked = false;
  std::optional<uint32_t> checkpoint_id;
};

// Compact representation of the Merkle tree on some point.
// Since batch inserting may contain gaps between scan ranges we insert
// frontier which allows to calculate node hashes and witnesses(merkle path from
// leaf to the tree root) even when previous scan ranges are not completed.
struct OrchardTreeState {
  OrchardTreeState();
  ~OrchardTreeState();
  OrchardTreeState(const OrchardTreeState&);

  // Tree state is linked to the end of some block.
  uint32_t block_height = 0u;
  // Number of leafs at the position.
  uint32_t tree_size = 0u;
  // https://docs.aztec.network/protocol-specs/l1-smart-contracts/frontier
  std::vector<uint8_t> frontier;
};

// Describes the interface for a persistent storage system utilized by the shard
// tree. See
class OrchardShardTreeDelegate {
 public:
  enum Error { kStorageError = 0 };

  virtual ~OrchardShardTreeDelegate() = default;

  virtual base::expected<std::optional<OrchardShardTreeCap>, Error> GetCap()
      const = 0;
  virtual base::expected<bool, Error> PutCap(
      const OrchardShardTreeCap& shard) = 0;

  virtual base::expected<std::optional<uint32_t>, Error> GetLatestShardIndex()
      const = 0;
  virtual base::expected<bool, Error> PutShard(const OrchardShard& shard) = 0;
  virtual base::expected<std::optional<OrchardShard>, Error> GetShard(
      const OrchardShardAddress& address) const = 0;
  virtual base::expected<std::optional<OrchardShard>, Error> LastShard(
      uint8_t shard_height) const = 0;
  virtual base::expected<bool, Error> Truncate(uint32_t block_height) = 0;
  virtual base::expected<bool, Error> TruncateCheckpoints(
      uint32_t checkpoint_id) = 0;
  virtual base::expected<size_t, Error> CheckpointCount() const = 0;
  virtual base::expected<std::optional<uint32_t>, Error> MinCheckpointId()
      const = 0;
  virtual base::expected<std::optional<uint32_t>, Error> MaxCheckpointId()
      const = 0;
  virtual base::expected<std::optional<uint32_t>, Error> GetCheckpointAtDepth(
      uint32_t depth) const = 0;
  virtual base::expected<std::optional<OrchardCheckpointBundle>, Error>
  GetCheckpoint(uint32_t checkpoint_id) const = 0;
  virtual base::expected<std::vector<OrchardCheckpointBundle>, Error>
  GetCheckpoints(size_t limit) const = 0;
  virtual base::expected<bool, Error> RemoveCheckpoint(
      uint32_t checkpoint_id) = 0;
  virtual base::expected<bool, Error> AddCheckpoint(
      uint32_t id,
      const OrchardCheckpoint& checkpoint) = 0;
  virtual base::expected<bool, Error> UpdateCheckpoint(
      uint32_t id,
      const OrchardCheckpoint& checkpoint) = 0;
  virtual base::expected<std::vector<OrchardShardAddress>, Error> GetShardRoots(
      uint8_t shard_level) const = 0;
};

}  // namespace brave_wallet

#endif  // BRAVE_COMPONENTS_BRAVE_WALLET_COMMON_ORCHARD_SHARD_TREE_DELEGATE_H_
