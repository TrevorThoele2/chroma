#include "Optional.h"

namespace Chroma
{
    OptionalException::OptionalException() : std::runtime_error("An invalid optional value was dereferenced.")
    {}
}