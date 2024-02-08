#pragma once

#include <pb/util/Traits.h>

struct RowProcessingData {
  Path                               root;
  std::vector<std::pair<Path, Path>> images;
};