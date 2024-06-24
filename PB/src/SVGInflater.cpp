#include <filesystem>
#include <pb/SVGInflater.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PB {

// TODO: Change this to be async
SVGInflater::SVGInflater(Path templatesFolder)
    : mTemplatesFolder(templatesFolder)
{
  if (!std::filesystem::exists(templatesFolder)) {
    auto success = std::filesystem::create_directories(templatesFolder);
    PBDev::basicAssert(success);
  }
  for (const auto &entry :
       std::filesystem::directory_iterator(templatesFolder)) {
    if (entry.is_regular_file() && entry.path().extension() == ".template") {
      mTemplates[PBDev::SVGTemplateId(RuntimeUUID::newUUID())] = entry.path();
    }
  }
}

std::optional<std::string> SVGInflater::inflate(BasicSVGModel svgModel,
                                                Path          svgFile)
{
  std::ifstream file(svgFile);
  std::string   content;
  std::string   inflatedContent;

  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      content += line + "\n";
    }
    file.close();
  }
  else {
    return std::nullopt;
  }

  auto model = svgModel.toJson();
  try {
    return inja::render(content, model);
  }
  catch (const std::exception ex) {
    spdlog::error("Error rendering SVG: {}", ex.what());
    return std::nullopt;
  }
}
} // namespace PB
