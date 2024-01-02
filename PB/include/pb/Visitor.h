#pragma once

#include <pb/project/Project.h>
#include <pb/project/ProjectMetadata.h>

namespace PB {
class Visitor {
public:
  virtual void visit(Project const &project) = 0;
  virtual void visit(ProjectMetadata const &projectMetadata) = 0;
};
} // namespace PB