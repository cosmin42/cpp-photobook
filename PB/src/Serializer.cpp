#include <pb/components/Serializer.h>

#include <magic_enum.hpp>

#include <boost/uuid/uuid_io.hpp>

#include <pb/entities/CollageImage.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/PaperSettings.h>
#include <pb/entities/RegularImageV2.h>
#include <pb/entities/TextImageV2.h>
#include <pb/project/Project.h>

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

std::variant<Json, PBDev::Error>
flatRegularImage(int depth, std::shared_ptr<RegularImageV2> regularImage)
{
  return flatDictionary<std::string, Path, ImageType>(
      depth, std::make_tuple("hash", regularImage->hash()),
      std::make_tuple("original", regularImage->original()),
      std::make_tuple("type", regularImage->type()));
}

std::variant<Json, PBDev::Error>
flatTextImage(int depth, std::shared_ptr<TextImageV2> textImage)
{
  return flatDictionary<std::string, std::string, ImageType>(
      depth, std::make_tuple("hash", textImage->hash()),
      std::make_tuple("text", textImage->text()),
      std::make_tuple("type", textImage->type()));
}

std::variant<Json, PBDev::Error>
flatCollageImage(int depth, std::shared_ptr<CollageImage> collageImage)
{
  return flatDictionary<std::string, std::vector<Path>, ImageType>(
      depth, std::make_tuple("hash", collageImage->hash()),
      std::make_tuple("images", collageImage->sources()),
      std::make_tuple("type", collageImage->type()));
}

template <>
std::variant<Json, PBDev::Error> flatSimple(int             depth,
                                            GenericImagePtr genericImage)
{
  if (genericImage->type() == ImageType::Regular) {
    return flatRegularImage(
        depth, std::static_pointer_cast<RegularImageV2>(genericImage));
  }
  else if (genericImage->type() == ImageType::Text) {
    return flatTextImage(depth,
                         std::static_pointer_cast<TextImageV2>(genericImage));
  }
  else if (genericImage->type() == ImageType::Collage) {
    return flatCollageImage(
        depth, std::static_pointer_cast<CollageImage>(genericImage));
  }
  else {
    PBDev::basicAssert(false);
  }
  return PBDev::Error() << "Unknown image type";
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

template <>
std::variant<Json, PBDev::Error> flatSimple(int depth, Project project)
{
  return flatDictionary<std::string, PaperSettings, GenericImagePtrMatrix,
                        GenericImagePtrLine>(
      depth, std::make_tuple("name", project.name),
      std::make_tuple("paperSettings", project.paperSettings),
      std::make_tuple("imageMonitor", project.imageMonitor().unstaged()),
      std::make_tuple("stagedImages", project.stagedImages().stagedPhotos()));
}

} // namespace PB
