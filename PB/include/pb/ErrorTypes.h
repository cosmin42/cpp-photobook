#pragma once

namespace PB {
enum class ErrorKind { Unknown, InvalidFile, FileDoesNotExist , NotADirectory, InputOutputFilesNotSet, CorruptPersistenceFile, InvalidPersistenceMap, CannotSaveFile};
} // namespace PB