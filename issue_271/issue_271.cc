#if defined(PERFETTO_AMALGAMATED_SDK_TEST) || defined(ISSUE_271)
#include "perfetto.h"
#else
#pragma clang diagnostic ignored "-Wweak-vtables"
#include "perfetto/tracing.h"
#endif

#include <chrono>
#include <thread>

/*
Build with flags:
target_os = "linux"
target_cpu = "x64"
is_debug = true
is_clang = true
is_asan = true
is_ubsan = true
*/

PERFETTO_DEFINE_CATEGORIES(perfetto::Category("trace_category").SetDescription(""));
PERFETTO_TRACK_EVENT_STATIC_STORAGE();

int main() {
  perfetto::TracingInitArgs args;
  args.backends |= perfetto::kSystemBackend;
  perfetto::Tracing::Initialize(args);
	perfetto::TrackEvent::Register();
  perfetto::TrackEvent::Flush();
  return 0;
}
