#pragma once

#include <pb/infra/Traits.h>
namespace PB {

struct ProcessingData {
  Path keyPath;
  Path resource;
  unsigned  position;
};

struct RowProcessingData {
  Path                        root;
  std::vector<ProcessingData> images;
};
} // namespace PB