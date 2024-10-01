#include <pb/components/Serializer.h>

#include <boost/uuid/uuid_io.hpp>

namespace PB {
template <>
std::variant<Json, PBDev::Error> flatSimple(int depth, boost::uuids::uuid uuid)
{
  Json json;
  json = boost::uuids::to_string(uuid);
#ifndef _CLANG_UML_
  spdlog::info("{}(string, uuid) {}\n", std::string(depth * 2, ' '),
               json.dump());
#endif
  return json;
}

} // namespace PB
