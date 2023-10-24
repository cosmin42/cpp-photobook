#include <pb/Enums.h>

#include <type_traits>

namespace PB {

ThumbnailType operator|(ThumbnailType lhs, ThumbnailType rhs)
{
  return static_cast<ThumbnailType>(
      static_cast<std::underlying_type<ThumbnailType>::type>(lhs) |
      static_cast<std::underlying_type<ThumbnailType>::type>(rhs));
}

ThumbnailType operator&(ThumbnailType lhs, ThumbnailType rhs)
{
  return static_cast<ThumbnailType>(
      static_cast<std::underlying_type<ThumbnailType>::type>(lhs) &
      static_cast<std::underlying_type<ThumbnailType>::type>(rhs));
}

} // namespace PB