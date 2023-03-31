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
//  File Name: Event.cpp
//  Date File Created: 03/31/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Event.h"

namespace sky::events
{

namespace
{

constexpr u32 max_message_codes{ 16384 };

bool is_initialized{false};

struct registered_event
{
    void*         listener;
    func_on_event callback;
};

struct event_code_entry
{
    registered_event* events;
};

event_code_entry registered[max_message_codes];

} // anonymous namespace

} // namespace sky::events