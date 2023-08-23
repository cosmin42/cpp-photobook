#include <ArgsHandler.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <ConfigDependants.h>

namespace BL {
ArgsHandler::ArgsHandler(ArgsHandlerListener const &listener)
    : mListener(listener)
{
}

ArgsHandler::ArgsHandler(ArgsHandlerListener &&listener) : mListener(listener)
{
}

ArgsHandler::ArgsHandler(ArgsHandler const &other) : mListener(other.mListener)
{
}

ArgsHandler::ArgsHandler(ArgsHandler &&other) : mListener(other.mListener) {}

void ArgsHandler::handle(int argc, char *argv[]) const noexcept
{
  try {
    po::options_description desc("PhotoBook options");
    desc.add_options()(HELP_PARAM_NAME, "Produce help message")(
        INPUT_PARAM_NAME, po::value<std::string>(), "Set images input path.")(
        OUTPUT_PARAM_NAME, po::value<std::string>(), "Set images output path.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count(HELP_PARAM_NAME)) {
      std::stringstream strStream;
      desc.print(strStream);
      PB::printDebug("%s\n", strStream.str().c_str());

      mListener.handleHelpArgument();
    }

    if (vm.count(INPUT_PARAM_NAME)) {
      auto inputPath = vm[INPUT_PARAM_NAME].as<std::string>();
      PB::printDebug("Input path set to %s\n", inputPath.c_str());
      mListener.handleInputArgument(inputPath);
    }
    else {
      PB::printDebug("Usage: ./PhotoBook --input <\\input\\path> --output "
                     "<\\output\\path>.\n");
    }

    if (vm.count(OUTPUT_PARAM_NAME)) {
      auto outputPath = vm[OUTPUT_PARAM_NAME].as<std::string>();
      PB::printDebug("Output path set to %s\n", outputPath.c_str());

      mListener.handleOutputArgument(outputPath);
    }
    else {
      PB::printDebug("Usage: ./PhotoBook --input <\\input\\path> --output "
                     "<\\output\\path>.\n");
    }
  }
  catch (...) {
    PB::printError("Exception of unknown type!\n");
  }
}
} // namespace BL