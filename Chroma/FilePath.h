#pragma once

#include <string>

namespace Chroma
{
    class FileName;
    class RelativeFilePath;

    class FilePath
    {
    public:
        static std::string fileSeparator;
    public:
        FilePath() = default;
        FilePath(const std::string& string);
        // Must be null-terminated
        FilePath(const char* buf);
        FilePath(const FilePath& arg, const RelativeFilePath& relative);
        FilePath(const FilePath& arg);
        FilePath(FilePath&& arg);
        FilePath& operator=(const FilePath& arg);
        FilePath& operator=(FilePath&& arg);

        bool operator==(const FilePath& arg) const;
        bool operator!=(const FilePath& arg) const;
        operator const char*();
        const char* c_str() const;

        FilePath& Set(const std::string& set);
        FilePath& SetFileName(const FileName& name);
        FilePath& SetExtension(const std::string& replace);
        FilePath& RemoveFileName();
        FilePath& RemoveExtension();
        FilePath& RemoveDirectory(size_t index);

        FilePath& Append(const std::string& append);
        FilePath& Append(const FileName& append);
        FilePath& Append(const RelativeFilePath& append);
        FilePath& Append(const char* append);
        FilePath& AppendSeparator();
        FilePath& ReplaceAllWith(const std::string& replace, const std::string& with);

        bool HasDirectory(const std::string& directoryName);
        size_t GetDirectoryCount() const;
        std::string GetDirectoryName(size_t index) const;

        operator std::string() const;
        std::string GetValue() const;
        std::string GetExtension() const;
        std::string GetWithoutExtension() const;
        bool IsEmpty() const;
        FileName GetFileName() const;
    private:
        std::string value;
        std::string extension;
    };
}

namespace std
{
    template<>
    struct hash<::Chroma::FilePath>
    {
        typedef ::Chroma::FilePath argument_type;
        typedef std::size_t result_type;

        result_type operator()(const argument_type& arg) const
        {
            const result_type first(std::hash<string>()(arg.GetValue()));
            return first;
        }
    };
}