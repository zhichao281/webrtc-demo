// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_MOJO_BINDING_CONTEXT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_MOJO_BINDING_CONTEXT_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/public/platform/task_type.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/renderer/platform/context_lifecycle_notifier.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/supplementable.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace blink {

class BrowserInterfaceBrokerProxy;

// This class encapsulates the necessary information for binding Mojo
// interfaces, to enable interfaces provided by the platform to be aware of the
// context in which they are intended to be used.
class PLATFORM_EXPORT MojoBindingContext
    : public ContextLifecycleNotifier,
      public Supplementable<MojoBindingContext> {
 public:
  virtual const BrowserInterfaceBrokerProxy& GetBrowserInterfaceBroker()
      const = 0;
  virtual scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner(
      TaskType) = 0;

  void Trace(Visitor* visitor) const override {
    ContextLifecycleNotifier::Trace(visitor);
    Supplementable::Trace(visitor);
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_MOJO_BINDING_CONTEXT_H_
