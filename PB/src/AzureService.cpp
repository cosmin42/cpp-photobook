#include <pb/services/AzureService.h>

#include <pb/infra/Traits.h>

#include <magic_enum/magic_enum.hpp>

namespace PB {
AzureService::AzureService() : mClient(mHost.c_str(), 443)
{
  mClient.enable_server_certificate_verification(true);
  mClient.enable_server_hostname_verification(true);
  mClient.set_read_timeout(5, 0);  // Timeout after 5 seconds for reading
  mClient.set_write_timeout(5, 0); // Timeout after 5 seconds for writing
}

void AzureService::configureListener(AzureServiceListener *listener)
{
  mListener = listener;
}

void AzureService::configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

void AzureService::subscribeAsync(const std::string &userEmail,
                                  const std::string &functionKey)
{
  mTaskCruncher->crunch(
      [this, userEmail, functionKey]() {
        subscribe(userEmail, functionKey);
      });
}

void AzureService::subscribe(const std::string &userEmail, const std::string &functionKey)
{
  std::string path =
      std::string() + "/api/SubscribeTrigger" + "?code=" + functionKey;
  
  Json requestContent = {
      {"email", userEmail}
  };
  auto res =
      mClient.Post(path.c_str(), requestContent.dump(), "application/json");

  if (res) {
    if (res->status == 200) {
      mListener->onSubscriptionSuccess();
    }
    else {
      mListener->onSubscriptionFailure("Subscription failed" +
                                       std::to_string(res->status));
    }
  }
  else {
    mListener->onSubscriptionFailure(std::string("Error: ") +
                                     magic_enum::enum_name(res.error()).data());
  }
}

} // namespace PB