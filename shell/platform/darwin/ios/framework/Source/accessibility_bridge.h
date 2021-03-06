// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SHELL_PLATFORM_IOS_FRAMEWORK_SOURCE_ACCESSIBILITY_BRIDGE_H_
#define SHELL_PLATFORM_IOS_FRAMEWORK_SOURCE_ACCESSIBILITY_BRIDGE_H_

#include <memory>
#include <set>
#include <unordered_map>

#include "flutter/services/semantics/semantics.mojom.h"
#include "flutter/shell/platform/darwin/ios/framework/Source/FlutterView.h"
#include "flutter/sky/engine/platform/geometry/FloatRect.h"
#include "lib/ftl/macros.h"
#include "mojo/public/cpp/bindings/array.h"
#include "mojo/public/cpp/bindings/strong_binding.h"
#include "mojo/public/interfaces/application/service_provider.mojom.h"
#include "third_party/skia/include/core/SkMatrix44.h"
#include "third_party/skia/include/core/SkRect.h"

namespace shell {
class AccessibilityBridge;
}  // namespace shell

@interface SemanticObject : NSObject

/**
 * The globally unique identifier for this node.
 */
@property(nonatomic, readonly) uint32_t uid;

/**
 * The parent of this node in the node tree. Will be nil for the root node and
 * during transient state changes.
 */
@property(nonatomic, assign) SemanticObject* parent;

- (instancetype)init __attribute__((unavailable("Use initWithBridge instead")));
- (instancetype)initWithBridge:(shell::AccessibilityBridge*)bridge
                           uid:(uint32_t)uid NS_DESIGNATED_INITIALIZER;

@end

namespace shell {

class AccessibilityBridge final : public semantics::SemanticsListener {
 public:
  AccessibilityBridge(UIView*, mojo::ServiceProvider*);
  ~AccessibilityBridge() override;

  void UpdateSemanticsTree(mojo::Array<semantics::SemanticsNodePtr>) override;

  UIView* view() { return view_; }
  semantics::SemanticsServer* server() { return semantics_server_.get(); }

 private:
  SemanticObject* UpdateSemanticObject(
      const semantics::SemanticsNodePtr& node,
      std::set<SemanticObject*>* updated_objects,
      std::set<SemanticObject*>* removed_objects);
  void RemoveSemanticObject(SemanticObject* node,
                            std::set<SemanticObject*>* updated_objects);

  UIView* view_;
  semantics::SemanticsServerPtr semantics_server_;
  std::unordered_map<int, SemanticObject*> objects_;

  mojo::Binding<semantics::SemanticsListener> binding_;

  FTL_DISALLOW_COPY_AND_ASSIGN(AccessibilityBridge);
};

}  // namespace shell

#endif  // SHELL_PLATFORM_IOS_FRAMEWORK_SOURCE_ACCESSIBILITY_BRIDGE_H_
