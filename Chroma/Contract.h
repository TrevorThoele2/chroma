#pragma once

#ifndef NDEBUG
#define SOFT_PRECONDITION(check, exception) if(!(check)) abort();
#define DEBUG_PRECONDITION(check) if(!(check)) abort();
#define SOFT_ASSERT(check, exception) SOFT_PRECONDITION(check, exception)
#define DEBUG_ASSERT(check) DEBUG_PRECONDITION(check)
#else
#define SOFT_PRECONDITION(check, exception) if(!(check)) throw exception;
#define DEBUG_PRECONDITION(check) (void)0;
#define SOFT_ASSERT(check, exception) SOFT_PRECONDITION(check, exception)
#define DEBUG_ASSERT(check) DEBUG_PRECONDITION(check)
#endif