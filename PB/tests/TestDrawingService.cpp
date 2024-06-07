#include <gtest/gtest.h>

#include <pb/CollageLibraryAssistant.h>
#include <pb/DrawingService.h>

using namespace PB;

TEST(TestDrawingService, TestSvgToPng)
{
  CollageLibraryAssistant assistant(std::filesystem::current_path(),
                                    std::filesystem::current_path());

  AspectRatio aspectRatio = {4, 3};

  auto templatePath = std::filesystem::current_path() / Path("svg-templates") /
                      "2x1-simple.svg.template";

  assistant.createTemplateThumbnail(templatePath, aspectRatio, {2480, 1754});

  auto &drawingServce = DrawingService::getInstance();

  drawingServce.renderSVG(
      std::filesystem::current_path() / "template_4to3-2480x1754.svg",
      std::filesystem::current_path() / "template_4to3-2480x1754.png",
      {2480, 1754});

  for (auto i = 0; i < 10; ++i) {
    std::filesystem::remove(std::filesystem::current_path() /
                            (std::string("placeolder_") + std::to_string(i)));
  }

  std::filesystem::remove(std::filesystem::current_path() /
                          "template_4to3-2480x1754.svg");
}