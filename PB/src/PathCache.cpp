#include <pb/project/PathCache.h>

namespace PB {

PathCache::PathCache(boost::bimaps::bimap<Path, std::string> data)
{
  mEntries = data;
}

PathCache::PathCache(PathCache const &other)
{
  for (auto& it : other.mEntries) {
    mEntries.insert({it.left, it.right});
  }
}

bool PathCache::valid(Path path, std::string hash)
{
  return std::filesystem::exists(path);
}

void PathCache::newHash(Path path)
{
  std::string hash_s = std::to_string(std::hash<std::string>{}(path.string()));

  std::string suffix;
  int         salt = 0;
  while (contains(hash_s + suffix)) {
    PBDev::basicAssert(salt < std::numeric_limits<int>::max());
    suffix = std::to_string(salt);
    salt++;
  }
  hash_s = hash_s + suffix;
  mEntries.insert({path, hash_s});
}

void PathCache::load(Path path, std::string hash)
{
  mEntries.insert({path, hash});
}

void PathCache::remove(Path path) { mEntries.left.erase(path); }

void PathCache::remove(std::string hash) { mEntries.right.erase(hash); }

std::string PathCache::hash(Path path) const { return mEntries.left.at(path); }

Path PathCache::path(std::string hash) const { return mEntries.right.at(hash); }

bool PathCache::contains(Path path) const
{
  return mEntries.left.find(path) != mEntries.left.end();
}

bool PathCache::contains(std::string hash) const
{
  return mEntries.right.find(hash) != mEntries.right.end();
}

boost::bimaps::bimap<Path, std::string> PathCache::data() const
{
  return mEntries;
}
} // namespace PB