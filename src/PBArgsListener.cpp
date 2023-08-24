#include <PBArgsListener.h>

namespace BL {
void PBArgsListener::handleInputArgument(std::string inputPath) const
{
  PB::printDebug("Handle input: %s\n", inputPath.c_str());
}

void PBArgsListener::handleOutputArgument(std::string outputPath) const
{
  PB::printDebug("Handle output: %s\n", outputPath.c_str());
}

void PBArgsListener::handleHelpArgument() const
{
  PB::printDebug("Handle help\n");
}
} // namespace BL