#pragma once

#include <pb/Project.h>
#include <pb/persistence/ProjectMetadata.h>

namespace PB {
class PersistenceVisitor final {
public:
  void visit(Project const &project);
  void visit(ProjectMetadata const &projectMetadata);
};
} // namespace PB
