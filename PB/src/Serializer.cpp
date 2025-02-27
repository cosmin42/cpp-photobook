#include <pb/components/Serializer.h>

#include <magic_enum/magic_enum.hpp>

#include <boost/uuid/uuid_io.hpp>

#include <pb/entities/CollageImage.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/PaperSettings.h>
#include <pb/entities/RegularImageV2.h>
#include <pb/entities/TextImageV2.h>
#include <pb/project/Project.h>

void to_json(Json &json, const boost::uuids::uuid &uuid)
{
  json = boost::uuids::to_string(uuid);
}

void from_json(const Json &json, boost::uuids::uuid &uuid)
{
  uuid = boost::uuids::string_generator()(json.get<std::string>());
}

void to_json(Json &json, const PB::ImageType &imageType)
{
  json = magic_enum::enum_name(imageType);
}

void from_json(const Json &json, PB::ImageType &imageType)
{
  imageType =
      magic_enum::enum_cast<PB::ImageType>(json.get<std::string>()).value();
}

void to_json(Json &json, const PB::PaperType &paperType)
{
  json = magic_enum::enum_name(paperType);
}

void from_json(const Json &json, PB::PaperType &paperType)
{
  paperType =
      magic_enum::enum_cast<PB::PaperType>(json.get<std::string>()).value();
}

void to_json(Json &json, const PB::PaperSettings &paper)
{
  json["type"] = paper.type;
  json["ppi"] = paper.ppi;
  json["width"] = paper.width;
  json["height"] = paper.height;
}

void from_json(const Json &json, PB::PaperSettings &paper)
{
  paper.type = json["type"].get<PB::PaperType>();
  paper.ppi = json["ppi"].get<int>();
  paper.width = json["width"].get<int>();
  paper.height = json["height"].get<int>();
}

void to_json(Json &json, const PB::RegularImageV2 &regularImage)
{
  json["hash"] = regularImage.hash();
  json["original"] = regularImage.original();
  json["type"] = regularImage.type();
}

void from_json(const Json &json, PB::RegularImageV2 &regularImage)
{
  auto        hash = json["hash"].get<std::string>();
  PBDev::Path original = json["original"].get<PBDev::Path>();
  regularImage = PB::RegularImageV2(hash, original);
}

void to_json(Json &json, const PB::TextImageV2 &textImage)
{
  json["hash"] = textImage.hash();
  json["text"] = textImage.text();
  json["type"] = textImage.type();
}

void from_json(const Json &json, PB::TextImageV2 &textImage)
{
  auto        hash = json["hash"].get<std::string>();
  std::string text = json["text"].get<std::string>();
  textImage = PB::TextImageV2(hash, text);
}

void to_json(nlohmann::json &j, const std::shared_ptr<PB::RegularImageV2> &p)
{
  if (p) {
    j = nlohmann::json{};
    to_json(j, *p);
  }
  else {
    j = nullptr;
  }
}

void from_json(const nlohmann::json &j, std::shared_ptr<PB::RegularImageV2> &p)
{
  if (!j.is_null()) {
    p = std::make_shared<PB::RegularImageV2>("", PBDev::Path());
    from_json(j, *p);
  }
  else {
    p.reset();
  }
}

void to_json(nlohmann::json &j, const std::shared_ptr<PB::TextImageV2> &p)
{
  if (p) {
    j = nlohmann::json{};
    to_json(j, *p);
  }
  else {
    j = nullptr;
  }
}

void from_json(const nlohmann::json &j, std::shared_ptr<PB::TextImageV2> &p)
{
  if (!j.is_null()) {
    p = std::make_shared<PB::TextImageV2>("", "");
    from_json(j, *p);
  }
  else {
    p.reset();
  }
}

void to_json(Json &json, const PB::GenericImagePtr &genericImage)
{
  if (genericImage->type() == PB::ImageType::Regular) {
    to_json(json, std::static_pointer_cast<PB::RegularImageV2>(genericImage));
  }
  else if (genericImage->type() == PB::ImageType::Text) {
    to_json(json, std::static_pointer_cast<PB::TextImageV2>(genericImage));
  }
  else {
    PBDev::basicAssert(false);
  }
}

void from_json(const Json &json, PB::GenericImagePtr &genericImage)
{
  if (json["type"].get<PB::ImageType>() == PB::ImageType::Regular) {
    auto regularImage = std::make_shared<PB::RegularImageV2>("", PBDev::Path());
    from_json(json, *regularImage);
    genericImage = regularImage;
  }
  else if (json["type"].get<PB::ImageType>() == PB::ImageType::Text) {
    auto textImage = std::make_shared<PB::TextImageV2>("", "");
    from_json(json, *textImage);
    genericImage = textImage;
  }
  else {
    PBDev::basicAssert(false);
  }
}

// PB::GenericImageMatrix
void to_json(Json &json, const PB::GenericImagePtrMatrix &matrix)
{
  json = Json::array();
  for (auto &line : matrix) {
    Json lineJson = Json::array();
    for (auto &image : line) {
      Json imageJson;
      to_json(imageJson, image);
      lineJson.push_back(imageJson);
    }
    json.push_back(lineJson);
  }
}

void from_json(const Json &json, PB::GenericImagePtrMatrix &matrix)
{
  matrix.clear();
  for (auto &line : json) {
    PB::GenericImagePtrLine lineVector;
    for (auto &image : line) {
      PB::GenericImagePtr genericImage;
      from_json(image, genericImage);
      lineVector.push_back(genericImage);
    }
    matrix.push_back(lineVector);
  }
}

void to_json(Json &json, const PB::GenericImagePtrLine &line)
{
  json = Json::array();
  for (auto &image : line) {
    Json imageJson;
    to_json(imageJson, image);
    json.push_back(imageJson);
  }
}

void from_json(const Json &json, PB::GenericImagePtrLine &line)
{
  line.clear();
  for (auto &image : json) {
    PB::GenericImagePtr genericImage;
    from_json(image, genericImage);
    line.push_back(genericImage);
  }
}

void to_json(Json &json, PB::Project &project)
{
  json["name"] = project.name;
  json["paperSettings"] = project.paperSettings;
  to_json(json["imageMonitor"], project.imageMonitor()->unstaged());
  to_json(json["stagedImages"], project.stagedImages()->stagedPhotos());
  to_json(json["roots"], project.imageMonitor()->rowList());
}

void from_json(const Json &json, PB::Project &project)
{
  project.name = json["name"].get<std::string>();
  from_json(json["paperSettings"], project.paperSettings);

  PB::GenericImagePtrMatrix unstaged;
  PB::GenericImagePtrLine   staged;
  std::vector<PBDev::Path>  roots;

  from_json(json["imageMonitor"], unstaged);
  from_json(json["stagedImages"], staged);
  from_json(json["roots"], roots);

  for (auto i = 0; i < roots.size(); ++i) {
    project.imageMonitor()->addRow(roots[i], unstaged[i]);
  }

  project.stagedImages()->addPictures(staged);
}

void from_json(const Json &json, PB::CollageImage &collageImage)
{
  auto hash = json["hash"].get<std::string>();
  auto sources = json["images"].get<std::vector<PBDev::Path>>();
  collageImage = PB::CollageImage(hash, sources);
}

void to_json(Json &json, const PB::CollageImage &collageImage)
{
  json["hash"] = collageImage.hash();
  json["images"] = collageImage.sources();
  json["type"] = collageImage.type();
}

void to_json(nlohmann::json &j, const std::shared_ptr<PB::CollageImage> &p)
{
  if (p) {
    j = nlohmann::json{};
    to_json(j, *p);
  }
  else {
    j = nullptr;
  }
}

void from_json(const nlohmann::json &j, std::shared_ptr<PB::CollageImage> &p)
{
  if (!j.is_null()) {
    p = std::make_shared<PB::CollageImage>("", std::vector<PBDev::Path>{});
    from_json(j, *p);
  }
  else {
    p.reset();
  }
}
