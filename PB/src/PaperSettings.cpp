#include <pb/project/PaperSettings.h>

namespace PB {

PaperType deserializePaperType(std::string paperType)
{
  if (paperType == "A4 Landscape") {
    return PaperType::A4_Landscape;
  }
  else if (paperType == "A4 Portrait") {
    return PaperType::A4_Portrait;
  }
  else if (paperType == "A3 Landscape") {
    return PaperType::A3_Landscape;
  }
  else if (paperType == "A3 Portrait") {
    return PaperType::A3_Portrait;
  }
  else if (paperType == "A5 Landscape") {
    return PaperType::A5_Landscape;
  }
  else if (paperType == "A5 Portrait") {
    return PaperType::A5_Portrait;
  }
  else if (paperType == "Square") {
    return PaperType::Square_Paper;
  }
  else if (paperType == "Custom") {
    return PaperType::Custom;
  }
  else {
    PBDev::basicAssert(false);
  }
  return PaperType::A4_Landscape;
}

} // namespace PB

bool operator==(PB::PaperSettings const &lhs, PB::PaperSettings const &rhs)
{
  return lhs.type == rhs.type && lhs.ppi == rhs.ppi && lhs.width == rhs.width &&
         lhs.height == rhs.height;
}