#pragma once

//---------------------------------------------------------------------------------------------------------------

#if defined(_DEBUG)

#include "custom_console_output.hpp" // for msg_ assert
#include <cassert>                   // for msg_assert
#include <iostream>                  // for msg_assert

#define ON_DEBUG(...) __VA_ARGS__
#define OFF_DEBUG(...)

#define builtin_unreachable_wrapper(debug_message) msg_assert(false, debug_message)

#define msg_assert(bool_expression, message)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(bool_expression))                                                                                        \
        {                                                                                                              \
            std::cerr << RED BOLD "assertation failed:\n" << (message) << RESET_CONSOLE_OUT << std::endl;              \
            assert(0);                                                                                                 \
        }                                                                                                              \
    } while (0)

#else /* defined(_DEBUG) */

#define ON_DEBUG(...)
#define OFF_DEBUG(...) __VA_ARGS__

#define builtin_unreachable_wrapper(debug_message) __builtin_unreachable()

#define msg_assert(bool_expression, message)

#endif /* defined(_DEBUG) */

//---------------------------------------------------------------------------------------------------------------
