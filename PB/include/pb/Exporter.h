#pragma once
#include <pb/Jpg.h>
#include <pb/PdfPoDoFo.h>

namespace PB {
template <typename ExporterType> class Exporter final {
public:
  virtual ~Exporter() = default;
  void exportImages(std::string name, Path dest, std::vector<Path> images)
  {
    mSpecific.exportImages(name, dest, images);
  }

private:
  ExporterType mSpecific;
};

class ExportFactory final {
public:
  void updateConfiguration(PaperSettings paperSettings,
                           Path           temporaryDirectory)
  {
    mPaperSettings = paperSettings;
    mTemporaryDirectory = temporaryDirectory;
  }

  std::shared_ptr<Exportable> makePdf(std::string name, Path destination,
                                      std::vector<Path> images)
  {
    std::shared_ptr<Exportable> exporter = std::make_shared<PdfPoDoFoExport>(
        Context::inst().sStopSource.get_token(), mPaperSettings,
        mTemporaryDirectory);
    exporter->configureExport(name, destination, images);

    return exporter;
  }

private:
  PaperSettings mPaperSettings;
  Path          mTemporaryDirectory = Path();
};
} // namespace PB