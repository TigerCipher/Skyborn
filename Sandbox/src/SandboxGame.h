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
//  File Name: SandboxGame.h
//  Date File Created: 03/30/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Common.h"
#include "Core/Application.h"
#include "Debug/Logger.h"
#include "Memory/Memory.h"
#include "Utl/Array.h"

class sandbox_game : public sky::app::game
{
public:
    explicit sandbox_game(sky::app::application_desc desc) : game{ std::move(desc) } {}
    ~sandbox_game() override {}

    bool initialize() override
    {
        m_test = (test*) sky::memory::allocate(sizeof(test), sky::memory_tag::game);
        //m_test          = (test*) sky::memory::operator new(sizeof(test), sky::memory::memory_tag::game);
        m_test->data    = 2.45543;
        m_test->offset  = 32;
        m_test->padding = 8;
        m_test->id      = 12344352;


        m_test_ref   = create_ref<test2, sky::memory_tag::game>(32, 4, 2, 23.44, "Test2");
        m_test_scope = create_scope<test2, sky::memory_tag::entity>(32, 32, 32, 4.34, "Test Scope");
        sky::utl::array<u64> m_arr{};
        m_arr.push_back(3);
        m_arr.push_back(4);
        m_arr.push_back(5);
        m_arr.push_back(6);

        u32 idx{};
        for (u64 i : m_arr)
        {
            LOG_DEBUGF("{}: {}", idx, i);
            ++idx;
        }

        u64 top{ m_arr.back() };
        u64 front{ m_arr.front() };
        u64 at1{ m_arr[1] };

        u64 old_3{ *m_arr.erase_unordered(3) };

        LOG_DEBUGF("Top/Back {} Front {} 1: {}.. 3: {}", top, front, at1, old_3);
        idx = 0;
        for (u64 i : m_arr)
        {
            LOG_DEBUGF("{}: {}", idx, i);
            ++idx;
        }


        LOG_TRACE(sky::memory::get_usage_str());
        LOG_TRACE(std::format("Expected {} bytes", sizeof(test)));

        sky::memory::free(m_test, sizeof(test), sky::memory_tag::game);
        m_test_ref.reset();
        m_test_scope.reset();


        LOG_DEBUG("Sandbox game initialize");
        return true;
    }
    bool update(f32 delta) override
    {
        //LOG_DEBUG("Sandbox game update");
        return true;
    }
    bool render(f32 delta) override
    {
        //LOG_DEBUG("Sandbox game render");
        return true;
    }
    void on_resize(i32 width, i32 height) override { LOG_DEBUG("Sandbox game on_resize"); }

private:
    struct test
    {
        u64 offset{};
        u64 padding{};
        u32 id{};
        f64 data{};
    };

    struct test2
    {
        u64         offset{};
        u64         padding{};
        u32         id{};
        f64         data{};
        std::string name{};
    };

    test* m_test{};

    ref<test2, sky::memory_tag::game>     m_test_ref{};
    scope<test2, sky::memory_tag::entity> m_test_scope{};
};
