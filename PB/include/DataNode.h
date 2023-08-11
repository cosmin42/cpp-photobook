#pragma once

#include <FileData.h>
#include <FolderData.h>
#include <map>
#include <variant>

#include <boost/uuid/uuid_generators.hpp>

namespace PB {
typedef std::variant<FileData, FolderData> EitherFolderOrFile;
struct DataNode {
  EitherFolderOrFile                               data;
  std::map<boost::uuids::uuid, EitherFolderOrFile> children;
};
} // namespace PB