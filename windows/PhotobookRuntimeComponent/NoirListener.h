#pragma once
#include <pb/NoirListener.h>

#include "LutIconInfo.g.h"
#include "LutIconInfo.h"

#include "PBError.h"

namespace winrt::PhotobookRuntimeComponent::implementation {

// TODO: Is this class even used anymore?
class NoirListener : public PB::NoirListener {
public:
  explicit NoirListener(
      PhotobookRuntimeComponent::NoirListener const &managedListener)
      : mManagedListener(managedListener)
  {
  }

  void onNoirLutAdded(PB::LutIconInfo iconInfo)
  {
    mManagedListener.OnNoirLutAdded(winrt::make<LutIconInfo>(iconInfo));
  }
  void onNoirError(PBDev::Error error)
  {
    mManagedListener.OnNoirError(winrt::make<PBError>(error));
  }

private:
  // TODO: Check if not & is enough;
  PhotobookRuntimeComponent::NoirListener mManagedListener;
};

} // namespace winrt::PhotobookRuntimeComponent::implementation