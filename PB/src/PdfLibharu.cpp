#include <pb/export/PdfLibharu.h>

namespace PB {
PdfLibharuExportTask::PdfLibharuExportTask(
    Path exportPdfPath, Path localStatePath, PaperSettings paperSettings,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages)
{
}

bool PdfLibharuExportTask::stoppingCondition() const { return true; }
int  PdfLibharuExportTask::stepsCount() const { return 0; }
void PdfLibharuExportTask::taskStep() {}

std::string PdfLibharuExportTask::name() const { return ""; }
} // namespace PB