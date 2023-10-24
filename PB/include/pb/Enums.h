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
  SQLiteError,

  FolderAlreadyImported,
  JSONParseError
};

enum class LogLevel { None, Debug, Info, Warning, Error };

enum class PhotoLine { None, Unstaged, Staged };

enum class MediaMapState { None, Started, Finished };

enum class ThumbnailType { None = 0, Small, Medium };

ThumbnailType operator|(ThumbnailType lhs, ThumbnailType rhs);
ThumbnailType operator&(ThumbnailType lhs, ThumbnailType rhs);
} // namespace PB