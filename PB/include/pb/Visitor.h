#pragma once

#include <pb/Project.h>
#include <pb/persistence/ProjectMetadata.h>

namespace PB {
class Visitor {
public:
  void visit(Project const &project) = 0;
  void visit(ProjectMetadata const &projectMetadata) = 0;
};
} // namespace PB