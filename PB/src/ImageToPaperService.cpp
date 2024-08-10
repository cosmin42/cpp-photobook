#include <pb/ImageToPaperService.h>

namespace PB {

void ImageToPaperService::map(
    PBDev::ImageToPaperServiceId id,
    std::unordered_map<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>,
                       boost::hash<PBDev::ImageToPaperId>>
        originalImages)
{
  auto &&task = ImageToPaperTask(originalImages);
  mTasks.emplace(id, task);
}

void ImageToPaperService::map(
    PBDev::ImageToPaperServiceId                                    id,
    std::pair<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>> image)
{

  mTasks.emplace(
      id, ImageToPaperTask(
              std::unordered_map<PBDev::ImageToPaperId,
                                 std::shared_ptr<VirtualImage>,
                                 boost::hash<PBDev::ImageToPaperId>>{image}));
}

void ImageToPaperService::removeTask(PBDev::ImageToPaperServiceId id)
{
  mTasks.erase(id);
}

} // namespace PB
