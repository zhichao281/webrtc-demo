/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_SCRIPT_RUNNER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_SCRIPT_RUNNER_H_

#include "base/location.h"
#include "base/task/single_thread_task_runner.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_state_observer.h"
#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/deque.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"

namespace blink {

class Document;
class PendingScript;
class ScriptLoader;

class CORE_EXPORT ScriptRunner final
    : public GarbageCollected<ScriptRunner>,
      public NameClient {
 public:
  explicit ScriptRunner(Document*);
  ~ScriptRunner() override = default;
  ScriptRunner(const ScriptRunner&) = delete;
  ScriptRunner& operator=(const ScriptRunner&) = delete;

  void QueueScriptForExecution(PendingScript*);
  void NotifyScriptReady(PendingScript*);

  static void MovePendingScript(Document&, Document&, ScriptLoader*);

  void SetTaskRunnerForTesting(base::SingleThreadTaskRunner* task_runner) {
    task_runner_ = task_runner;
  }

  void Trace(Visitor*) const;
  const char* NameInHeapSnapshot() const override { return "ScriptRunner"; }

 private:
  void MovePendingScript(ScriptRunner*, PendingScript*);
  bool RemovePendingInOrderScript(PendingScript*);

  // Execute the given pending script.
  void ExecutePendingScript(PendingScript*);

  Member<Document> document_;

  // https://html.spec.whatwg.org/#list-of-scripts-that-will-execute-in-order-as-soon-as-possible
  HeapDeque<Member<PendingScript>> pending_in_order_scripts_;
  // https://html.spec.whatwg.org/#set-of-scripts-that-will-execute-as-soon-as-possible
  HeapHashSet<Member<PendingScript>> pending_async_scripts_;

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_SCRIPT_RUNNER_H_
