#include <pb/components/Serializer.h>

#include <magic_enum.hpp>

#include <boost/uuid/uuid_io.hpp>

#include <pb/entities/GenericImage.h>
#include <pb/entities/PaperSettings.h>

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

template <>
std::variant<Json, PBDev::Error> flatSimple(int depth, ImageType imageType)
{
  Json json;
  json = magic_enum::enum_name(imageType);
  return json;
}

template <>
std::variant<Json, PBDev::Error> flatSimple(int             depth,
                                            GenericImagePtr genericImage)
{
  return flatDictionary<std::string, ImageType>(
      depth, std::make_tuple("hash", genericImage->hash()),
      std::make_tuple("type", genericImage->type()));
}

template <>
std::variant<Json, PBDev::Error> flatSimple(int depth, PaperType genericImage)
{
  Json json;
  json = magic_enum::enum_name(genericImage);
  return json;
}

template <>
std::variant<Json, PBDev::Error> flatSimple(int depth, PaperSettings paper)
{
  return flatDictionary<PaperType, int, int, int>(
      depth, std::make_tuple("type", paper.type),
      std::make_tuple("ppi", paper.ppi), std::make_tuple("width", paper.width),
      std::make_tuple("height", paper.height));
}

} // namespace PB
