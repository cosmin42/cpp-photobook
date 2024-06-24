#include <pb/CollageTemplatesManager.h>

namespace PB {
CollageTemplatesManager::CollageTemplatesManager(
    std::shared_ptr<Project> project)
    : mInflater(std::filesystem::current_path() / "res" / "svg-templates"),
      mDrawingService(std::make_shared<SkiaResources>()), mProject(project)
{
}

void CollageTemplatesManager::generateTemplatesImages()
{

}

} // namespace PB
