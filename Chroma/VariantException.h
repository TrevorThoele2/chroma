#pragma once

namespace Chroma
{
    class VariantException : public std::exception
    {
    public:
        VariantException(const char* message) : exception(message)
        {}
    };
}