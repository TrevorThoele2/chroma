#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <filesystem>

#include "TypeIdentity.h"

namespace Chroma
{
    std::string ToUppercase(const std::string& string);

    std::string ReplaceString(const std::string& string, const std::string& instance, const std::string& with);

    template<class String>
    [[nodiscard]] size_t CountInstances(const String& input, const String& of)
    {
        size_t instances = 0;
        const auto instanceSize = of.size();
        for (size_t i = 0; i < input.size(); ++i)
        {
            const auto substring = input.substr(i, instanceSize);
            if (substring == of)
                ++instances;
        }

        return instances;
    }

    [[nodiscard]] bool Contains(const std::string& input, const std::string& of);

    void SpliceString(std::string& in, const std::string& check, const std::string& replace);

    std::string Trim(const std::string& trim);
    bool IsAllWhitespace(const std::string& check);
    bool StartsWith(const std::string& check, const std::string& startsWith);
    bool EndsWith(const std::string& check, const std::string& endsWith);
    template<class T>
    std::string Join(std::string joiner, T begin, T end)
    {
        if (begin == end)
            return "";

        std::string returnValue;
        const auto prevEnd = std::prev(end);
        auto current = begin;
        while (current != end)
        {
            returnValue += *current;
            if (current != prevEnd)
                returnValue += joiner;
            current = std::next(current);
        }
        return returnValue;
    }
    template<class T>
    std::string Join(T begin, T end)
    {
        return Join("", begin, end);
    }

    template<class String>
    std::vector<String> Split(const String& string, const String& splitter)
    {
        if (string.empty())
            return {};

        auto manipulateString = string;
        auto splitterPosition = manipulateString.find(splitter);
        if (splitterPosition == String::npos)
            return { manipulateString };

        std::vector<String> returnValue;
        returnValue.reserve(CountInstances(string, splitter));

        while (splitterPosition != String::npos)
        {
            auto substr = manipulateString.substr(0, splitterPosition);
            if (!substr.empty())
                returnValue.push_back(substr);
            manipulateString.erase(0, splitterPosition + splitter.size());
            splitterPosition = manipulateString.find(splitter);
        }

        if (!manipulateString.empty())
            returnValue.push_back(manipulateString);

        return returnValue;
    }

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
        return detail::FromStringImpl(arg, ::Chroma::TypeIdentity<T>{});
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
    std::string ToString(bool arg);
    std::string ToString(const std::string& arg);
    std::string ToString(const std::wstring& arg);
    std::string ToString(const std::filesystem::path& arg);
}