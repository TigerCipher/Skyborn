﻿//  ------------------------------------------------------------------------------
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
//  File Name: CommonHeaders.h
//  Date File Created: 03/31/2023
//  Author: Matt
// 
//  ------------------------------------------------------------------------------

#pragma once

#include "Common.h"
#include "Debug/Logger.h"
#include "Memory/Memory.h"

template<typename T>
using ref = std::shared_ptr<T>;

template<typename T, typename... Args>
    requires constructible_from_args<T, Args...>
constexpr ref<T> create_ref(Args&&... args)
{
    return std::allocate_shared<T>(sky::memory::sky_allocator<T>{}, std::forward<Args>(args)...);
}