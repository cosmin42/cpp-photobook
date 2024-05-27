#include <filesystem>
#include <pb/SVGInflater.h>

namespace PB {

// TODO: Change this to be async
SVGInflater::SVGInflater(Path templatesFolder)
    : mTemplatesFolder(templatesFolder)
{
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

  return inja::render(content, svgModel.toJson());
}
} // namespace PB
