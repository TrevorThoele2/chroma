#pragma once

#include <string>

namespace Chroma
{
    class FilePath;

    class FileName
    {
    public:
        FileName() = default;
        FileName(const std::string& string);
        // Must be null-terminated
        FileName(const char* buf);
        FileName(const FilePath& path);
        FileName(const FileName& arg);
        FileName(FileName&& arg);
        FileName& operator=(const FileName& arg);
        FileName& operator=(FileName&& arg);

        bool operator==(const FileName& arg) const;
        bool operator!=(const FileName& arg) const;
        operator const std::string&();
        operator const char*();
        const char* c_str() const;

        FileName& Set(const std::string& set);
        FileName& SetExtension(const std::string& replace);

        operator const std::string&() const;
        const std::string& GetValue() const;
        const std::string& GetExtension() const;
        std::string GetWithoutExtension() const;
        bool IsEmpty() const;
    private:
        std::string value;
        std::string extension;
    };
}

namespace std
{
    template<>
    struct hash<::Chroma::FileName>
    {
        typedef ::Chroma::FileName argument_type;
        typedef std::size_t result_type;

        result_type operator()(const argument_type& arg) const
        {
            const result_type first(std::hash<string>()(arg.GetValue()));
            return first;
        }
    };
}