# Perfetto issue 271

This code is a standalone example that demonstrates part of [#271](https://github.com/google/perfetto/issues/271) issue.
Initially, this issue was found under `Ubuntu 18.04` and `clang 13.0.0-2` for debug build.
Others building environments are not tested yet.

The example has to be built using meson build system.
Before building it will clone `perfetto` v24.2 from github into `subprojects/perfetto` directory as a `subproject` dependency.
Then it will build `libperfetto.a` target using `subprojects/perfetto/meson.build` file.

## How to build
For clean build execute:
```sh
./setup.sh
```

To re-build source code execute:
```sh
ninja -C ../out/build.issue_271
```

## How to run

```sh
export UBSAN_OPTIONS=print_stacktrace=1
export ASAN_OPTIONS=detect_stack_use_after_return=1,print_stacktrace=1
../out/build.issue_271/issue_271
```

## Issue summary

Initial `use-after-poison` ASAN issue disappeared since both `perfetto` and `issue_271.cpp` compiled with the same compilation flags.

This is UBSAN output:
>
../../issue_271/subprojects/perfetto-v24.2/sdk/perfetto.h:1488:37: runtime error: applying non-zero offset 1 to null pointer
    #0 0x56459126d8cf in protozero::ScatteredStreamWriter::WriteBytes(unsigned char const*, unsigned long) /home/osboxes/projects/perfetto/out/build.issue_271/../../issue_271/subprojects/perfetto-v24.2/sdk/perfetto.h:1488:37
    #1 0x56459107f630 in protozero::Message::WriteToStream(unsigned char const*, unsigned char const*) /home/osboxes/projects/perfetto/out/build.issue_271/../../issue_271/subprojects/perfetto-v24.2/sdk/perfetto.h:1743:21
    #2 0x56459093c546 in protozero::Message::BeginNestedMessageInternal(unsigned int) /home/osboxes/projects/perfetto/out/build.issue_271/../../issue_271/subprojects/perfetto-v24.2/sdk/perfetto.cc:9692:3
    #3 0x5645915e6a6e in perfetto::protos::pbzero::TrackEventCategory* protozero::Message::BeginNestedMessage<perfetto::protos::pbzero::TrackEventCategory>(unsigned int) /home/osboxes/projects/perfetto/out/build.issue_271/../../issue_271/subprojects/perfetto-v24.2/sdk/perfetto.h:1712:28
    #4 0x56459118f2df in perfetto::protos::pbzero::TrackEventCategory* perfetto::protos::pbzero::TrackEventDescriptor::add_available_categories<perfetto::protos::pbzero::TrackEventCategory>() /home/osboxes/projects/perfetto/out/build.issue_271/../../issue_271/subprojects/perfetto-v24.2/sdk/perfetto.h:26871:12
    #5 0x564590d25235 in perfetto::internal::TrackEventInternal::Initialize(perfetto::internal::TrackEventCategoryRegistry const&, bool (*)(perfetto::protos::gen::DataSourceDescriptor const&)) /home/osboxes/projects/perfetto/out/build.issue_271/../../issue_271/subprojects/perfetto-v24.2/sdk/perfetto.cc:43351:21
    #6 0x5645908ea445 in perfetto::internal::TrackEventDataSource<perfetto::TrackEvent, &(perfetto::internal::kCategoryRegistry)>::Register() /home/osboxes/projects/perfetto/out/build.issue_271/../../issue_271/subprojects/perfetto-v24.2/sdk/perfetto.h:15711:12
    #7 0x5645908e9bc4 in main /home/osboxes/projects/perfetto/out/build.issue_271/../../issue_271/issue_271.cc:28:2
    #8 0x7fd9dd9d2fcf in __libc_start_call_main csu/../sysdeps/nptl/libc_start_call_main.h:58:16
    #9 0x7fd9dd9d307c in __libc_start_main csu/../csu/libc-start.c:409:3
    #10 0x564590838984 in _start (/home/osboxes/projects/perfetto/out/build.issue_271/issue_271+0xda1984)
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior ../../issue_271/subprojects/perfetto-v24.2/sdk/perfetto.h:1488:37 in
