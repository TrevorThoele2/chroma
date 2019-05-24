#pragma once

#include <string>

namespace Chroma
{
    class FilePath;
    class FileName;

    class RelativeFilePath
    {
    public:
        static std::string fileSeparator;
    public:
        RelativeFilePath();
        RelativeFilePath(const std::string& moveDown);
        // Must be null-terminated
        RelativeFilePath(const char* moveDown);
        RelativeFilePath(size_t moveUp, const std::string& moveDown);
        RelativeFilePath(const FilePath& from, const FilePath& to);
        RelativeFilePath(const RelativeFilePath& arg);
        RelativeFilePath(RelativeFilePath&& arg);
        RelativeFilePath& operator=(const RelativeFilePath& arg);
        RelativeFilePath& operator=(RelativeFilePath&& arg);

        bool operator==(const RelativeFilePath& arg) const;
        bool operator!=(const RelativeFilePath& arg) const;
        operator const char*();
        const char* c_str() const;

        RelativeFilePath& SetFileName(const FileName& name);
        RelativeFilePath& SetExtension(const std::string& replace);
        RelativeFilePath& RemoveFileName();
        RelativeFilePath& RemoveExtension();

        RelativeFilePath& Up(size_t number = 1);
        RelativeFilePath& Down(const std::string& down);
        size_t GetMoveUpCount() const;
        std::string GetMoveDown() const;

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
        std::string moveDown;
        size_t moveUp;
        std::string extension;
    };
}

namespace std
{
    template<>
    struct hash<::Chroma::RelativeFilePath>
    {
        typedef ::Chroma::RelativeFilePath argument_type;
        typedef std::size_t result_type;

        result_type operator()(const argument_type& arg) const
        {
            const result_type first(std::hash<string>()(arg.GetValue()));
            return first;
        }
    };
}