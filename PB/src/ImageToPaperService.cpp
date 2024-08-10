#include <pb/ImageToPaperService.h>

namespace PB {

void ImageToPaperService::map(
    std::unordered_map<PBDev::ImageToPaperServiceId,
                       std::shared_ptr<VirtualImage>,
                       boost::hash<PBDev::ImageToPaperServiceId>>
        originalImages)
{
}

void ImageToPaperService::map(
    std::pair<PBDev::ImageToPaperServiceId, std::shared_ptr<VirtualImage>>
        image)
{
}

} // namespace PB
