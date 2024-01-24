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

enum class MediaMapState { None, Started, Finished, Aborted };

enum class ThumbnailType { None = 0, Small, Medium };

ThumbnailType operator|(ThumbnailType lhs, ThumbnailType rhs);
ThumbnailType operator&(ThumbnailType lhs, ThumbnailType rhs);

enum class PaperType {
  None,
  A4_Landscape,
  A4_Portrait,
  A5_Landscape,
  A5_Portrait,
  A3_Landscape,
  A3_Portrait,
  Custom
};

enum class VirtualImageType { None, Text, Regular, Test };

enum class JobType { None, Map, ThumbnailsProcess, Export };
} // namespace PB