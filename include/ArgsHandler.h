#pragma once

#include <string>

namespace BL {

class ArgsHandlerListener {
public:
  virtual void handleInputArgument(std::string inputPath) const = 0;
  virtual void handleOutputArgument(std::string outputPath) const = 0;
  virtual void handleHelpArgument() const = 0;
};

class ArgsHandler final {
public:
  ArgsHandler() = delete;
  explicit ArgsHandler(ArgsHandlerListener const &listener);
  explicit ArgsHandler(ArgsHandlerListener &&listener);

  ArgsHandler(ArgsHandler const &other);
  ArgsHandler(ArgsHandler &&other);
  ArgsHandler &operator=(ArgsHandler const &other) = delete;
  ArgsHandler &operator=(ArgsHandler &&other) = delete;
  ~ArgsHandler() = default;

  void handle(int argc, char *argv[]) const noexcept;

private:
  static constexpr char const *INPUT_PARAM_NAME = "input";
  static constexpr char const *OUTPUT_PARAM_NAME = "output";
  static constexpr char const *HELP_PARAM_NAME = "help";

  ArgsHandlerListener const &mListener;
};

} // namespace BL