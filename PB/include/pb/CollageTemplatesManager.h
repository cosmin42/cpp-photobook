#pragma once

#include <pb/DrawingService.h>
#include <pb/SVGInflater.h>
#include <pb/project/Project.h>

namespace PB {
class CollageTemplatesManager {
public:
  CollageTemplatesManager(std::shared_ptr<Project> project);
  ~CollageTemplatesManager() = default;

  void generateTemplatesImages();

private:
  SVGInflater              mInflater;
  DrawingService           mDrawingService;
  std::shared_ptr<Project> mProject;
};
} // namespace PB
