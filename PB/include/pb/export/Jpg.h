#pragma once


namespace PB {
class JpgExport final {
public:
  bool stoppingCondition() const;
  int  stepsCount() const;
  void taskStep();
};
} // namespace PB