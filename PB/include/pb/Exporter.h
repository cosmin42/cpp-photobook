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
  static Exportable makePdf(std::string name, Path destination,
                            std::vector<Path> images)
  {
    PdfPoDoFoExport exporter =
        PdfPoDoFoExport(Context::inst().sStopSource.get_token());
    exporter.configureExport(name, destination, images);

    return exporter;
  }
};
} // namespace PB