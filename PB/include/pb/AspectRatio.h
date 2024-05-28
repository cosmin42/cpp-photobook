#pragma once

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
};
}
