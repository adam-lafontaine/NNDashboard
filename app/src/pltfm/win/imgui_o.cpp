#include "imgui_options.hpp"


#define IMGUI_IMPLEMENTATION
#define IMGUI_USE_STB_SPRINTF

// replace generated ini file with a custom ini text
// imgui/imgui.cpp modifications at line 13378, 13406, 13570
#define USE_INI_STR



#ifdef USE_INI_STR

// same custom ini to disk
// makes for easier modifcation during development
#define SAVE_INI_STR

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

#ifdef SHOW_IMGUI_DEMO
    "[Window][Dear ImGui Demo]\n"
    "Pos=0,131\n"
    "Size=432,388\n"
    "Collapsed=0\n"
    "DockId=0x00000007,2\n"

    "[Window][Hello, world!]\n"
    "Pos=0,131\n"
    "Size=432,388\n"
    "Collapsed=0\n"
    "DockId=0x00000007,3\n"
#endif

    "[Window][Status]\n"
    "Pos=0,0\n"
    "Size=432,129\n"
    "Collapsed=0\n"
    "DockId=0x00000006,0\n"

    "[Window][Inspect]\n"
    "Pos=0,131\n"
    "Size=432,388\n"
    "Collapsed=0\n"
    "DockId=0x00000007,0\n"

    "[Window][Topology]\n"
    "Pos=434,0\n"
    "Size=948,244\n"
    "Collapsed=0\n"
    "DockId=0x00000003,0\n"

    "[Window][Train]\n"
    "Pos=434,246\n"
    "Size=948,273\n"
    "Collapsed=0\n"
    "DockId=0x0000000A,0\n"

    "[Window][Test]\n"
    "Pos=434,246\n"
    "Size=948,273\n"
    "Collapsed=0\n"
    "DockId=0x0000000A,1\n"

    "[Window][Activations]\n"
    "Pos=0,521\n"
    "Size=1382,256\n"
    "Collapsed=0\n"
    "DockId=0x00000002,0\n"

#ifdef ALLOC_COUNT
    "[Window][Diagnostics]\n"
    "Pos=0,131\n"
    "Size=432,388\n"
    "Collapsed=0\n"
    "DockId=0x00000007,1\n"
#endif

    "[Docking][Data]\n"
    "DockSpace       ID=0x8B93E3BD Window=0xA787BDB4 Pos=0,0 Size=1382,777 Split=Y\n"
    "  DockNode      ID=0x00000001 Parent=0x8B93E3BD SizeRef=1382,519 Split=X\n"
    "    DockNode    ID=0x00000004 Parent=0x00000001 SizeRef=432,598 Split=Y Selected=0x559F4DD0\n"
    "      DockNode  ID=0x00000006 Parent=0x00000004 SizeRef=338,129 Selected=0x559F4DD0\n"
    "      DockNode  ID=0x00000007 Parent=0x00000004 SizeRef=338,388 Selected=0x9FEC4C38\n"
    "    DockNode    ID=0x00000005 Parent=0x00000001 SizeRef=948,598 Split=Y\n"
    "      DockNode  ID=0x00000003 Parent=0x00000005 SizeRef=948,244 Selected=0x411CE2EF\n"
    "      DockNode  ID=0x0000000A Parent=0x00000005 SizeRef=948,273 CentralNode=1 Selected=0xAB789A92\n"
    "  DockNode      ID=0x00000002 Parent=0x8B93E3BD SizeRef=1382,256 Selected=0x887087C1\n"
    "\n";

}

#endif


#include "../../../../libs/imgui/misc/single_file/imgui_single_file.h"

#include "../../../../libs/imgui/backends/imgui_impl_sdl2.cpp"
#include "../../../../libs/imgui/backends/imgui_impl_dx11.cpp"