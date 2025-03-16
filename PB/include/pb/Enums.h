#pragma once

namespace PB {
enum class ErrorCode {
  Unknown,
  FileDoesNotExist,
  NotADirectory,
  CorruptPersistenceFile,
  SQLiteError,

  FolderAlreadyImported,
  JSONParseError,

  NoImages,
  CannotExport,
  WaitForLoadingCompletion,
  CannotApplyEffect
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
  Custom,
  Square_Paper
};

enum class ImageType { None, Text, Regular, Collage };

enum class JobType {
  None,
  Map,
  ThumbnailsProcess,
  ExportPdf,
  ExportJpg,
  Full,
  ExportLibharu
};

enum class ProgressType { None, Defined, Undefined };

enum class ExportType {
  None = 0x0,
  Pdf = 0x1,
  Jpg = 0x2,
  Html = 0x4,
  PdfLibharu = 0x8
};

enum class ImageProcessingType { None, LUT, LutInMemory };
} // namespace PB