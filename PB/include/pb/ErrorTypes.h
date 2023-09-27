#pragma once

namespace PB {
enum class ErrorCode {
  Unknown,
  InvalidFile,
  FileDoesNotExist,
  NotADirectory,
  InputOutputFilesNotSet,
  CorruptPersistenceFile,
  InvalidPersistenceMap,
  CannotSaveFile,
  InvalidUUID,
  InvalidProjectDescription,
  ProjectPathDoesNotExist
};
} // namespace PB