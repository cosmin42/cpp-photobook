#include <pb/export/Jpg.h>

namespace PB {

bool JpgExport::stoppingCondition() const { return true; }

int JpgExport::stepsCount() const { return 0; }

void JpgExport::taskStep() {}
/*
std::string makeName(unsigned counter)
{
    struct JPG_TEMPLATE_PARAMS
    {
        static constexpr const char* JPG_NAME_TEMPLATE = "PHOTO_0000000.JPG";
        static constexpr unsigned PLACEHOLDER_SIZE = 7;
        static constexpr unsigned PLACEHOLDER_OFFSET = 6;
    };
    
    auto counterStr = std::to_string(counter);
    PBDev::basicAssert(counterStr.size() <= JPG_TEMPLATE_PARAMS::PLACEHOLDER_SIZE);
    std::string result = JPG_TEMPLATE_PARAMS::JPG_NAME_TEMPLATE;
    unsigned replaceOffset = JPG_TEMPLATE_PARAMS::PLACEHOLDER_OFFSET + (JPG_TEMPLATE_PARAMS::PLACEHOLDER_SIZE-counterStr.length());
    result.replace(replaceOffset, counterStr.size(), counterStr);

    return result;
}
*/
} // namespace PB
