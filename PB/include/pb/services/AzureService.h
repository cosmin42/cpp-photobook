#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <string>

#include <httplib.h>

#include <pb/infra/TaskCruncher.h>

namespace PB {
class AzureServiceListener {
public:
  virtual ~AzureServiceListener() = default;

  virtual void onSubscriptionSuccess() = 0;
  virtual void onSubscriptionFailure(const std::string &error) = 0;
};

class AzureService {
public:
  AzureService();
  ~AzureService() = default;

  void configureListener(AzureServiceListener *listener);
  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);

  void subscribeAsync(const std::string &userEmail,
                      const std::string &functionKey);

private:
  AzureServiceListener *mListener = nullptr;
  std::string           mHost = "newsletterfunction.azurewebsites.net";
  httplib::SSLClient    mClient;
  std::shared_ptr<TaskCruncher> mTaskCruncher = nullptr;

private:
  void subscribe(const std::string &userEmail, const std::string &functionKey);
};
} // namespace PB