#include <gtest/gtest.h>

#include <pb/CollageLibraryAssistant.h>
#include <pb/DrawingService.h>

using namespace PB;

TEST(TestDrawingService, TestSvgToPng)
{
  CollageLibraryAssistant assistant(std::filesystem::current_path() / "res",
                                    std::filesystem::current_path());

  AspectRatio aspectRatio = {4, 3};

  auto templatePath = std::filesystem::current_path() / Path("svg-templates") /
                      "2x1-simple.svg.template";

  assistant.createTemplateThumbnail(templatePath, aspectRatio, {2480/2, 1754/2});

  std::shared_ptr<SkiaResources> resources = std::make_shared<SkiaResources>();

  auto id =
      resources->addResource(std::filesystem::current_path() / "res" / "");

  DrawingService drawingServce(resources);

  SkFILEWStream outFile(
      (std::filesystem::current_path() / "testCollageTemplate.png")
          .string()
          .c_str());

  drawingServce.renderToStream(id, outFile,
                               std::filesystem::current_path() / "res" /
                                   "template_4to3-1240x877.svg",
                               {2480/2, 1754/2});
  outFile.~SkFILEWStream();
  for (auto i = 0; i < 10; ++i) {
    Path fileToRemove =
        std::filesystem::current_path() /
        (std::string("placeholder_") + std::to_string(i) + ".jpg");
    std::filesystem::remove(fileToRemove);
  }

  Path thumbnailToRemove =
      (std::filesystem::current_path() / "testCollageTemplate.png");
  std::filesystem::remove(thumbnailToRemove);
}