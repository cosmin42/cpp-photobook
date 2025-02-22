#pragma once

#include <boost/uuid/uuid.hpp>

class WinConversions {
public:
  static boost::uuids::uuid toNativeUuid(winrt::guid managedUuid);
  static winrt::guid        toManagedGuid(boost::uuids::uuid nativeUuid);
};
