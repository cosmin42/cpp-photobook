#include "MockListeners.h"

#include <pb/Config.h>
#include <pb/persistence/ProjectPersistence.h>


TEST(ProjectPersistence, Test0)
{
  clearProjectCache();
  TestProjectPersistenceListener testProjectPersistenceListener;
}