//  ------------------------------------------------------------------------------
//
//  Skyborn
//     Copyright 2023 Matthew Rogers
//
//     This library is free software; you can redistribute it and/or modify
//     it under the terms of the GNU Lesser General Public License as
//     published by the Free Software Foundation; either version 3 of the
//     License, or (at your option) any later version.
//
//     This library is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//     Lesser General Public License for more details.
//
//     You should have received a copy of the GNU Lesser General Public
//     License along with this library; if not, see <http://www.gnu.org/licenses/>.
//
//  File Name: TestManager.h
//  Date File Created: 04/09/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include <Defines.h>

constexpr u8 bypass = 2;

using func_test = u8 (*)();

namespace test_manager
{


void register_test(func_test test, const char* desc);

void run_tests();

} // namespace test_manager
