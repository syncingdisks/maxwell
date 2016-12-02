// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "apps/maxwell/src/acquirers/mock/mock_gps.h"

namespace maxwell {
namespace acquirers {

constexpr char GpsAcquirer::kLabel[];
constexpr char GpsAcquirer::kSchema[];

MockGps::MockGps(ContextEngine* context_engine) : ctl_(this) {
  maxwell::ContextPublisherPtr cx;
  context_engine->RegisterPublisher("MockGps", cx.NewRequest());

  fidl::InterfaceHandle<ContextPublisherController> ctl_handle;
  ctl_.Bind(&ctl_handle);

  cx->Publish(kLabel, kSchema, std::move(ctl_handle), out_.NewRequest());
}

void MockGps::Publish(float latitude, float longitude) {
  std::ostringstream json;
  json << "{ \"lat\": " << latitude << ", \"lng\": " << longitude << " }";
  out_->Update(json.str());
}

void MockGps::OnHasSubscribers() {
  has_subscribers_ = true;
}

void MockGps::OnNoSubscribers() {
  has_subscribers_ = false;
}

}  // namespace acquirers
}  // namespace maxwell
