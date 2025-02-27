#pragma once

#include <variant>

#include <magic_enum/magic_enum.hpp>

#include <boost/uuid/uuid_io.hpp>

#include <pb/entities/CollageImage.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/PaperSettings.h>
#include <pb/entities/RegularImageV2.h>
#include <pb/entities/TextImageV2.h>
#include <pb/components/Project.h>

void to_json(Json &json, const boost::uuids::uuid &uuid);

void from_json(const Json &json, boost::uuids::uuid &uuid);

void to_json(Json &json, const PB::ImageType &imageType);

void from_json(const Json &json, PB::ImageType &imageType);

void to_json(Json &json, const PB::PaperType &paperType);

void from_json(const Json &json, PB::PaperType &paperType);

void to_json(Json &json, const PB::PaperSettings &paper);

void from_json(const Json &json, PB::PaperSettings &paper);

void to_json(Json &json, const PB::RegularImageV2 &regularImage);

void from_json(const Json &json, PB::RegularImageV2 &regularImage);

void to_json(Json &json, const PB::TextImageV2 &textImage);

void from_json(const Json &json, PB::TextImageV2 &textImage);

void to_json(nlohmann::json &j, const std::shared_ptr<PB::RegularImageV2> &p);

void from_json(const nlohmann::json &j, std::shared_ptr<PB::RegularImageV2> &p);

void to_json(nlohmann::json &j, const std::shared_ptr<PB::TextImageV2> &p);

void from_json(const nlohmann::json &j, std::shared_ptr<PB::TextImageV2> &p);

void to_json(Json &json, const PB::GenericImagePtr &genericImage);

void from_json(const Json &json, PB::GenericImagePtr &genericImage);

void to_json(Json &json, const PB::GenericImagePtrMatrix &matrix);

void from_json(const Json &json, PB::GenericImagePtrMatrix &matrix);

void to_json(Json &json, const PB::GenericImagePtrLine &line);

void from_json(const Json &json, PB::GenericImagePtrLine &line);

void to_json(Json &json, PB::Project &project);

void from_json(const Json &json, PB::CollageImage &collageImage);

void to_json(Json &json, const PB::CollageImage &collageImage);

void to_json(nlohmann::json &j, const std::shared_ptr<PB::CollageImage> &p);

void from_json(const nlohmann::json &j, std::shared_ptr<PB::CollageImage> &p);

void to_json(Json &json, const PB::Project &project);

void from_json(const Json &json, PB::Project &project);

template <typename T, typename U>
void to_json(Json &json, const boost::bimaps::bimap<T, U> &bimap)
{
  json = Json::object();
  for (const auto &pair : bimap) {
    json[pair.left] = pair.right;
  }
}

template <typename T, typename U>
void from_json(const Json &json, boost::bimaps::bimap<T, U> &bimap)
{
  bimap.clear();
  for (const auto &pair : json.items()) {
    bimap.insert({pair.key(), pair.value()});
  }
}
