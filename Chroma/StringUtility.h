#pragma once

#include <string>
#include <sstream>

#include "TypeIdentity.h"

namespace Chroma
{
    std::string GetFileName(const std::string& filePath);
    std::string GetFileExtension(const std::string& filePath);
    std::string RemoveFileExtension(const std::string& string);
    std::string RemoveFileName(const std::string& string);
    std::string ReplaceFileExtension(const std::string& string, const std::string& extension);

    void SpliceString(std::string& in, const std::string& check, const std::string& replace);

    std::string Trim(const std::string& trim);
    bool IsAllWhitespace(const std::string& check);
    bool StartsWith(const std::string& check, const std::string& startsWith);

    template<class T>
    void SpliceString(std::string& in, const std::string& check, const T replace)
    {
        SpliceString(in, check, std::string(replace));
    }

    namespace detail
    {
        template<class T>
        T FromStringImpl(const std::string& arg, const TypeIdentity<T>& t)
        {
            ::std::istringstream stream(arg);

            T toReturn;
            stream >> toReturn;
            return toReturn;
        }

        std::string FromStringImpl(const std::string& arg, const TypeIdentity<std::string>& t);
    }

    template<class T, typename ::std::enable_if<!::std::is_enum<T>::value, int>::type = 0>
    T FromString(const std::string& arg)
    {
        return detail::FromStringImpl(arg, ::Chroma::Type<T>{});
    }

    template<class T, typename ::std::enable_if<::std::is_enum<T>::value, int>::type = 0>
    T FromString(const std::string& arg)
    {
        return static_cast<T>(FromString<::std::underlying_type<T>::type>(arg));
    }

    template<> char FromString(const std::string& arg);
    template<> signed char FromString(const std::string& arg);
    template<> unsigned char FromString(const std::string& arg);
    template<> short FromString(const std::string& arg);
    template<> unsigned short FromString(const std::string& arg);
    template<> bool FromString(const std::string& arg);

    template<class T>
    std::string ToString(T arg)
    {
        std::ostringstream s;
        s << arg;
        return std::string(s.str());
    }

    std::string ToString(char arg);
    std::string ToString(signed char arg);
    std::string ToString(unsigned char arg);
    std::string ToString(const std::string& arg);
    std::string ToString(bool arg);
}