#pragma once

#include <vector>
#include "NameValuePair.h"

namespace Chroma
{
    class DetailedException
    {
    public:
        using NameValuePairs = std::vector<NameValuePair>;
    public:
        explicit DetailedException(const std::string& message);
        DetailedException(const std::string& message, NameValuePairs details);

        [[nodiscard]] std::string Message() const;

        [[nodiscard]] NameValuePairs Details() const;
    private:
        std::string message;
        NameValuePairs details;
    };
}