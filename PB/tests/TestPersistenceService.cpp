#include "MockListeners.h"

#include <pb/Config.h>
#include <pb/persistence/PersistenceService.h>


TEST(PersistenceService, Test0)
{
  clearProjectCache();
  TestPersistenceServiceListener testPersistenceServiceListener;
}