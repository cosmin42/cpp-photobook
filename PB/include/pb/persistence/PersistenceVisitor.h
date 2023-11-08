#pragma once

#include <pb/FilePersistence.h>
#include <pb/SQLPersistence.h>
#include <pb/Visitor.h>

namespace PB {
class PersistenceVisitor final : public Visitor {
public:
  void visit(Project const &project);

  void visit(ProjectMetadata const &projectMetadata);

  void openFile(Path path);
  void closeFile(Path path);

private:
  SQLitePersistence                         mCentral;
  std::unordered_map<Path, FilePersistence> mScattered;
};
} // namespace PB
