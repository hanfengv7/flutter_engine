// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SHELL_PLATFORM_IOS_PLATFORM_VIEW_IOS_H_
#define SHELL_PLATFORM_IOS_PLATFORM_VIEW_IOS_H_

#include <memory>

#include "lib/ftl/macros.h"
#include "lib/ftl/memory/weak_ptr.h"
#include "base/mac/scoped_nsobject.h"
#include "flutter/services/platform/app_messages.mojom.h"
#include "flutter/shell/platform/darwin/ios/framework/Source/accessibility_bridge.h"
#include "flutter/shell/platform/darwin/ios/framework/Source/application_messages_impl.h"
#include "flutter/shell/common/platform_view.h"

@class CAEAGLLayer;
@class UIView;

namespace shell {

class IOSGLContext;

class PlatformViewIOS : public PlatformView {
 public:
  explicit PlatformViewIOS(CAEAGLLayer* layer);

  ~PlatformViewIOS() override;

  void ToggleAccessibility(UIView* view, bool enable);

  void ConnectToEngineAndSetupServices();

  sky::SkyEnginePtr& engineProxy();

  flutter::platform::ApplicationMessagesPtr& AppMessageSender();

  ApplicationMessagesImpl& AppMessageReceiver();

  ftl::WeakPtr<PlatformView> GetWeakViewPtr() override;

  uint64_t DefaultFramebuffer() const override;

  bool ContextMakeCurrent() override;

  bool ResourceContextMakeCurrent() override;

  bool SwapBuffers() override;

  void RunFromSource(const std::string& main,
                     const std::string& packages,
                     const std::string& assets_directory) override;

 private:
  std::unique_ptr<IOSGLContext> context_;
  sky::SkyEnginePtr engine_;
  mojo::ServiceProviderPtr dart_services_;
  flutter::platform::ApplicationMessagesPtr app_message_sender_;
  ApplicationMessagesImpl app_message_receiver_;
  std::unique_ptr<AccessibilityBridge> accessibility_bridge_;
  ftl::WeakPtrFactory<PlatformViewIOS> weak_factory_;

  void SetupAndLoadFromSource(const std::string& main,
                              const std::string& packages,
                              const std::string& assets_directory);

  FTL_DISALLOW_COPY_AND_ASSIGN(PlatformViewIOS);
};

}  // namespace shell

#endif  // SHELL_PLATFORM_IOS_PLATFORM_VIEW_IOS_H_
