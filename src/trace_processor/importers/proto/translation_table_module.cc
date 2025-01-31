/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "src/trace_processor/importers/proto/translation_table_module.h"

#include "src/trace_processor/importers/common/args_translation_table.h"

#include "protos/perfetto/trace/trace_packet.pbzero.h"
#include "protos/perfetto/trace/translation/translation_table.pbzero.h"

namespace perfetto {
namespace trace_processor {

using perfetto::protos::pbzero::TracePacket;

TranslationTableModule::TranslationTableModule(TraceProcessorContext* context)
    : context_(context) {
  RegisterForField(TracePacket::kTranslationTableFieldNumber, context);
}

TranslationTableModule::~TranslationTableModule() = default;

void TranslationTableModule::ParsePacket(const TracePacket::Decoder& decoder,
                                         const TimestampedTracePiece& /*ttp*/,
                                         uint32_t field_id) {
  if (field_id != TracePacket::kTranslationTableFieldNumber) {
    return;
  }
  const auto translation_table =
      protos::pbzero::TranslationTable::Decoder(decoder.translation_table());
  if (translation_table.has_chrome_histogram()) {
    ParseChromeHistogramRules(translation_table.chrome_histogram());
  } else if (translation_table.has_chrome_user_event()) {
    ParseChromeUserEventRules(translation_table.chrome_user_event());
  } else if (translation_table.has_chrome_performance_mark()) {
    ParseChromePerformanceMarkRules(
        translation_table.chrome_performance_mark());
  }
}

void TranslationTableModule::ParseChromeHistogramRules(
    protozero::ConstBytes bytes) {
  const auto chrome_histogram =
      protos::pbzero::ChromeHistorgramTranslationTable::Decoder(bytes);
  for (auto it = chrome_histogram.hash_to_name(); it; ++it) {
    protos::pbzero::ChromeHistorgramTranslationTable::HashToNameEntry::Decoder
        entry(*it);
    context_->args_translation_table->AddChromeHistogramTranslationRule(
        entry.key(), entry.value());
  }
}

void TranslationTableModule::ParseChromeUserEventRules(
    protozero::ConstBytes bytes) {
  const auto chrome_user_event =
      protos::pbzero::ChromeUserEventTranslationTable::Decoder(bytes);
  for (auto it = chrome_user_event.action_hash_to_name(); it; ++it) {
    protos::pbzero::ChromeUserEventTranslationTable::ActionHashToNameEntry::
        Decoder entry(*it);
    context_->args_translation_table->AddChromeUserEventTranslationRule(
        entry.key(), entry.value());
  }
}

void TranslationTableModule::ParseChromePerformanceMarkRules(
    protozero::ConstBytes bytes) {
  const auto chrome_performance_mark =
      protos::pbzero::ChromePerformanceMarkTranslationTable::Decoder(bytes);
  for (auto it = chrome_performance_mark.site_hash_to_name(); it; ++it) {
    protos::pbzero::ChromePerformanceMarkTranslationTable::SiteHashToNameEntry::
        Decoder entry(*it);
    context_->args_translation_table
        ->AddChromePerformanceMarkSiteTranslationRule(entry.key(),
                                                      entry.value());
  }
  for (auto it = chrome_performance_mark.mark_hash_to_name(); it; ++it) {
    protos::pbzero::ChromePerformanceMarkTranslationTable::MarkHashToNameEntry::
        Decoder entry(*it);
    context_->args_translation_table
        ->AddChromePerformanceMarkMarkTranslationRule(entry.key(),
                                                      entry.value());
  }
}

}  // namespace trace_processor
}  // namespace perfetto
