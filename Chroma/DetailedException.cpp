#include "DetailedException.h"

namespace Chroma
{
    DetailedException::DetailedException(const std::string& message) :
        runtime_error(message)
    {}

    DetailedException::DetailedException(const std::string& message, NameValuePairs details) :
        runtime_error(message), details(details)
    {}
    
    auto DetailedException::Details() const -> NameValuePairs
    {
        return details;
    }
}