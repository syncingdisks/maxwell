// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "apps/maxwell/services/resolver/resolver.fidl-sync.h"

#include "application/lib/app/application_context.h"
#include "application/lib/app/connect.h"
#include "application/lib/app/service_provider_impl.h"
#include "lib/fidl/cpp/bindings/binding_set.h"
#include "lib/fidl/cpp/bindings/interface_ptr_set.h"
#include "lib/fidl/cpp/bindings/synchronous_interface_ptr.h"
#include "lib/ftl/macros.h"
#include "lib/mtl/tasks/message_loop.h"

namespace {

class ResolverTest {
 public:
  ResolverTest()
      : context_(app::ApplicationContext::CreateFromStartupInfo()) {}

  void RunTests() {
    fidl::SynchronousInterfacePtr<resolver::Resolver> resolver;
    app::ServiceProviderPtr child_services;

    auto launch_info = app::ApplicationLaunchInfo::New();
    launch_info->url = "file:///system/apps/resolver_main";
    launch_info->services = child_services.NewRequest();

    context_->launcher()->CreateApplication(std::move(launch_info),
                                            resolver_controller_.NewRequest());

    app::ConnectToService(child_services.get(),
                              fidl::GetSynchronousProxy(&resolver));

    FTL_CHECK(resolver.is_bound());
    FTL_LOG(INFO) << "Connected to " << resolver::Resolver::Name_;

    fidl::Array<resolver::ModuleInfoPtr> modules;
    FTL_CHECK(resolver->ResolveModules(
        "https://fuchsia-contracts.google.com/hello_contract", nullptr,
        &modules));
    FTL_CHECK(1 == modules.size());
    FTL_CHECK("https://www.example.com/hello" == modules[0]->component_id);
    FTL_LOG(INFO) << "Test passed.";
  }

 private:
  std::unique_ptr<app::ApplicationContext> context_;
  app::ApplicationControllerPtr resolver_controller_;
};

}  // namespace

int main(int argc, const char** argv) {
  FTL_LOG(INFO) << "resolver_test";
  mtl::MessageLoop loop;
  ResolverTest app;
  app.RunTests();
  loop.Run();
  return 0;
}
