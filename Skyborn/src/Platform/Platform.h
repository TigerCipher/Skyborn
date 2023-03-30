#pragma once

#include <string_view>

#include "Common.h"

namespace sky::platform
{

//bool startup(std::string_view app_name, i32 x, i32 y, i32 width, i32 height);
//void shutdown();

SAPI void write_console(std::string_view msg, u8 color);
SAPI void write_error(std::string_view msg, u8 color);

}