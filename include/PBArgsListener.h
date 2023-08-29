#pragma once
#include <ArgsHandler.h>

namespace BL {
class PBArgsListener final : public ArgsHandlerListener {
public:
  PBArgsListener() = default;
  ~PBArgsListener() = default;

  void handleInputArgument(std::string inputPath) const override;

  void handleOutputArgument(std::string outputPath) const override;

  void handleHelpArgument() const override;

private:
};
} // namespace BL