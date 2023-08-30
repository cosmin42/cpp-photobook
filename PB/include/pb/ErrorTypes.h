#pragma once

#include <string_view>

#include <pb/util/Traits.h>

namespace PB {
enum class ErrorKind { Unknown, InvalidFile, FileDoesNotExist , NotADirectory, InputOutputFilesNotSet};
} // namespace PB