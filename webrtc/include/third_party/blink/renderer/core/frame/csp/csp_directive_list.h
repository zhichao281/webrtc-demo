// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_CSP_CSP_DIRECTIVE_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_CSP_CSP_DIRECTIVE_LIST_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/frame/csp/content_security_policy.h"
#include "third_party/blink/renderer/core/frame/csp/media_list_directive.h"
#include "third_party/blink/renderer/core/frame/csp/require_trusted_types_for_directive.h"
#include "third_party/blink/renderer/core/frame/csp/source_list_directive.h"
#include "third_party/blink/renderer/core/frame/csp/trusted_types_directive.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_request.h"
#include "third_party/blink/renderer/platform/network/content_security_policy_parsers.h"
#include "third_party/blink/renderer/platform/network/http_parsers.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/reporting_disposition.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class ContentSecurityPolicy;
enum class ResourceType : uint8_t;

struct CSPOperativeDirective {
  CSPDirectiveName type;
  const network::mojom::blink::CSPSourceList* source_list;
};

class CORE_EXPORT CSPDirectiveList final
    : public GarbageCollected<CSPDirectiveList> {
 public:
  static CSPDirectiveList* Create(ContentSecurityPolicy*,
                                  const UChar* begin,
                                  const UChar* end,
                                  network::mojom::ContentSecurityPolicyType,
                                  network::mojom::ContentSecurityPolicySource,
                                  bool should_parse_wasm_eval = false);

  explicit CSPDirectiveList(ContentSecurityPolicy*);

  void Parse(const UChar* begin,
             const UChar* end,
             bool should_parse_wasm_eval = false);

  const String& Header() const { return header_->header_value; }
  network::mojom::ContentSecurityPolicyType HeaderType() const {
    return header_->type;
  }
  network::mojom::ContentSecurityPolicySource HeaderSource() const {
    return header_->source;
  }

  bool AllowInline(ContentSecurityPolicy::InlineType,
                   Element*,
                   const String& content,
                   const String& nonce,
                   const String& context_url,
                   const WTF::OrdinalNumber& context_line,
                   ReportingDisposition) const;

  // Returns whether or not the Javascript code generation should call back the
  // CSP checker before any script evaluation from a string is being made.
  bool ShouldCheckEval() const;

  bool AllowEval(ReportingDisposition,
                 ContentSecurityPolicy::ExceptionStatus,
                 const String& script_content) const;
  bool AllowWasmEval(ReportingDisposition,
                     ContentSecurityPolicy::ExceptionStatus,
                     const String& script_content) const;
  bool AllowPluginType(const String& type,
                       const String& type_attribute,
                       const KURL&,
                       ReportingDisposition) const;

  bool AllowFromSource(CSPDirectiveName,
                       const KURL&,
                       const KURL& url_before_redirects,
                       ResourceRequest::RedirectStatus,
                       ReportingDisposition,
                       const String& nonce = String(),
                       const IntegrityMetadataSet& = IntegrityMetadataSet(),
                       ParserDisposition = kParserInserted) const;

  bool AllowTrustedTypePolicy(
      const String& policy_name,
      bool is_duplicate,
      ContentSecurityPolicy::AllowTrustedTypePolicyDetails& violation_details)
      const;

  bool AllowDynamic(CSPDirectiveName) const;
  bool AllowDynamicWorker() const;

  bool AllowTrustedTypeAssignmentFailure(const String& message,
                                         const String& sample,
                                         const String& sample_prefix) const;

  bool StrictMixedContentChecking() const { return block_all_mixed_content_; }
  void ReportMixedContent(const KURL& blocked_url,
                          ResourceRequest::RedirectStatus) const;

  bool ShouldDisableEval(String& error_message) const;
  bool IsReportOnly() const {
    return header_->type == network::mojom::ContentSecurityPolicyType::kReport;
  }
  bool IsActiveForConnections() const {
    return OperativeDirective(CSPDirectiveName::ConnectSrc).source_list;
  }
  const Vector<String>& ReportEndpoints() const { return report_endpoints_; }
  bool UseReportingApi() const { return use_reporting_api_; }

  // Used to copy plugin-types into a plugin document in a nested
  // browsing context.
  bool HasPluginTypes() const { return plugin_types_.has_value(); }
  String PluginTypesText() const;

  bool ShouldSendCSPHeader(ResourceType) const;

  bool AllowHash(const network::mojom::blink::CSPHashSource& hash_value,
                 const ContentSecurityPolicy::InlineType inline_type) const;

  // Export the policies. The primary goal of this method is to make
  // the embedders aware of the directives that affect navigation, as the
  // embedder is responsible for navigational enforcement.
  network::mojom::blink::ContentSecurityPolicyPtr ExposeForNavigationalChecks()
      const;

  // We consider `object-src` restrictions to be reasonable iff they're
  // equivalent to `object-src 'none'`.
  bool IsObjectRestrictionReasonable() const;

  // We consider `base-uri` restrictions to be reasonable iff they're equivalent
  // to `base-uri 'none'` or `base-uri 'self'`.
  bool IsBaseRestrictionReasonable() const;

  // We consider `script-src` restrictions to be reasonable iff they're not
  // URL-based (e.g. they contain only nonces and hashes, or they use
  // 'strict-dynamic'). Neither `'unsafe-eval'` nor `'unsafe-hashes'` affect
  // this judgement.
  bool IsScriptRestrictionReasonable() const;

  bool RequiresTrustedTypes() const;

  bool TrustedTypesAllowDuplicates() const {
    return trusted_types_ && trusted_types_->allow_duplicates;
  }

  void Trace(Visitor*) const;

 private:
  FRIEND_TEST_ALL_PREFIXES(CSPDirectiveListTest, IsMatchingNoncePresent);
  FRIEND_TEST_ALL_PREFIXES(CSPDirectiveListTest, OperativeDirectiveGivenType);

  void ApplyParsedDirectives();
  bool ParseDirective(const UChar* begin,
                      const UChar* end,
                      String* name,
                      String* value);
  void ParseReportURI(const String& name, const String& value);
  void ParseReportTo(const String& name, const String& value);
  void ParseAndAppendReportEndpoints(const String& value);
  void ParsePluginTypes(const String& name, const String& value);
  void AddDirective(const String& name, const String& value);
  void ParseSandboxPolicy(const String& name, const String& sandbox_policy);
  void ParseTreatAsPublicAddress();
  void ParseBlockAllMixedContent(const String& name, const String& value);
  void ParseUpgradeInsecureRequests(const String& name, const String& value);

  CSPDirectiveName FallbackDirective(CSPDirectiveName current_directive,
                                     CSPDirectiveName original_directive) const;
  void ReportViolation(
      const String& directive_text,
      CSPDirectiveName,
      const String& console_message,
      const KURL& blocked_url,
      ResourceRequest::RedirectStatus,
      ContentSecurityPolicy::ContentSecurityPolicyViolationType violation_type =
          ContentSecurityPolicy::kURLViolation,
      const String& sample = String(),
      const String& sample_prefix = String()) const;
  void ReportViolationWithLocation(const String& directive_text,
                                   CSPDirectiveName,
                                   const String& console_message,
                                   const KURL& blocked_url,
                                   const String& context_url,
                                   const WTF::OrdinalNumber& context_line,
                                   Element*,
                                   const String& source) const;
  void ReportEvalViolation(const String& directive_text,
                           CSPDirectiveName,
                           const String& message,
                           const KURL& blocked_url,
                           const ContentSecurityPolicy::ExceptionStatus,
                           const String& content) const;

  bool CheckEval(const network::mojom::blink::CSPSourceList* directive) const;
  bool CheckWasmEval(
      const network::mojom::blink::CSPSourceList* directive) const;
  bool CheckDynamic(const network::mojom::blink::CSPSourceList* directive,
                    CSPDirectiveName effective_type) const;
  bool IsMatchingNoncePresent(
      const network::mojom::blink::CSPSourceList* directive,
      const String&) const;
  bool AreAllMatchingHashesPresent(
      const network::mojom::blink::CSPSourceList* directive,
      const IntegrityMetadataSet&) const;
  bool CheckHash(const network::mojom::blink::CSPSourceList* directive,
                 const network::mojom::blink::CSPHashSource&) const;
  bool CheckUnsafeHashesAllowed(
      const network::mojom::blink::CSPSourceList* directive) const;
  bool CheckSource(const network::mojom::blink::CSPSourceList*,
                   const KURL&,
                   ResourceRequest::RedirectStatus) const;
  bool CheckMediaType(const Vector<String>& plugin_types,
                      const String& type,
                      const String& type_attribute) const;

  bool CheckEvalAndReportViolation(const String& console_message,
                                   ContentSecurityPolicy::ExceptionStatus,
                                   const String& script_content) const;
  bool CheckWasmEvalAndReportViolation(const String& console_message,
                                       ContentSecurityPolicy::ExceptionStatus,
                                       const String& script_content) const;
  bool CheckInlineAndReportViolation(CSPOperativeDirective directive,
                                     const String& console_message,
                                     Element*,
                                     const String& source,
                                     const String& context_url,
                                     const WTF::OrdinalNumber& context_line,
                                     bool is_script,
                                     const String& hash_value,
                                     CSPDirectiveName effective_type) const;

  bool CheckSourceAndReportViolation(CSPOperativeDirective directive,
                                     const KURL&,
                                     CSPDirectiveName,
                                     const KURL& url_before_redirects,
                                     ResourceRequest::RedirectStatus) const;
  bool CheckMediaTypeAndReportViolation(const Vector<String>& plugin_types,
                                        const String& type,
                                        const String& type_attribute,
                                        const String& console_message) const;

  bool DenyIfEnforcingPolicy() const { return IsReportOnly(); }

  // Return the operative directive name and CSPSourceList for a given directive
  // name, falling back to generic directives according to Content Security
  // Policies rules. For example, if 'default-src' is defined but 'media-src' is
  // not, OperativeDirective(CSPDirectiveName::MediaSrc) will return type
  // CSPDirectiveName::DefaultSrc and the corresponding CSPSourceList. If no
  // operative directive for the given type is defined, this will return
  // CSPDirectiveName::Unknown and nullptr.
  CSPOperativeDirective OperativeDirective(
      CSPDirectiveName type,
      CSPDirectiveName original_type = CSPDirectiveName::Unknown) const;

  Member<ContentSecurityPolicy> policy_;

  network::mojom::blink::ContentSecurityPolicyHeaderPtr header_;

  HashMap<CSPDirectiveName, String> raw_directives_;

  network::mojom::blink::WebSandboxFlags sandbox_flags_;

  bool block_all_mixed_content_;

  bool upgrade_insecure_requests_;

  base::Optional<Vector<String>> plugin_types_;
  HashMap<CSPDirectiveName, network::mojom::blink::CSPSourceListPtr>
      directives_;
  network::mojom::blink::CSPTrustedTypesPtr trusted_types_;
  network::mojom::blink::CSPRequireTrustedTypesFor require_trusted_types_for_;

  // If a "report-to" directive is used:
  // - |report_endpoints_| is a list of token parsed from the "report-to"
  //   directive's value, and
  // - |use_reporting_api_| is true.
  // Otherwise,
  // - |report_endpoints_| is a list of uri-reference parsed from a
  //   "report-uri" directive's value if any, and
  // - |use_reporting_api_| is false.
  Vector<String> report_endpoints_;
  bool use_reporting_api_;

  DISALLOW_COPY_AND_ASSIGN(CSPDirectiveList);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_CSP_CSP_DIRECTIVE_LIST_H_
