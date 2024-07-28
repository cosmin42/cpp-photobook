#include <gtest/gtest.h>

#include <pb/CollageManager.h>

using namespace PB;

TEST(TestDrawingService, TestSvgToPng)
{
  CollageLibraryAssistant assistant(std::filesystem::current_path() /
                                        Path("svg-templates"),
                                    std::filesystem::current_path() / "res");

  AspectRatio aspectRatio = {4, 3};

  auto templatePath = std::filesystem::current_path() / Path("svg-templates") /
                      "2x1-simple.svg.template";

  auto numberedImages = assistant.createNumberedImages({2480 / 2, 1754 / 2});
  assistant.createTemplateThumbnail(numberedImages, templatePath, aspectRatio,
                                    {2480 / 2, 1754 / 2});

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
                                   "4to3-1240x877.svg",
                               {2480 / 2, 1754 / 2});
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

TEST(CollageManager, TestGenerateTemplatesImages)
{
  auto success = std::filesystem::create_directories(
      std::filesystem::current_path() / "test-generate-templates");

  auto project = std::make_shared<Project>();
  project->paperSettings.width = 2480;
  project->paperSettings.height = 1754;
  auto templatesPath =
      std::filesystem::current_path() / "test-generate-templates";
  CollageManager manager(templatesPath, std::filesystem::current_path());

  manager.generateTemplatesImages();

  std::filesystem::remove_all(std::filesystem::current_path() /
                              "test-generate-templates");
}