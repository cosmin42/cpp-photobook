#pragma once

#include <string>

#include <httplib.h>

namespace PB {
class AzureServiceListener {
public:
  virtual ~AzureServiceListener() = default;

  virtual void onSubscriptionSuccess() = 0;
  virtual void onSubscriptionFailure(const std::string &error) = 0;
};

class AzureService {
public:
  AzureService(const std::string &host);
  ~AzureService() = default;

  void configureListener(AzureServiceListener *listener);

  void subscribeAsync(const std::string &userEmail,
                      const std::string &functionKey);

private:
  AzureServiceListener *mListener = nullptr;
  std::string           mHost;
  httplib::Client       mClient;
};
} // namespace PB