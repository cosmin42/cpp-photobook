#include <PBArgsListener.h>

auto main(int argc, char *argv[]) -> int
{
  BL::PBArgsListener argsListener;
  BL::ArgsHandler handler(argsListener);
  handler.handle(argc, argv);
  return 0;
}