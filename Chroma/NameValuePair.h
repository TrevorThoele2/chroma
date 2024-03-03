#pragma once

#include <string>

namespace Chroma
{
    struct NameValuePair
    {
        std::string name;
        std::string value;

        NameValuePair() = default;
        NameValuePair(const std::string& name, const std::string& value);

        bool operator==(const NameValuePair& arg) const;
        bool operator!=(const NameValuePair& arg) const;
    };
}