#include <pb/PdfPoDoFo.h>

namespace PB {
PdfPoDoFoExport::PdfPoDoFoExport(std::stop_token stopToken) : Thread(stopToken)
	{

}

void PdfPoDoFoExport::executeSingleTask() {}

void PdfPoDoFoExport::finish() {}
} // namespace PB