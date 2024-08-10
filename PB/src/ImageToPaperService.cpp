#include <pb/ImageToPaperService.h>

namespace PB {

void ImageToPaperService::map(
    std::unordered_map<PBDev::ImageToPaperId,
                       std::shared_ptr<VirtualImage>,
                       boost::hash<PBDev::ImageToPaperId>>
        originalImages)
{
  
}

void ImageToPaperService::map(
    std::pair<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>>
        image)
{


  
}

void ImageToPaperService::removeTask(PBDev::ImageToPaperServiceId id) {

}

} // namespace PB
