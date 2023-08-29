#include <memory>

#include <PBArgsListener.h>
#include <PhotoBook.h>
#include <util/Timer.h>

auto main(int argc, char *argv[]) -> int
{
  PB::TimerWithPrint t;
  auto               mainActivity = std::shared_ptr<BL::MainActivity>();
  BL::PBArgsListener argsListener(mainActivity);
  BL::ArgsHandler    handler(argsListener);
  handler.handle(argc, argv);
  return 0;
}