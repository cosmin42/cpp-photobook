#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
using namespace std;

#include <ConfigDependants.h>

auto main(int argc, char *argv[]) -> int
{
  try {
    po::options_description desc("PhotoBook options");
    desc.add_options()("help", "Produce help message")(
        "input", po::value<std::string>(), "Set images input path.")(
        "output", po::value<std::string>(), "Set images output path.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::stringstream strStream;
      desc.print(strStream);
      PB::printDebug("%s\n", strStream.str().c_str());
      return 0;
    }

    if (vm.count("input")) {
      PB::printDebug("Input path set to %s\n",
                     vm["input"].as<std::string>().c_str());
    }
    else {
      PB::printDebug("Usage: ./PhotoBook --input <\\input\\path> --output "
                     "<\\output\\path>.\n");
    }

    if (vm.count("output")) {
      PB::printDebug("Output path set to %s\n",
                     vm["output"].as<std::string>().c_str());
    }
    else {
      PB::printDebug("Usage: ./PhotoBook --input <\\input\\path> --output "
                     "<\\output\\path>.\n");
    }
  }
  catch (exception &e) {
    cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch (...) {
    cerr << "Exception of unknown type!\n";
  }
  return 0;
}