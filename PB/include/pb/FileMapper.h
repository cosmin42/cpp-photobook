#pragma once

#include <filesystem>
#include <vector>

namespace PB {

auto mapImages(std::filesystem::path const &) -> std::vector<std::filesystem::path>;


} // namespace PB