#pragma once

#include <unordered_set>

#include <boost/bimap/bimap.hpp>

#include <pb/util/Traits.h>

namespace PB {

class PathCache final {
public:
  static bool valid(Path path, std::string hash);

  PathCache() = default;
  explicit PathCache(std::string projectName);
  PathCache(PathCache const &);
  ~PathCache() = default;

  void configure(boost::bimaps::bimap<Path, std::string> data);
  void configure(
      std::unordered_set<std::pair<Path, std::string>> pathProjectAssociation);

  void newHash(Path path);
  void load(Path path, std::string hash);
  void remove(Path path);
  void remove(std::string hash);

  std::string hash(Path path) const;
  Path        path(std::string jash) const;

  std::string hashCreateIfMissing(Path path);

  bool contains(Path path) const;
  bool contains(std::string hash) const;

  boost::bimaps::bimap<Path, std::string> data() const;

  bool operator==(PathCache const &other)
  {
    if (mEntries.size() != other.mEntries.size()) {
      return false;
    }
    for (auto &it : mEntries) {
      if (other.mEntries.left.find(it.left) == other.mEntries.left.end()) {
        return false;
      }
    }
    return true;
  }

private:
  std::string                             mProjectName;
  boost::bimaps::bimap<Path, std::string> mEntries;
  std::unordered_set<std::pair<Path, std::string>, PathProjectAssociationHash>
      mPathProjectAssociation;
};
} // namespace PB