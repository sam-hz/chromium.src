// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/extensions/manifest_handlers/ui_overrides_handler.h"

#include "base/command_line.h"
#include "base/json/json_string_value_serializer.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/extensions/features/feature_channel.h"
#include "chrome/common/extensions/manifest_url_handler.h"
#include "extensions/common/error_utils.h"
#include "extensions/common/extension.h"
#include "extensions/common/manifest_constants.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

const char kManifest[] = "{"
    " \"version\" : \"1.0.0.0\","
    " \"name\" : \"Test\","
    " \"chrome_ui_overrides\" : {"
    "   \"bookmarks_ui\" : {"
    "        \"remove_button\" : true,"
    "        \"remove_bookmark_shortcut\" : true"
    "    }"
    "  }"
    "}";

const char kBrokenManifest[] = "{"
    " \"version\" : \"1.0.0.0\","
    " \"name\" : \"Test\","
    " \"chrome_ui_overrides\" : {"
    "  }"
    "}";

using extensions::api::manifest_types::ChromeUIOverrides;
using extensions::Extension;
using extensions::Manifest;
using extensions::UIOverrides;
namespace manifest_keys = extensions::manifest_keys;

class UIOverrideTest : public testing::Test {
};


TEST_F(UIOverrideTest, ParseManifest) {
  extensions::ScopedCurrentChannel channel(chrome::VersionInfo::CHANNEL_DEV);
  // This functionality requires a feature flag.
  CommandLine::ForCurrentProcess()->AppendSwitchASCII(
      "--enable-override-bookmarks-ui",
      "1");
  std::string manifest(kManifest);
  JSONStringValueSerializer json(&manifest);
  std::string error;
  scoped_ptr<base::Value> root(json.Deserialize(NULL, &error));
  ASSERT_TRUE(root);
  ASSERT_TRUE(root->IsType(base::Value::TYPE_DICTIONARY));
  scoped_refptr<Extension> extension = Extension::Create(
      base::FilePath(FILE_PATH_LITERAL("//nonexistent")),
      Manifest::INVALID_LOCATION,
      *static_cast<base::DictionaryValue*>(root.get()),
      Extension::NO_FLAGS,
      &error);
  ASSERT_TRUE(extension) << error;
  ASSERT_TRUE(extension->manifest()->HasPath(manifest_keys::kUIOverride));

  UIOverrides* ui_override = static_cast<UIOverrides*>(
      extension->GetManifestData(manifest_keys::kUIOverride));
  ASSERT_TRUE(ui_override);
  ASSERT_TRUE(ui_override->bookmarks_ui);
  EXPECT_TRUE(ui_override->bookmarks_ui->remove_button);
  EXPECT_TRUE(ui_override->bookmarks_ui->remove_bookmark_shortcut);
}

TEST_F(UIOverrideTest, ParseBrokenManifest) {
  extensions::ScopedCurrentChannel channel(chrome::VersionInfo::CHANNEL_DEV);
  // This functionality requires a feature flag.
  CommandLine::ForCurrentProcess()->AppendSwitchASCII(
      "--enable-override-bookmarks-ui",
      "1");
  std::string manifest(kBrokenManifest);
  JSONStringValueSerializer json(&manifest);
  std::string error;
  scoped_ptr<base::Value> root(json.Deserialize(NULL, &error));
  ASSERT_TRUE(root);
  ASSERT_TRUE(root->IsType(base::Value::TYPE_DICTIONARY));
  scoped_refptr<Extension> extension = Extension::Create(
      base::FilePath(FILE_PATH_LITERAL("//nonexistent")),
      Manifest::INVALID_LOCATION,
      *static_cast<base::DictionaryValue*>(root.get()),
      Extension::NO_FLAGS,
      &error);
  EXPECT_FALSE(extension);
  EXPECT_EQ(
      extensions::ErrorUtils::FormatErrorMessage(
          extensions::manifest_errors::kInvalidEmptyDictionary,
          extensions::manifest_keys::kUIOverride),
      error);
}

}  // namespace
