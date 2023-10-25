#pragma once

#include <pb/util/Traits.h>

namespace PB {
struct Command {};

struct PhotoAddition : Command {
  std::vector<Path> photo;
  int               position;
};

class CommandStack {
public:
  void push(Command const &);

  void undo();
  void redo();

  std::optional<Command> currentCommand() const;
  std::optional<Command> nextCommand() const;

private:
  std::vector<Command> mCommands;
  int                  mIndex = -1;
};

} // namespace PB