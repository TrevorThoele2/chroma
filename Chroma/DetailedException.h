#pragma once

#include <stdexcept>
#include <vector>
#include "NameValuePair.h"

namespace Chroma
{
    class DetailedException : public std::runtime_error
    {
    public:
        using NameValuePairs = std::vector<NameValuePair>;
    public:
        explicit DetailedException(const std::string& message);
        DetailedException(const std::string& message, NameValuePairs details);
        
        [[nodiscard]] NameValuePairs Details() const;
    private:
        NameValuePairs details;
    };
}