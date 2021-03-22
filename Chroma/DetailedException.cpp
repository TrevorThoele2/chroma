#include "DetailedException.h"

namespace Chroma
{
    DetailedException::DetailedException(const std::string& message) :
        message(message)
    {}

    DetailedException::DetailedException(const std::string& message, NameValuePairs details) :
        message(message), details(details)
    {}

    std::string DetailedException::Message() const
    {
        return message;
    }

    auto DetailedException::Details() const -> NameValuePairs
    {
        return details;
    }
}