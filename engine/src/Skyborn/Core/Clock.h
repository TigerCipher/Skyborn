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
// File Name: Clock.h
// Date File Created: 04/17/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Skyborn/Defines.h"

namespace sky::core
{
class clock
{
public:
    SAPI constexpr clock() = default;

    // Starts clock and resets elapsed time
    SAPI void start();

    // Stops clock and resets start time but not elapsed time
    SAPI void stop();

    // Resets start and elapsed time to 0
    SAPI void reset();

    // Updates the clock. Should be called before checking elapsed time
    SAPI void update();

    // update() should be called first. This retrieves the elapsed time (current time - start_time)
    [[nodiscard]] SAPI constexpr f64 elapsed() const { return m_elapsed; }

private:
    f64 m_start_time{};
    f64 m_elapsed{};
};

} // namespace sky::core