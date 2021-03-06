// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/assets/zip_asset_store.h"

#include <fcntl.h>
#include <unistd.h>

#include <utility>

#include "flutter/assets/unzip_job.h"
#include "flutter/glue/data_pipe_utils.h"
#include "lib/ftl/files/eintr_wrapper.h"
#include "lib/ftl/files/unique_fd.h"
#include "lib/zip/unique_unzipper.h"
#include "third_party/zlib/contrib/minizip/unzip.h"

namespace blink {

ZipAssetStore::ZipAssetStore(UnzipperProvider unzipper_provider,
                             ftl::RefPtr<ftl::TaskRunner> task_runner)
    : unzipper_provider_(std::move(unzipper_provider)),
      task_runner_(std::move(task_runner)) {}

ZipAssetStore::~ZipAssetStore() {}

void ZipAssetStore::AddOverlayFile(std::string asset_name,
                                   std::string file_path) {
  overlay_files_.emplace(std::move(asset_name), std::move(file_path));
}

void ZipAssetStore::GetAsStream(const std::string& asset_name,
                                mojo::ScopedDataPipeProducerHandle producer) {
  auto overlay = overlay_files_.find(asset_name);
  if (overlay != overlay_files_.end()) {
    // TODO(abarth): Consider moving the |open| call to task_runner_.
    ftl::UniqueFD fd(HANDLE_EINTR(open(overlay->second.c_str(), O_RDONLY)));
    if (fd.get() < 0)
      return;
    glue::CopyFromFileDescriptor(std::move(fd), std::move(producer),
                                 task_runner_, [](bool ignored) {});
  } else {
    zip::UniqueUnzipper unzipper = unzipper_provider_();
    if (!unzipper.is_valid())
      return;
    new UnzipJob(std::move(unzipper), asset_name, std::move(producer),
                 task_runner_);
  }
}

bool ZipAssetStore::GetAsBuffer(const std::string& asset_name,
                                std::vector<uint8_t>* data) {
  zip::UniqueUnzipper unzipper = unzipper_provider_();
  if (!unzipper.is_valid())
    return false;

  int result = unzLocateFile(unzipper.get(), asset_name.c_str(), 0);
  if (result != UNZ_OK) {
    return false;
  }

  unz_file_info file_info;
  result = unzGetCurrentFileInfo(unzipper.get(), &file_info, nullptr, 0,
                                 nullptr, 0, nullptr, 0);
  if (result != UNZ_OK) {
    FTL_LOG(WARNING) << "unzGetCurrentFileInfo failed, error=" << result;
    return false;
  }

  result = unzOpenCurrentFile(unzipper.get());
  if (result != UNZ_OK) {
    FTL_LOG(WARNING) << "unzOpenCurrentFile failed, error=" << result;
    return false;
  }

  data->resize(file_info.uncompressed_size);
  int total_read = 0;
  while (total_read < static_cast<int>(data->size())) {
    int bytes_read = unzReadCurrentFile(
        unzipper.get(), data->data() + total_read, data->size() - total_read);
    if (bytes_read <= 0)
      return false;
    total_read += bytes_read;
  }

  return true;
}

}  // namespace blink
