// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SHELL_PLATFORM_MAC_PLATFORM_VIEW_MAC_H_
#define SHELL_PLATFORM_MAC_PLATFORM_VIEW_MAC_H_

#include "base/mac/scoped_nsobject.h"
#include "flutter/shell/common/platform_view.h"
#include "lib/ftl/memory/weak_ptr.h"

@class NSOpenGLView;
@class NSOpenGLContext;

namespace shell {

class PlatformViewMac : public PlatformView {
 public:
  PlatformViewMac(NSOpenGLView* gl_view);

  ~PlatformViewMac() override;

  void SetupAndLoadDart();

  sky::SkyEnginePtr& engineProxy();

  ftl::WeakPtr<PlatformView> GetWeakViewPtr() override;

  uint64_t DefaultFramebuffer() const override;

  bool ContextMakeCurrent() override;

  bool ResourceContextMakeCurrent() override;

  bool SwapBuffers() override;

  void RunFromSource(const std::string& main,
                     const std::string& packages,
                     const std::string& assets_directory) override;

 private:
  base::scoped_nsobject<NSOpenGLView> opengl_view_;
  base::scoped_nsobject<NSOpenGLContext> resource_loading_context_;
  sky::SkyEnginePtr sky_engine_;
  ftl::WeakPtrFactory<PlatformViewMac> weak_factory_;

  bool IsValid() const;

  void ConnectToEngineAndSetupServices();

  void SetupAndLoadFromSource(const std::string& main,
                              const std::string& packages,
                              const std::string& assets_directory);

  FTL_DISALLOW_COPY_AND_ASSIGN(PlatformViewMac);
};

}  // namespace shell

#endif  // SHELL_PLATFORM_MAC_PLATFORM_VIEW_MAC_H_
