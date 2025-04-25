#include <filesystem>
#include <pb/components/SVGInflater.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PB {

// TODO: Change this to be async
SVGInflater::SVGInflater(Path templatesFolder)
    : mTemplatesFolder(templatesFolder)
{
  auto success = std::filesystem::create_directories(templatesFolder);
  UNUSED(success);
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
  catch (const inja::ParserError &ex) {
#ifndef _CLANG_UML_
    spdlog::error("Error rendering SVG: {}", ex.what());
#endif
    return std::nullopt;
  }
  catch (const std::exception ex) {
#ifndef _CLANG_UML_
    spdlog::error("Error rendering SVG: {}", ex.what());
#endif
    return std::nullopt;
  }
}
} // namespace PB
