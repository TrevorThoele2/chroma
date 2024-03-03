#include "NameValuePair.h"

namespace Chroma
{
    bool NameValuePair::operator==(const NameValuePair& arg) const
    {
        return name == arg.name && value == arg.value;
    }

    bool NameValuePair::operator!=(const NameValuePair& arg) const
    {
        return !(*this == arg);
    }
}