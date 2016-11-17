// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <unordered_map>

#include "apps/maxwell/services/suggestion/suggestion_provider.fidl.h"
#include "gtest/gtest.h"

class TestSuggestionListener : public maxwell::suggestion::Listener {
 public:
  void OnAdd(
      fidl::Array<maxwell::suggestion::SuggestionPtr> suggestions) override;
  void OnRemove(const fidl::String& uuid) override;
  void OnRemoveAll() override;

  int suggestion_count() const { return naive_suggestion_count_; }

  // Exposes a pointer to the only suggestion in this listener. Retains
  // ownership of the pointer.
  const maxwell::suggestion::Suggestion* GetOnlySuggestion() const {
    EXPECT_EQ(1, suggestion_count());
    return suggestions_.begin()->second.get();
  }

 private:
  int naive_suggestion_count_ = 0;
  std::unordered_map<std::string, maxwell::suggestion::SuggestionPtr>
      suggestions_;
};