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
//  File Name: Clock.h
//  Date File Created: 04/01/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Common.h"

namespace sky::core
{

class clock
{
public:
    using high_res_clock = std::chrono::high_resolution_clock;
    using duration       = std::chrono::duration<f64>;
    using time_point     = high_res_clock::time_point;

    constexpr clock() = default;

    // Starts clock and resets elapsed time
    void start();

    // Stops clock and resets start time but not elapsed time
    void stop();

    // Resets start and elapsed time to 0
    void reset();

    // Updates the clock. Should be called before checking elapsed time
    void update();

    // update() should be called first. This retrieves the elapsed time (current time - start_time)
    [[nodiscard]] constexpr f64 elapsed() const { return m_elapsed; }

private:
    //time_point m_start_time{};
    f64 m_start_time{};
    f64        m_elapsed{};
};

} // namespace sky::core