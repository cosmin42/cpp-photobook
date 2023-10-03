#pragma once
namespace PB {
template <typename ExporterType> class Exporter final {
public:
  void exportImages(Path dest, std::vector<Path> images)
  {
    mSpecific.exportImages(dest, images);
  }

private:
  ExporterType mSpecific;
};
} // namespace PB