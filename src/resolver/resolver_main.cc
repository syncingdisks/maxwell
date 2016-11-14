// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "apps/maxwell/services/resolver.fidl.h"

#include "apps/modular/lib/app/service_provider_impl.h"

#include "lib/fidl/cpp/bindings/binding.h"
#include "lib/mtl/tasks/message_loop.h"

namespace resolver {

class ResolverApp {
 public:
  ResolverApp()
    : context_(modular::ApplicationContext::CreateFromStartupInfo()) {
    // Singleton service
    context_->outgoing_services()->AddService<ResolverImpl>(
        [this](fidl::InterfaceRequest<Resolver> request) {
          resolver_bindings_.AddBinding(&resolver_impl, std::move(request));
        });
  }
 private:
  std::unique_ptr<modular::ApplicationContext> context_;
  ResolverImpl resolver_impl_;
  fidl::BindingSet<Resolver> resolver_bindings_;

  FTL_DISALLOW_COPY_AND_ASSIGN(ResolverApp);
};

} // namespace resolver

int main(int argc, const char** argv) {
  mtl::MessageLoop loop;
  maxwell::resolver::ResolverApp app;
  loop.Run();
  return 0;
}