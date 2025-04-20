#include <pb/services/AzureService.h>

namespace PB
{
AzureService::AzureService(const std::string& host)
    : mHost(host){}

void AzureService::configureListener(AzureServiceListener* listener)
{
    mListener = listener;
}

void AzureService::subscribeAsync(const std::string& userEmail, const std::string& functionKey)
{

}
}