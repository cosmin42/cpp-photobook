#include <pb/project/PaperSettings.h>

namespace PB {

} // namespace PB

bool operator==(PB::PaperSettings const &lhs, PB::PaperSettings const &rhs)
{
  return lhs.type == rhs.type && lhs.ppi == rhs.ppi && lhs.width == rhs.width &&
         lhs.height == rhs.height;
}