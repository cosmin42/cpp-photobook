#pragma once
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

template <typename ExporterType>
class ExportFactory final
{

};
} // namespace PB