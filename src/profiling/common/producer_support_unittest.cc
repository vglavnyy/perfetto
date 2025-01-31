/*
 * Copyright (C) 2020 The Android Open Source Project
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

#include "src/profiling/common/producer_support.h"

#include "perfetto/ext/base/file_utils.h"
#include "perfetto/ext/base/temp_file.h"
#include "perfetto/ext/base/utils.h"
#include "perfetto/tracing/core/data_source_config.h"
#include "test/gtest_and_gmock.h"

namespace perfetto {
namespace profiling {
namespace {

TEST(CanProfileAndroidTest, NonUserSystemExtraGuardrails) {
  DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(true);
  EXPECT_TRUE(CanProfileAndroid(ds_config, 1, {}, "userdebug", "/dev/null"));
}

TEST(CanProfileAndroidTest, NonUserNonProfileableApp) {
  DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(false);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example.profileable 10001 0 "
      "/data/user/0/invalid.example.profileable default:targetSdkVersion=10000 "
      "none 0 1\n";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_TRUE(CanProfileAndroid(ds_config, 10001, {}, "userdebug", tmp.path()));
}

TEST(CanProfileAndroidTest, NonUserNonProfileableAppExtraGuardrails) {
  DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(true);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example.profileable 10001 0 "
      "/data/user/0/invalid.example.profileable default:targetSdkVersion=10000 "
      "none 0 1\n";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_TRUE(CanProfileAndroid(ds_config, 10001, {}, "userdebug", tmp.path()));
}

TEST(CanProfileAndroidTest, UserProfileableApp) {
  DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(false);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example.profileable 10001 0 "
      "/data/user/0/invalid.example.profileable default:targetSdkVersion=10000 "
      "none 1 1\n";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_TRUE(CanProfileAndroid(ds_config, 10001, {}, "user", tmp.path()));
}

TEST(CanProfileAndroidTest, UserProfileableAppMultiuser) {
  DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(false);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example.profileable 10001 0 "
      "/data/user/0/invalid.example.profileable default:targetSdkVersion=10000 "
      "none 1 1\n";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_TRUE(CanProfileAndroid(ds_config, 210001, {}, "user", tmp.path()));
}

TEST(CanProfileAndroidTest, UserNonProfileableApp) {
  DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(false);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example.profileable 10001 0 "
      "/data/user/0/invalid.example.profileable default:targetSdkVersion=10000 "
      "none 0 1\n";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_FALSE(CanProfileAndroid(ds_config, 10001, {}, "user", tmp.path()));
}

TEST(CanProfileAndroidTest, UserDebuggableApp) {
  DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(false);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example.profileable 10001 1 "
      "/data/user/0/invalid.example.profileable default:targetSdkVersion=10000 "
      "none 0 1\n";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_TRUE(CanProfileAndroid(ds_config, 10001, {}, "user", tmp.path()));
}

TEST(CanProfileAndroidTest, UserProfileableMatchingInstallerStatsd) {
  DataSourceConfig ds_config;
  ds_config.set_session_initiator(
      DataSourceConfig::SESSION_INITIATOR_TRUSTED_SYSTEM);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example 10001 0 /data/user/0/invalid.example "
      "default:targetSdkVersion=29 3002,3003 0 13030407 1 invalid.store";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_TRUE(CanProfileAndroid(ds_config, 10001, {"invalid.store"}, "user",
                                tmp.path()));
}

TEST(CanProfileAndroidTest, UserProfileableMatchingInstallerShell) {
  DataSourceConfig ds_config;
  ds_config.set_session_initiator(
      DataSourceConfig::SESSION_INITIATOR_UNSPECIFIED);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example 10001 0 /data/user/0/invalid.example "
      "default:targetSdkVersion=29 3002,3003 0 13030407 1 invalid.store";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_FALSE(CanProfileAndroid(ds_config, 10001, {"invalid.store"}, "user",
                                 tmp.path()));
}

TEST(CanProfileAndroidTest, UserProfileableNonMatchingInstallerStatsd) {
  DataSourceConfig ds_config;
  ds_config.set_session_initiator(
      DataSourceConfig::SESSION_INITIATOR_TRUSTED_SYSTEM);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example 10001 0 /data/user/0/invalid.example "
      "default:targetSdkVersion=29 3002,3003 0 13030407 1 invalid.store";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_FALSE(CanProfileAndroid(ds_config, 10001, {"invalid.otherstore"},
                                 "user", tmp.path()));
}

TEST(CanProfileAndroidTest, UserProfileableNonMatchingInstallerShell) {
  DataSourceConfig ds_config;
  ds_config.set_session_initiator(
      DataSourceConfig::SESSION_INITIATOR_UNSPECIFIED);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example 10001 0 /data/user/0/invalid.example "
      "default:targetSdkVersion=29 3002,3003 0 13030407 1 invalid.store";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_FALSE(CanProfileAndroid(ds_config, 10001, {"invalid.otherstore"},
                                 "user", tmp.path()));
}

TEST(CanProfileAndroidTest, UserProfileableFromShellWithInstallerOldPackages) {
  DataSourceConfig ds_config;
  ds_config.set_session_initiator(
      DataSourceConfig::SESSION_INITIATOR_UNSPECIFIED);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example 10001 0 /data/user/0/invalid.example "
      "default:targetSdkVersion=29 3002,3003 1 13030407";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_FALSE(CanProfileAndroid(ds_config, 10001, {"invalid.otherstore"},
                                 "user", tmp.path()));
}

TEST(CanProfileAndroidTest, UserProfileableAppSdkSandbox) {
    DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(false);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example.profileable 10001 0 "
      "/data/user/0/invalid.example.profileable default:targetSdkVersion=10000 "
      "none 1 1\n";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_TRUE(CanProfileAndroid(ds_config, 20001, {}, "user", tmp.path()));
}

TEST(CanProfileAndroidTest, UserNonProfileableAppSdkSandbox) {
    DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(false);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example.profileable 10001 0 "
      "/data/user/0/invalid.example.profileable default:targetSdkVersion=10000 "
      "none 0 1\n";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_FALSE(CanProfileAndroid(ds_config, 20001, {}, "user", tmp.path()));
}

TEST(CanProfileAndroidTest, UserDebuggableAppSdkSandbox) {
  DataSourceConfig ds_config;
  ds_config.set_enable_extra_guardrails(false);
  auto tmp = base::TempFile::Create();
  constexpr char content[] =
      "invalid.example.profileable 10001 1 "
      "/data/user/0/invalid.example.profileable default:targetSdkVersion=10000 "
      "none 0 1\n";
  base::WriteAll(tmp.fd(), content, sizeof(content));
  EXPECT_TRUE(CanProfileAndroid(ds_config, 20001, {}, "user", tmp.path()));
}

}  // namespace
}  // namespace profiling
}  // namespace perfetto
