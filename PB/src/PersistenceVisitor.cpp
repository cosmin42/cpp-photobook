#include <pb/persistence/PersistenceVisitor.h>

namespace PB {
void PersistenceVisitor::visit(Project const &project) {}

void PersistenceVisitor::visit(ProjectMetadata const &projectMetadata) {}

void PersistenceVisitor::openFile(Path path)
{
  mScattered[path] = FilePersistence(path);
}

void PersistenceVisitor::closeFile(Path path)
{
  if (mScattered.find(path) != mScattered.end()) {
    mScattered.erase(path);
  }
}

} // namespace PB