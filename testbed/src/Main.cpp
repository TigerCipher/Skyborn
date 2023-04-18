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
// Date File Created: 04/17/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "TestManager.h"

#include "Tests/VectorTest.h"
#include <Skyborn/Debug/Asserts.h>
#include <Skyborn/Debug/Logger.h>

int main(int argc, char** argv)
{
    LOG_INFO("Starting unit tests");
    register_vector_tests();
    tests::run_tests();

    return 0;
}