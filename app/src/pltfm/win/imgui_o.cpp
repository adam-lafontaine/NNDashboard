#include "imgui_options.hpp"


#define IMGUI_IMPLEMENTATION
#define IMGUI_USE_STB_SPRINTF

// replace generated ini file with a custom ini text
// imgui/imgui.cpp modifications at line 13378, 13406, 13570
#define USE_INI_STR

#ifdef USE_INI_STR

namespace ini_str
{
    constexpr auto INI_STR = 

    "[Window][DockSpaceViewport_11111111]\n"
    "Pos=0,0\n"
    "Size=1382,777\n"
    "Collapsed=0\n"

    "[Window][Debug##Default]\n"
    "Pos=60,60\n"
    "Size=400,400\n"
    "Collapsed=0\n"

    "[Window][Dear ImGui Demo]\n"
    "Pos=0,530\n"
    "Size=485,247\n"
    "Collapsed=0\n"
    "DockId=0x00000002,1\n"

    "[Window][Hello, world!]\n"
    "Pos=0,530\n"
    "Size=485,247\n"
    "Collapsed=0\n"
    "DockId=0x00000002,2\n"

    "[Window][Status]\n"
    "Pos=0,0\n"
    "Size=485,152\n"
    "Collapsed=0\n"
    "DockId=0x00000003,0\n"

    "[Window][Topology]\n"
    "Pos=487,0\n"
    "Size=895,281\n"
    "Collapsed=0\n"
    "DockId=0x00000009,0\n"

    "[Window][Train]\n"
    "Pos=487,283\n"
    "Size=895,241\n"
    "Collapsed=0\n"
    "DockId=0x00000008,0\n"

    "[Window][Test]\n"
    "Pos=487,283\n"
    "Size=895,241\n"
    "Collapsed=0\n"
    "DockId=0x00000008,1\n"

    "[Window][Activations]\n"
    "Pos=487,526\n"
    "Size=895,251\n"
    "Collapsed=0\n"
    "DockId=0x0000000A,0\n"

    "[Window][Diagnostics]\n"
    "Pos=0,530\n"
    "Size=485,247\n"
    "Collapsed=0\n"
    "DockId=0x00000002,0\n"

    "[Window][Inspect]\n"
    "Pos=0,154\n"
    "Size=485,374\n"
    "Collapsed=0\n"
    "DockId=0x00000006,0\n"

    "[Docking][Data]\n"
    "DockSpace       ID=0x8B93E3BD Window=0xA787BDB4 Pos=0,0 Size=1382,777 Split=X\n"
    "  DockNode      ID=0x00000001 Parent=0x8B93E3BD SizeRef=485,777 Split=Y Selected=0xAB789A92\n"
    "    DockNode    ID=0x00000003 Parent=0x00000001 SizeRef=455,152 Selected=0x559F4DD0\n"
    "    DockNode    ID=0x00000004 Parent=0x00000001 SizeRef=455,623 Split=Y Selected=0x9FEC4C38\n"
    "      DockNode  ID=0x00000006 Parent=0x00000004 SizeRef=485,374 Selected=0x9FEC4C38\n"
    "      DockNode  ID=0x00000002 Parent=0x00000004 SizeRef=485,247 Selected=0x7FB45FEC\n"
    "  DockNode      ID=0x00000005 Parent=0x8B93E3BD SizeRef=895,777 Split=Y\n"
    "    DockNode    ID=0x00000007 Parent=0x00000005 SizeRef=895,524 Split=Y Selected=0x411CE2EF\n"
    "      DockNode  ID=0x00000009 Parent=0x00000007 SizeRef=541,281 Selected=0x411CE2EF\n"
    "      DockNode  ID=0x00000008 Parent=0x00000007 SizeRef=541,241 Selected=0x44A6A033\n"
    "    DockNode    ID=0x0000000A Parent=0x00000005 SizeRef=895,251 CentralNode=1 Selected=0x887087C1\n";
}

#endif


#include "../../../../libs/imgui/misc/single_file/imgui_single_file.h"

#include "../../../../libs/imgui/backends/imgui_impl_sdl2.cpp"
#include "../../../../libs/imgui/backends/imgui_impl_dx11.cpp"