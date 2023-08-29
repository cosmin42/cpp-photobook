#pragma once

#include <string_view>

#include <util/Traits.h>

namespace PB {
enum class ErrorKind { Unknown, InvalidFile, FileDoesNotExist , NotADirectory, InputOutputFilesNotSet};
} // namespace PB