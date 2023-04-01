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
// File Name: Main.cpp
// Date File Created: 03/30/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "Skyborn.h"
#include "SandboxGame.h"

#pragma comment(lib, "Skyborn.lib")


using namespace sky;

scope<app::game, memory_tag::game> create_game()
{
    return scope<app::game, memory_tag::game>{create_scope<sandbox_game, memory_tag::game>(app::application_desc{ 300, 300, 1280, 720, "Skyborn Sandbox" }).release()};
}