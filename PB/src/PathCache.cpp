#include <pb/project/PathCache.h>

namespace PB {

bool PathCache::valid(Path path, std::string hash)
{
  return std::filesystem::exists(path);
}

void PathCache::newHash(Path path)
{
  std::string hash_s = std::to_string(std::hash<std::string>{}(path.string()));
  mEntries.insert({path, hash_s});
}

void PathCache::load(Path path, std::string hash)
{
  mEntries.insert({path, hash});
}

void PathCache::remove(Path path) { mEntries.erase(mEntries.left.find(path)); }

void PathCache::remove(std::string hash)
{
  mEntries.erase(mEntries.right.find(hash));
}

std::string PathCache::hash(Path path) const { return mEntries.left.at(path); }

Path PathCache::path(std::string hash) const { return mEntries.right.at(hash); }

bool PathCache::contains(Path path) const
{
  return mEntries.left.find(path) != mEntries.left.end();
}

bool PathCache::contains(std::string hash) const
{
  return mEntries.right.find(path) != mEntries.right.end();
}
} // namespace PB