#include "Common.h"
#include "Platform/Platform.h"

#pragma comment(lib, "Skyborn.lib")

using namespace sky;

int main(int argc, char* argv[])
{
    platform::write_console("[WARNING] Just a test\n", 5);

    return 0;
}
