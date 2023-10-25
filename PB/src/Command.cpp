#include <pb/Command.h>

namespace PB {
void CommandStack::push(Command const &) {}

void CommandStack::undo() {}
void CommandStack::redo() {}

std::optional<Command> CommandStack::currentCommand() const
{
  return std::nullopt;
}

std::optional<Command> CommandStack::nextCommand() const
{
  return std::nullopt;
}

} // namespace PB