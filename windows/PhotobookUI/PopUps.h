#pragma once

#include <functional>
#include <variant>

#include <pb/Error.h>

namespace winrt::PhotobookUI::implementation {
class PopUps {
public:
  auto fireFolderPicker(HWND hWnd, std::function<void(std::string)> onSuccess)
      -> winrt::fire_and_forget;

  auto fireSaveFilePicker(
      HWND                                                      hWnd,
      std::function<void(std::variant<std::string, PB::Error>)> onReturn)
      -> winrt::fire_and_forget;
};
} // namespace winrt::PhotobookUI::implementation