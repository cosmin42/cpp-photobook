#pragma once

#include <map>
#include <string>

namespace PB
{

class MetadataReader final
{
public:
    static MetadataReader& instance()
    {
        return metadataReader;
    }

    auto metadata([[maybe_unused]] const std::string& absoluteFilePath) const -> std::map<std::string, std::string> const;

    auto metadata([[maybe_unused]] const std::string&& absoluteFilePath) const -> std::map<std::string, std::string> const;

private:
    static MetadataReader metadataReader;

    MetadataReader() = default;
    ~MetadataReader() = default;
    MetadataReader(const MetadataReader&) = delete;
    MetadataReader& operator= (const MetadataReader) = delete;
};

}