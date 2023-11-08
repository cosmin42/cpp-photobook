#pragma once

#include <pb/Visitor.h>

namespace PB {
class PersistenceVisitor final : public Visitor {
public:
  void visit(Project const &project);
  void visit(ProjectMetadata const &projectMetadata);
};
} // namespace PB
