#pragma once

#include <FileData.h>
#include <FolderData.h>
#include <map>
#include<regex>
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
    str += "\n";
    std::string childrenStr;
    for (auto &[id, dataPointer]: children) {
      childrenStr += dataPointer->operator std::string();
    }
    childrenStr = std::regex_replace(childrenStr, std::regex(R"(^)"), "  ");
    str += childrenStr;
    str = std::regex_replace(str, std::regex(R"(^)"), "  ");
    return str;
  }
};
} // namespace PB