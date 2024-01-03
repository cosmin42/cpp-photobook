#pragma once

#include <boost/bimap/bimap.hpp>

#include <pb/util/Traits.h>

namespace PB {
class PathCache final{
public:
  static bool valid(Path path, std::string hash);

  void newHash(Path path);
  void load(Path path, std::string hash);
  void remove(Path path);
  void remove(std::string hash);

  std::string hash(Path path) const;
  Path        path(std::string jash) const;

  bool contains(Path path) const;
  bool contains(std::string hash) const;

  boost::bimaps::bimap<Path, std::string> data() const;

private:
  boost::bimaps::bimap<Path, std::string> mEntries;
};
} // namespace PB