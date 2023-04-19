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
// File Name: Expect.h
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include <Skyborn/Debug/Logger.h>
#include <Skyborn/Util/Maths.h>

#define expect_should_be(expected, actual)                                                                             \
    if ((actual) != (expected))                                                                                        \
    {                                                                                                                  \
        LOG_ERROR("--> Expected {}, but got {} . File: {}:{}", expected, actual, __FILE__, __LINE__);                  \
        return 0;                                                                                                      \
    }

#define expect_should_not_be(expected, actual)                                                                         \
    if ((actual) == (expected))                                                                                        \
    {                                                                                                                  \
        LOG_ERROR("--> Expected {} != {} but they were ==. File: {}:{}", expected, actual, __FILE__, __LINE__);        \
        return 0;                                                                                                      \
    }

#define expect_float_to_equal(expected, actual)                                                                        \
    if (!sky::math::near_equal(expected, actual, 0.001f))                                                              \
    {                                                                                                                  \
        LOG_ERROR("--> Expected {}, but got {} . File: {}:{}", expected, actual, __FILE__, __LINE__);                  \
        return 0;                                                                                                      \
    }

#define expects_to_be_true(actual)                                                                                     \
    if (!(actual))                                                                                                     \
    {                                                                                                                  \
        LOG_ERROR("--> Expected '{}' to be true, but it was false. File: {}:{}", #actual, __FILE__, __LINE__);         \
        return 0;                                                                                                      \
    }

#define expects_to_be_false(actual)                                                                                    \
    if ((actual))                                                                                                      \
    {                                                                                                                  \
        LOG_ERROR("--> Expected '{}' to be false, but it was true. File: {}:{}", #actual, __FILE__, __LINE__);         \
        return 0;                                                                                                      \
    }