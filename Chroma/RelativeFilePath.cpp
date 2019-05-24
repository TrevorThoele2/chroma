#include "RelativeFilePath.h"

#include "FilePath.h"
#include "FileName.h"

#include "StringUtility.h"

namespace Chroma
{
    std::string RelativeFilePath::fileSeparator = "";

    RelativeFilePath::RelativeFilePath() : moveUp(0)
    {}

    RelativeFilePath::RelativeFilePath(const std::string& moveDown) :
        moveDown(moveDown), extension(GetFileExtension(moveDown)), moveUp(0)
    {}

    RelativeFilePath::RelativeFilePath(const char* moveDown) :
        moveDown(moveDown), extension(GetFileExtension(moveDown)), moveUp(0)
    {}

    RelativeFilePath::RelativeFilePath(size_t moveUp, const std::string& moveDown) :
        moveUp(moveUp), moveDown(moveDown), extension(GetFileExtension(moveDown))
    {}

    RelativeFilePath::RelativeFilePath(const FilePath& from, const FilePath& to) : moveUp(0)
    {
        bool foundDifference = false;
        std::string fromValue = from.GetValue();
        std::string toValue = to.GetValue();
        for (size_t loop = 0; loop < from.GetDirectoryCount(); ++loop)
        {
            std::string fromDirectoryName = from.GetDirectoryName(loop);
            if (fromDirectoryName == to.GetDirectoryName(loop))
                continue;

            toValue = toValue.substr(toValue.find(fromDirectoryName));
            foundDifference = true;
            break;
        }

        if (foundDifference)
        {
            FilePath newFilePath(toValue);
            moveUp = newFilePath.GetDirectoryCount() - 1;
            moveDown = newFilePath.GetValue();
        }
        else
            moveDown = to.GetValue().substr(fromValue.size());

        extension = GetFileExtension(moveDown);
    }

    RelativeFilePath::RelativeFilePath(const RelativeFilePath& arg) :
        moveDown(arg.moveDown), moveUp(arg.moveUp), extension(arg.extension)
    {}

    RelativeFilePath::RelativeFilePath(RelativeFilePath&& arg) :
        moveDown(std::move(arg.moveDown)), moveUp(std::move(arg.moveUp)), extension(std::move(arg.extension))
    {}

    RelativeFilePath& RelativeFilePath::operator=(const RelativeFilePath& arg)
    {
        moveDown = arg.moveDown;
        moveUp = arg.moveUp;
        extension = arg.extension;
        return *this;
    }

    RelativeFilePath& RelativeFilePath::operator=(RelativeFilePath&& arg)
    {
        moveDown = std::move(arg.moveDown);
        moveUp = std::move(arg.moveUp);
        extension = std::move(arg.extension);
        return *this;
    }

    bool RelativeFilePath::operator==(const RelativeFilePath& arg) const
    {
        return moveDown == arg.moveDown && moveUp == arg.moveUp;
    }

    bool RelativeFilePath::operator!=(const RelativeFilePath& arg) const
    {
        return !(*this == arg);
    }

    RelativeFilePath::operator const char*()
    {
        return GetValue().c_str();
    }

    const char* RelativeFilePath::c_str() const
    {
        return GetValue().c_str();
    }

    RelativeFilePath& RelativeFilePath::SetFileName(const FileName& name)
    {
        auto found = moveDown.find_last_of(fileSeparator);
        if (found == moveDown.npos)
        {
            moveDown = name;
            extension = GetFileExtension(moveDown);
            return *this;
        }

        moveDown.replace(found + 1, moveDown.npos, name);
        extension = GetFileExtension(moveDown);

        return *this;
    }

    RelativeFilePath& RelativeFilePath::SetExtension(const std::string& replace)
    {
        moveDown = ReplaceFileExtension(moveDown, replace);
        extension = replace;

        return *this;
    }

    RelativeFilePath& RelativeFilePath::RemoveFileName()
    {
        moveDown = ::Chroma::RemoveFileName(moveDown);
        extension = "";

        return *this;
    }

    RelativeFilePath& RelativeFilePath::RemoveExtension()
    {
        moveDown = RemoveFileExtension(moveDown);
        extension = "";

        return *this;
    }

    RelativeFilePath& RelativeFilePath::Up(size_t number)
    {
        moveUp += number;
        return *this;
    }

    RelativeFilePath& RelativeFilePath::Down(const std::string& down)
    {
        moveDown += down;
        return *this;
    }

    size_t RelativeFilePath::GetMoveUpCount() const
    {
        return moveUp;
    }

    std::string RelativeFilePath::GetMoveDown() const
    {
        return moveDown;
    }

    bool RelativeFilePath::HasDirectory(const std::string& directoryName)
    {
        size_t curPos = 0;
        // Retrieve folders from front to back until folderName is found or not
        std::string slice;
        while (true)
        {
            auto found = moveDown.find(fileSeparator, curPos);
            if (found == moveDown.npos)
                return false;

            slice = moveDown.substr(curPos, found);
            if (slice == directoryName)
                return true;

            curPos = found;
        }

        return false;
    }

    size_t RelativeFilePath::GetDirectoryCount() const
    {
        size_t count = 0;
        if (moveDown.find_first_of(fileSeparator) != 0)
            ++count;

        size_t position = moveDown.find(fileSeparator);
        while (position != moveDown.npos)
        {
            ++count;
            position = moveDown.find(fileSeparator, position + 1);
        }

        return count;
    }

    std::string RelativeFilePath::GetDirectoryName(size_t index) const
    {
        if (index >= GetDirectoryCount())
            return std::string();

        size_t positionStart = 0;
        size_t positionEnd = moveDown.find(fileSeparator, positionStart + 1);
        for (size_t loop = 0; loop < index; ++loop)
        {
            positionStart = positionEnd;
            positionEnd = moveDown.find(fileSeparator, positionEnd + 1);
        }

        return moveDown.substr(positionStart, positionEnd - positionStart);
    }

    RelativeFilePath::operator std::string() const
    {
        return GetValue();
    }

    std::string RelativeFilePath::GetValue() const
    {
        std::string ret;
        for (size_t loop = 0; loop < moveUp; ++loop)
            ret += "..\\";
        ret += moveDown;
        return ret;
    }

    std::string RelativeFilePath::GetExtension() const
    {
        return extension;
    }

    std::string RelativeFilePath::GetWithoutExtension() const
    {
        return RemoveFileExtension(moveDown);
    }

    bool RelativeFilePath::IsEmpty() const
    {
        return moveDown == "" && moveUp == 0;
    }

    FileName RelativeFilePath::GetFileName() const
    {
        auto found = moveDown.find_last_of(fileSeparator);
        if (found == moveDown.npos)
            return FileName(moveDown);

        return FileName(moveDown.substr(found + 1));
    }
}