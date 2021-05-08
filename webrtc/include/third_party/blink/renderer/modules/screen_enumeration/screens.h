// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_SCREEN_ENUMERATION_SCREENS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_SCREEN_ENUMERATION_SCREENS_H_

#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_observer.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class LocalDOMWindow;
class ScreenAdvanced;
struct ScreenInfos;

// Interface exposing multi-screen information.
// https://github.com/webscreens/window-placement
class MODULES_EXPORT Screens final : public EventTargetWithInlineData,
                                     public ExecutionContextLifecycleObserver {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit Screens(LocalDOMWindow* window);

  // Web-exposed interface:
  const HeapVector<Member<ScreenAdvanced>>& screens() const;
  ScreenAdvanced* currentScreen() const;
  DEFINE_ATTRIBUTE_EVENT_LISTENER(change, kChange)

  // EventTargetWithInlineData:
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  // ExecutionContextLifecycleObserver:
  void ContextDestroyed() override;

  void Trace(Visitor*) const override;

  // Called when the underlying multi-screen information changes.
  void ScreenInfosChanged(LocalDOMWindow* window, const ScreenInfos& infos);

 private:
  HeapVector<Member<ScreenAdvanced>> screens_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_SCREEN_ENUMERATION_SCREENS_H_
