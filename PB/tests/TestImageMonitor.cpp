#include "MockListeners.h"

#include <pb/ImageMonitor.h>

using ::testing::_;
using ::testing::AtLeast;

TEST(TestImageMonitor, Test0)
{ 
	PB::ImageMonitor imageMonitor;
}