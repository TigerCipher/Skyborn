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
//  File Name: Version.h
//  Date File Created: 03/31/2023
//  Author: Matt
// 
//  ------------------------------------------------------------------------------

#pragma once
#include "Defines.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define VERSION_BUILD 70

#define SKY_VERSION STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH) "." STRINGIFY(VERSION_BUILD)

#define SKY_ENGINE_NAME "Skyborn [Version " SKY_VERSION "]"
