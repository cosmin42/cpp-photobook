#include <pb/export/Jpg.h>

namespace PB {

bool JpgExport::stoppingCondition() const { return true; }

int JpgExport::stepsCount() const { return 0; }

void JpgExport::taskStep() {}
} // namespace PB