#include <pb/image/VirtualImage.h>

namespace PB {
std::shared_ptr<PlatformInfo> VirtualImage::platformInfo = nullptr;
std::string                   VirtualImage::projectName;
} // namespace PB