// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <unordered_map>

#include "apps/maxwell/src/suggestion_engine/proposal_record.h"
#include "apps/maxwell/src/suggestion_engine/suggestion_agent_client_impl.h"
#include "apps/maxwell/src/suggestion_engine/next_channel.h"
#include "apps/maxwell/src/suggestion_engine/ask_subscriber.h"

namespace maxwell {
namespace suggestion {

class Repo {
 public:
  SuggestionAgentClientImpl* GetOrCreateSourceClient(
      const std::string& component_url);

  void RemoveSourceClient(const std::string& component_url) {
    sources_.erase(component_url);
  }

  void AddSuggestion(std::unique_ptr<ProposalRecord> proposal,
                     AgentSuggestionRecord* agent_suggestion_record);

  void RemoveSuggestion(const std::string& id) { suggestions_.erase(id); }

  auto next_ranked_suggestions() const {
    return next_channel_.ranked_suggestions();
  }

  void AddNextSubscriber(std::unique_ptr<NextSubscriber> subscriber) {
    next_channel_.AddSubscriber(std::move(subscriber));
  }

  void AddAskSubscriber(std::unique_ptr<AskSubscriber> subscriber) {
    // TODO(rosswang): bootstrap
    asks_.emplace(std::move(subscriber));
  }

  // Non-mutating indexer; returns NULL if no such suggestion exists.
  const ProposalRecord* operator[](const std::string& suggestion_id) const {
    auto it = suggestions_.find(suggestion_id);
    return it == suggestions_.end() ? NULL : it->second.get();
  }

 private:
  std::string RandomUuid() {
    static uint64_t id = 0;
    // TODO(rosswang): real UUIDs
    return std::to_string(id++);
  }

  std::unordered_map<std::string, std::unique_ptr<SuggestionAgentClientImpl>>
      sources_;
  // indexed by suggestion ID
  std::unordered_map<std::string, ProposalRecordPtr> suggestions_;
  NextChannel next_channel_;
  maxwell::BindingSet<AskController,
                      std::unique_ptr<AskSubscriber>,
                      AskSubscriber::GetBinding>
      asks_;
};

}  // namespace suggestion
}  // namespace maxwell
