// ------------------------------------------------------------------------------
//
// Skyborn
//    Copyright 2023 Matthew Rogers
//
//    This library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as
//    published by the Free Software Foundation; either version 3 of the
//    License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, see <http://www.gnu.org/licenses/>.
//
// File Name: Asserts.h
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include "Skyborn/Defines.h"

#define SKY_ASSERTIONS_ENABLED 1

#if SKY_ASSERTIONS_ENABLED
    #if _MSC_VER
        #include <intrin.h>
        #define DBG_BREAK __debugbreak
    #else
        #define DBG_BREAK __builtin_trap
    #endif

SAPI void report_assertion_failure(const char* expression, const char* message, const char* file, u32 line);

    #define gassert(expr)                                                                                              \
        {                                                                                                              \
            if (expr)                                                                                                  \
            {                                                                                                          \
            } else                                                                                                     \
            {                                                                                                          \
                report_assertion_failure(#expr, "", __FILE__, __LINE__);                                               \
                DBG_BREAK();                                                                                           \
            }                                                                                                          \
        }

    #define gassert_msg(expr, msg)                                                                                     \
        {                                                                                                              \
            if (expr)                                                                                                  \
            {                                                                                                          \
            } else                                                                                                     \
            {                                                                                                          \
                report_assertion_failure(#expr, msg, __FILE__, __LINE__);                                              \
                DBG_BREAK();                                                                                           \
            }                                                                                                          \
        }

    #ifdef _DEBUG
        #define gassert_dbg(expr)                                                                                      \
            {                                                                                                          \
                if (expr)                                                                                              \
                {                                                                                                      \
                } else                                                                                                 \
                {                                                                                                      \
                    report_assertion_failure(#expr, "", __FILE__, __LINE__);                                           \
                    DBG_BREAK();                                                                                       \
                }                                                                                                      \
            }
    #else
        #define gassert_dbg(expr)
    #endif

#else
    #define gassert(expr)
    #define gassert_msg(expr, msg)
    #define gassert_dbg(expr)
#endif