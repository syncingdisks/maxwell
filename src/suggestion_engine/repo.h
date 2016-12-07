// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <unordered_map>

#include "apps/maxwell/src/suggestion_engine/ask_channel.h"
#include "apps/maxwell/src/suggestion_engine/filter.h"
#include "apps/maxwell/src/suggestion_engine/next_channel.h"
#include "apps/maxwell/src/suggestion_engine/suggestion_prototype.h"
#include "apps/maxwell/src/suggestion_engine/proposal_publisher_impl.h"
#include "lib/fidl/cpp/bindings/interface_ptr_set.h"

namespace maxwell {

class Repo {
 public:
  Repo(ProposalFilter filter) : next_channel_(filter), filter_(filter) {}

  ProposalPublisherImpl* GetOrCreateSourceClient(
      const std::string& component_url);

  void RemoveSourceClient(const std::string& component_url) {
    sources_.erase(component_url);
  }

  // Should only be called from ProposalPublisherImpl
  void AddSuggestion(SuggestionPrototype* prototype);
  // Should only be called from ProposalPublisherImpl
  void RemoveSuggestion(const std::string& id) { suggestions_.erase(id); }

  void SubscribeToNext(fidl::InterfaceHandle<SuggestionListener> listener,
                       fidl::InterfaceRequest<NextController> controller) {
    next_channel_.AddSubscriber(std::make_unique<NextSubscriber>(
        next_channel_.ranked_suggestions(), std::move(listener),
        std::move(controller)));
  }

  void InitiateAsk(fidl::InterfaceHandle<SuggestionListener> listener,
                   fidl::InterfaceRequest<AskController> controller);

  void AddAskHandler(fidl::InterfaceHandle<AskHandler> ask_handler) {
    ask_handlers_.AddInterfacePtr(
        AskHandlerPtr::Create(std::move(ask_handler)));
  }

  void DispatchAsk(UserInputPtr query) {
    ask_handlers_.ForAllPtrs([&query](AskHandler* handler) {
      handler->Ask(query.Clone(), [](fidl::Array<ProposalPtr> proposals) {
        // TODO(rosswang)
      });
    });
  }

  std::unique_ptr<SuggestionPrototype> Extract(const std::string& id) {
    auto it = suggestions_.find(id);
    if (it == suggestions_.end()) {
      return NULL;
    } else {
      std::unique_ptr<SuggestionPrototype> suggestion =
          std::unique_ptr<SuggestionPrototype>(std::move(it->second));
      suggestions_.erase(id);
      return suggestion;
    }
  }

  // Non-mutating indexer; returns NULL if no such suggestion exists.
  const SuggestionPrototype* operator[](
      const std::string& suggestion_id) const {
    auto it = suggestions_.find(suggestion_id);
    return it == suggestions_.end() ? NULL : it->second;
  }

  ProposalFilter filter() { return filter_; }

 private:
  std::string RandomUuid() {
    static uint64_t id = 0;
    // TODO(rosswang): real UUIDs
    return std::to_string(id++);
  }

  std::unordered_map<std::string, std::unique_ptr<ProposalPublisherImpl>>
      sources_;
  // indexed by suggestion ID
  std::unordered_map<std::string, SuggestionPrototype*> suggestions_;
  NextChannel next_channel_;
  maxwell::BoundNonMovableSet<AskChannel> ask_channels_;
  fidl::InterfacePtrSet<AskHandler> ask_handlers_;

  ProposalFilter filter_;
};

}  // namespace maxwell
