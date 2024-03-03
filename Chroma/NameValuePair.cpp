#include "NameValuePair.h"

namespace Chroma
{
    NameValuePair::NameValuePair(const std::string& name, const std::string& value) :
        name(name), value(value)
    {}

    bool NameValuePair::operator==(const NameValuePair& arg) const
    {
        return name == arg.name && value == arg.value;
    }

    bool NameValuePair::operator!=(const NameValuePair& arg) const
    {
        return !(*this == arg);
    }
}