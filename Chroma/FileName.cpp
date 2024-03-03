#include "FileName.h"

#include "FilePath.h"

#include "StringUtility.h"

namespace Chroma
{
    FileName::FileName(const std::string& string) : value(string), extension(GetFileExtension(string))
    {}

    FileName::FileName(const char* buf) : value(buf), extension(GetFileExtension(buf))
    {}

    FileName::FileName(const FilePath& path) : value(path.GetFileName().GetValue()), extension(path.GetExtension())
    {}

    FileName::FileName(const FileName& arg) : value(arg.value), extension(arg.extension)
    {}

    FileName::FileName(FileName&& arg) : value(std::move(arg.value)), extension(std::move(arg.extension))
    {}

    FileName& FileName::operator=(const FileName& arg)
    {
        value = arg.value;
        extension = arg.extension;
        return *this;
    }

    FileName& FileName::operator=(FileName&& arg)
    {
        value = std::move(arg.value);
        extension = std::move(arg.extension);
        return *this;
    }

    bool FileName::operator==(const FileName& arg) const
    {
        return value == arg.value;
    }

    bool FileName::operator!=(const FileName& arg) const
    {
        return !(*this == arg);
    }

    FileName::operator const std::string&()
    {
        return value;
    }

    FileName::operator const char*()
    {
        return value.c_str();
    }

    const char* FileName::c_str() const
    {
        return value.c_str();
    }

    FileName& FileName::Set(const std::string& set)
    {
        value = set;
        extension = GetFileExtension(value);

        return *this;
    }

    FileName& FileName::SetExtension(const std::string& replace)
    {
        value = ReplaceFileExtension(value, replace);
        extension = replace;

        return *this;
    }

    FileName::operator const std::string&() const
    {
        return value;
    }

    const std::string& FileName::GetValue() const
    {
        return value;
    }

    const std::string& FileName::GetExtension() const
    {
        return extension;
    }

    std::string FileName::GetWithoutExtension() const
    {
        return RemoveFileExtension(value);
    }

    bool FileName::IsEmpty() const
    {
        return value == "";
    }
}