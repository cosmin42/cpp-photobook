#include <pb/infra/FileSupport.h>

#include <fstream>

namespace PB::infra {
std::shared_ptr<cv::Mat> loadImageToCvMat(Path const path)
{
  auto image = cv::imread(path.string(), cv::IMREAD_COLOR);

  if (image.empty()) {
    return nullptr;
  }

  std::shared_ptr<cv::Mat> inputImage = std::make_shared<cv::Mat>(image);

  if (inputImage == nullptr) {
    return nullptr;
  }

  std::vector<cv::Mat> matChannels;

  cv::split(*inputImage, matChannels);

  if (matChannels.size() == 3) {
    cv::Mat alpha(inputImage->rows, inputImage->cols, CV_8UC1);
    alpha = cv::Scalar(255);
    matChannels.push_back(alpha);
    cv::merge(matChannels, *inputImage);
  }
  else if (matChannels.size() != 4) {
    return nullptr;
  }

  return inputImage;
}

// TODO: improve this function
std::vector<cv::Vec3f> readLutData(Path lutPath)
{
  std::ifstream file(lutPath);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open .cube file");
  }

  std::string            line;
  std::vector<cv::Vec3f> lut;

  struct Counter3d {
    unsigned i = 0;
    unsigned j = 0;
    unsigned k = 0;
    unsigned size = 0;

    Counter3d &operator++()
    {
      if (k < size - 1) {
        ++k;
      }
      else if (j < size - 1) {
        ++j;
        k = 0;
      }
      else if (i < size - 1) {
        ++i;
        j = 0;
        k = 0;
      }
      else {
        PBDev::basicAssert(false);
      }
      return *this;
    }
  };

  Counter3d counter3d;

  // Skip header and comments
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Read LUT entries
    std::istringstream iss(line);

    if (line.find("LUT_3D_SIZE") != std::string::npos) {
      std::string cubeRtStr;
      iss >> cubeRtStr;
      iss >> cubeRtStr;

      try {
        counter3d.size = std::stoi(cubeRtStr) + 1;
      }
      catch (...) {
        PBDev::basicAssert(false);
      }
      continue;
    }
    if (line.find("DOMAIN_MIN") != std::string::npos) {
      continue;
    }
    if (line.find("DOMAIN_MAX") != std::string::npos) {
      continue;
    }
    if (line.find("TITLE") != std::string::npos) {
      continue;
    }

    PBDev::basicAssert(counter3d.size > 0);

    cv::Vec3f entry;
    if (iss >> entry[0] >> entry[1] >> entry[2]) {
      lut.push_back(entry);
      ++counter3d;
    }
    else {
      PBDev::basicAssert(false);
    }
  }
  file.close();
  return lut;
}

bool isValidImage(Path const path)
{
  auto image = cv::imread(path.string(), cv::IMREAD_COLOR);
  return !image.empty();
}

std::shared_ptr<cv::Mat>
loadImageToCvMatToFixedSize(Path const path, cv::Size size,
                            Geometry::ScalePolicy scalePolicy,
                            Geometry::OverlapType scaleType)
{
  auto image = loadImageToCvMat(path);
  if (image == nullptr) {
    return nullptr;
  }
  auto newSize = Geometry::resizeBox({image->cols, image->rows}, size,
                                     scaleType, scalePolicy);
  return Process::resize(image, newSize, false);
}

} // namespace PB::infra