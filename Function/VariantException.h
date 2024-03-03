#pragma once

namespace function
{
    class VariantException : public std::exception
    {
    public:
        VariantException(const char* message) : exception(message)
        {}
    };
}