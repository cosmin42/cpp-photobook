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
  ProjectPathDoesNotExist,

  CouldNotLoadProjectFromGenericPersistence,
  SQLiteError
};

enum class LogLevel { None, Debug, Info, Warning, Error };
} // namespace PB