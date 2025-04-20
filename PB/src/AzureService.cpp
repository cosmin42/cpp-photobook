#include <pb/services/AzureService.h>

namespace PB
{
AzureService::AzureService(const std::string &host)
    : mHost(host), mClient(mHost)
{

}

void AzureService::configureListener(AzureServiceListener* listener)
{
    mListener = listener;
}

void AzureService::subscribeAsync(const std::string& userEmail, const std::string& functionKey)
{

}
}