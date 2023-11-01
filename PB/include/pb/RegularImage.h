#pragma once

#include <pb/VirtualImage.h>

namespace PB {
	class RegularImage final : public VirtualImage {
public:
  cv::Mat image() override;
  Path    path() override;
};
} // namespace PB