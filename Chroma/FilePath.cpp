#include "FilePath.h"

#include "FileName.h"
#include "RelativeFilePath.h"

#include "StringUtility.h"

namespace Chroma
{
    std::string FilePath::fileSeparator = "";

    FilePath::FilePath(const std::string& string) : value(string), extension(GetFileExtension(string))
    {}

    FilePath::FilePath(const char* buf) : value(buf), extension(GetFileExtension(buf))
    {}

    FilePath::FilePath(const FilePath& arg, const RelativeFilePath& relative) : value(arg.value)
    {
        Append(relative);
    }

    FilePath::FilePath(const FilePath& arg) : value(arg.value), extension(arg.extension)
    {}

    FilePath::FilePath(FilePath&& arg) : value(std::move(arg.value)), extension(std::move(arg.extension))
    {}

    FilePath& FilePath::operator=(const FilePath& arg)
    {
        value = arg.value;
        extension = arg.extension;
        return *this;
    }

    FilePath& FilePath::operator=(FilePath&& arg)
    {
        value = std::move(arg.value);
        extension = std::move(arg.extension);
        return *this;
    }

    bool FilePath::operator==(const FilePath& arg) const
    {
        return value == arg.value;
    }

    bool FilePath::operator!=(const FilePath& arg) const
    {
        return !(*this == arg);
    }

    FilePath::operator const char*()
    {
        return value.c_str();
    }

    const char* FilePath::c_str() const
    {
        return value.c_str();
    }

    FilePath& FilePath::Set(const std::string& set)
    {
        value = set;
        extension = GetFileExtension(value);

        return *this;
    }

    FilePath& FilePath::SetFileName(const FileName& name)
    {
        auto found = value.find_last_of(fileSeparator);
        if (found == value.npos)
        {
            value = name;
            extension = GetFileExtension(value);
            return *this;
        }

        value.replace(found + 1, value.npos, name);
        extension = GetFileExtension(value);

        return *this;
    }

    FilePath& FilePath::SetExtension(const std::string& replace)
    {
        value = ReplaceFileExtension(value, replace);
        extension = replace;

        return *this;
    }

    FilePath& FilePath::RemoveFileName()
    {
        value = ::Chroma::RemoveFileName(value);
        extension = "";

        return *this;
    }

    FilePath& FilePath::RemoveExtension()
    {
        value = RemoveFileExtension(value);
        extension = "";

        return *this;
    }

    FilePath& FilePath::RemoveDirectory(size_t index)
    {
        if (index >= GetDirectoryCount())
            return *this;

        std::string directoryName = GetDirectoryName(index);
        value.erase(value.find(directoryName), directoryName.size());

        return *this;
    }

    FilePath& FilePath::Append(const std::string& append)
    {
        value.append(append);
        extension = GetFileExtension(value);
        return *this;
    }

    FilePath& FilePath::Append(const FileName& append)
    {
        return Append(append.GetValue());
    }

    FilePath& FilePath::Append(const RelativeFilePath& append)
    {
        RemoveFileName();
        size_t directoryIndex = GetDirectoryCount() - 1;
        for (size_t loop = 0; loop < append.GetMoveUpCount(); ++loop, --directoryIndex)
            RemoveDirectory(directoryIndex);

        Append(append.GetMoveDown());
        return *this;
    }

    FilePath& FilePath::Append(const char* append)
    {
        return Append(std::string(append));
    }

    FilePath& FilePath::AppendSeparator()
    {
        Append(fileSeparator);
        return *this;
    }

    FilePath& FilePath::ReplaceAllWith(const std::string& replace, const std::string& with)
    {
        for (size_t index = 0; index < value.length() - replace.length(); ++index)
            if (value.substr(index, replace.length()) == replace)
                value.replace(index, replace.length(), with);

        return *this;
    }

    bool FilePath::HasDirectory(const std::string& directoryName)
    {
        size_t curPos = 0;
        // Retrieve folders from front to back until folderName is found or not
        std::string slice;
        while (true)
        {
            auto found = value.find(fileSeparator, curPos);
            if (found == value.npos)
                return false;

            slice = value.substr(curPos, found);
            if (slice == directoryName)
                return true;

            curPos = found;
        }

        return false;
    }

    size_t FilePath::GetDirectoryCount() const
    {
        size_t count = 0;
        if (value.find_first_of(fileSeparator) != 0)
            ++count;

        size_t position = value.find(fileSeparator);
        while (position != value.npos)
        {
            ++count;
            position = value.find(fileSeparator, position + 1);
        }

        return count;
    }

    std::string FilePath::GetDirectoryName(size_t index) const
    {
        if (index >= GetDirectoryCount())
            return std::string();

        size_t positionStart = 0;
        size_t positionEnd = value.find(fileSeparator, positionStart + 1);
        for (size_t loop = 0; loop < index; ++loop)
        {
            positionStart = positionEnd;
            positionEnd = value.find(fileSeparator, positionEnd + 1);
        }

        return value.substr(positionStart, positionEnd - positionStart);
    }

    FilePath::operator std::string() const
    {
        return value;
    }

    std::string FilePath::GetValue() const
    {
        return value;
    }

    std::string FilePath::GetExtension() const
    {
        return extension;
    }

    std::string FilePath::GetWithoutExtension() const
    {
        return RemoveFileExtension(value);
    }

    bool FilePath::IsEmpty() const
    {
        return value == "";
    }

    FileName FilePath::GetFileName() const
    {
        auto found = value.find_last_of(fileSeparator);
        if (found == value.npos)
            return FileName(value);

        return FileName(value.substr(found + 1));
    }
}