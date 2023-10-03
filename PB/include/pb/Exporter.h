#pragma once
namespace PB {
template <typename ExporterType> class Exporter final {
public:
  ExporterType mSpecific;
};
} // namespace PB