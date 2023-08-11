#pragma once

#include <FileData.h>
#include <FolderData.h>
#include <map>
#include <util/Traits.h>
#include <variant>

#include <boost/uuid/uuid_generators.hpp>

namespace PB {
typedef std::variant<FileData, FolderData> EitherFolderOrFile;

struct DataNode {
  EitherFolderOrFile                                      data;
  std::map<boost::uuids::uuid, std::shared_ptr<DataNode>> children;

  operator std::string() const
  {
    std::string str;
    std::visit(
        overloaded{
            [&str](const FileData &fileData) { str += std::string(fileData); },
            [&str](const FolderData &folderData) { str += std::string(folderData); }},
        data);

    for (auto &[id, dataPointer]: children) {
      str += dataPointer->operator std::string();
    }
    return str;
  }
};
} // namespace PB