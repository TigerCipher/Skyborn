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
//  File Name: Clock.cpp
//  Date File Created: 04/01/2023
//  Author: Matt
// 
//  ------------------------------------------------------------------------------

#include "Clock.h"

#include "Skyborn/Platform/Platform.h"
#include <chrono>


namespace sky::core
{

void clock::start()
{
    //m_start_time = high_res_clock::now();
    m_start_time = platform::get_time();
    m_elapsed = 0.0;
}

void clock::stop()
{
    //m_start_time = {};
    m_start_time = 0.0;
}

void clock::reset()
{
    //m_start_time = {};
    m_start_time = 0.0;
    m_elapsed = 0.0;
}

void clock::update()
{
    //if (m_start_time == high_res_clock::time_point{})
    //    return;
    if(m_start_time == 0.0) return;
    //const duration elapsed{ high_res_clock::now() - m_start_time };
    //m_elapsed = elapsed.count();
    m_elapsed = platform::get_time() - m_start_time;
}
}
