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
// File Name: TestManager.cpp
// Date File Created: 04/17/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "TestManager.h"

#include <Skyborn/Util/Vector.h>
#include <Skyborn/Debug/Logger.h>
#include <Skyborn/Core/Clock.h>

using namespace sky;

namespace tests
{
namespace
{
struct test_entry
{
    func_test   func{};
    const char* desc{};
};

utl::vector<test_entry> test_entries{};

} // anonymous namespace

void register_test(func_test test, const char* desc)
{
    test_entries.push_back({ test, desc });
}

void run_tests()
{
    u32 passed{ 0 };
    u32 failed{ 0 };
    u32 skipped{ 0 };

    core::clock timer{};
    timer.start();

    for (u32 i = 0; i < test_entries.size(); ++i)
    {
        core::clock test_timer{};
        test_timer.start();
        const u8 result{ test_entries[i].func() };
        test_timer.update();

        if (result == 1)
        {
            ++passed;
        } else if (result == bypass)
        {
            LOG_WARN("[SKIPPED]: {}", test_entries[i].desc);
            ++skipped;
        } else
        {
            LOG_ERROR("[FAILED]: {}", test_entries[i].desc);
            ++failed;
        }

        timer.update();

        std::string status{ failed ? std::format("*** {} FAILED ***", failed) : "SUCCESS" };
        LOG_INFO("Executed {} of {} (skipped {}) {} ({:.6f} sec / {:.6f} sec total)", i + 1, test_entries.size(),
                 skipped, status, test_timer.elapsed(), timer.elapsed());
    }

    timer.stop();

    LOG_INFO("Results: {} passed, {} failed, {} skipped", passed, failed, skipped);
}

} // namespace tests