#pragma once

#include <string>

namespace PB
{
struct AspectRatio
{
  unsigned widthRatio;
  unsigned heightRatio;

  unsigned getWidth(unsigned height) const
  {
	return (widthRatio * height) / heightRatio;
  }

  unsigned getHeight(unsigned width) const
  {
	  return (heightRatio * width) / widthRatio;
  }

  std::string operator()() const
  {
	  return std::to_string(widthRatio) + "to" + std::to_string(heightRatio);
  }
};
}