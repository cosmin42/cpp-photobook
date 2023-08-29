#pragma once
#include <memory>

#include <ArgsHandler.h>
#include <MainActivity.h>

namespace BL {
class PBArgsListener final : public ArgsHandlerListener {
public:
  PBArgsListener() = delete;
  explicit PBArgsListener(std::shared_ptr<MainActivity> mainActivity);
  PBArgsListener(PBArgsListener const &) = delete;
  PBArgsListener(PBArgsListener &&) = delete;
  PBArgsListener &operator=(PBArgsListener const &) = delete;
  ~PBArgsListener() = default;

  void handleInputArgument(std::string inputPath) const override;

  void handleOutputArgument(std::string outputPath) const override;

  void handleHelpArgument() const override;

private:
  std::shared_ptr<MainActivity> mMainActivity = nullptr;
};
} // namespace BL