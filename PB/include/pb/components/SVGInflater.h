#pragma once

#include <optional>

#include <pb/entities/BasicSVGModel.h>

DECLARE_STRONG_UUID(SVGTemplateId)

namespace PB {
class SVGInflater final {
public:
  explicit SVGInflater(Path templatesFolder);
  ~SVGInflater() = default;
  std::optional<std::string> inflate(BasicSVGModel svgModel, Path svgFile);

private:
  Path mTemplatesFolder;
  std::unordered_map<PBDev::SVGTemplateId, Path,
                     boost::hash<PBDev::SVGTemplateId>>
      mTemplates;
};
} // namespace PB
