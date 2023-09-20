#pragma once

namespace PB {
enum class ErrorKind { Unknown, InvalidFile, FileDoesNotExist , NotADirectory, InputOutputFilesNotSet, CorruptPersistenceFile};
} // namespace PB