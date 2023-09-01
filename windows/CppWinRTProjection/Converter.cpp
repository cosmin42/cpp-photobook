#include <Converter.h>

#include <string>

namespace NativePB {

template <> auto Converter::operator()(const winrt::hstring nativeObject) {
  std::string result = winrt::to_string(nativeObject);
  return result;
}
} // namespace NativePB
