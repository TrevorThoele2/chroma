#include "StringUtility.h"

#include "FilePath.h"

namespace Chroma
{
    std::string GetFileName(const std::string& filePath)
    {
        std::string newString = filePath;
        if (newString.find(FilePath::fileSeparator) != newString.npos)
        {
            // Slice the file path to just its file name
            auto pos = newString.find_last_of(FilePath::fileSeparator) + 1;
            newString = newString.substr(pos);
        }

        return newString;
    }

    std::string GetFileExtension(const std::string& filePath)
    {
        std::string newString = filePath;
        if (newString.find(".") != newString.npos)
        {
            // Slice the file path to just its file name
            auto pos = newString.find_last_of(".") + 1;
            return newString.substr(pos);
        }

        return newString;
    }

    std::string RemoveFileExtension(const std::string& string)
    {
        std::string newString = string;
        if (newString.find(".") != newString.npos)
        {
            // Slice the file extension off
            auto pos = newString.find_last_of(".");
            newString.erase(pos);
        }

        return newString;
    }

    std::string RemoveFileName(const std::string& string)
    {
        std::string newString = string;
        if (newString.find(".") != newString.npos)
        {
            // Slice the file extension off
            auto pos = newString.find_last_of(FilePath::fileSeparator);
            newString.erase(pos);
        }

        return newString;
    }

    std::string ReplaceFileExtension(const std::string& string, const std::string& extension)
    {
        std::string newString = string;
        if (newString.find(".") != newString.npos)
        {
            // Slice the file extension off
            auto pos = newString.find_last_of(".");
            newString.replace(pos + 1, std::string::npos, extension);
        }
        else
            newString.append('.' + extension);

        return newString;
    }

    void SpliceString(std::string& in, const std::string& check, const std::string& replace)
    {
        auto pos = in.find(check);

        while (pos != in.npos)
        {
            in.erase(pos, check.size());
            in.insert(pos, replace);

            pos = in.find(check);
        }
    }

    void Trim(std::string& trim)
    {
        while (trim[0] == ' ' || trim[0] == '\n')
            trim.erase(0, 1);

        while (trim[trim.size() - 1] == ' ' || trim[trim.size() - 1] == '\n')
            trim.erase(trim.size() - 1, 1);
    }

    std::string Trim(const std::string& trim)
    {
        std::string ret = trim;

        while (ret[ret.size() - 1] == ' ' || ret[ret.size() - 1] == '\n')
        {
            ret.erase(ret.size() - 1, 1);

            if (ret.empty())
                return ret;
        }

        while (ret[0] == ' ' || ret[0] == '\n')
            ret.erase(0, 1);

        return ret;
    }

    bool IsAllWhitespace(const std::string& check)
    {
        if (check.empty())
            return false;

        for (auto &c : check)
        {
            if (c != ' ' && c != '\n')
                return false;
        }

        return true;
    }

    bool StartsWith(const std::string& check, const std::string& startsWith)
    {
        return check.find_first_of(startsWith) == 0;
    }

    namespace detail
    {
        std::string FromStringImpl(const std::string& arg, const ::Chroma::Type<std::string>& t)
        {
            return arg;
        }
    }

    template<class T>
    T FromStringCommon(const std::string& arg)
    {
        long long returned = ::std::strtoll(arg.c_str(), nullptr, 10);
        if (returned > ::std::numeric_limits<T>::max())
            return ::std::numeric_limits<T>::max();
        else if (returned < ::std::numeric_limits<T>::min())
            return ::std::numeric_limits<T>::min();
        else
            return static_cast<T>(returned);
    }

    template<> char FromString(const std::string& arg)
    {
        return FromStringCommon<char>(arg);
    }

    template<> signed char FromString(const std::string& arg)
    {
        return FromStringCommon<signed char>(arg);
    }

    template<> unsigned char FromString(const std::string& arg)
    {
        return FromStringCommon<unsigned char>(arg);
    }

    template<> short FromString(const std::string& arg)
    {
        return FromStringCommon<short>(arg);
    }

    template<> unsigned short FromString(const std::string& arg)
    {
        return FromStringCommon<unsigned short>(arg);
    }

    template<> bool FromString(const std::string& arg)
    {
        if (arg == "true")
            return true;
        else
            return false;
    }

    template<class T>
    std::string ToStringCharCommon(T arg)
    {
        return ToString(short(arg));
    }

    std::string ToString(char arg)
    {
        return ToStringCharCommon(arg);
    }

    std::string ToString(signed char arg)
    {
        return ToStringCharCommon(arg);
    }

    std::string ToString(unsigned char arg)
    {
        return ToStringCharCommon(arg);
    }

    std::string ToString(const std::string& arg)
    {
        return arg;
    }

    std::string ToString(bool arg)
    {
        if (arg)
            return "true";
        else
            return "false";
    }
}