#include "imgui_options.hpp"

// use a known ini configuration below
// prevents the user from having to place windows when first using the application
// imgui/imgui.cpp modifications at line 13378, 13406, 13570
#define USE_INI_STR

// prevent imgui from saving imgui.ini
// used with USE_INI_STR
#define DO_NOT_SAVE_INI

// create a script for converting imgui.ini to a string to paste below
//#define SAVE_INI_STR_SCRIPT


#ifdef USE_INI_STR

// save custom ini to disk
// makes for easier modifcation during development

namespace ini_str
{
    constexpr auto INI_STR = 

"[Window][DockSpaceViewport_11111111]\n"
"Pos=0,0\n"
"Size=1382,777\n"
"Collapsed=0\n"
"\n"
"[Window][Debug##Default]\n"
"Pos=60,60\n"
"Size=400,400\n"
"Collapsed=0\n"
"\n"

#ifdef SHOW_IMGUI_DEMO
"[Window][Dear ImGui Demo]\n"
"Pos=0,131\n"
"Size=432,443\n"
"Collapsed=0\n"
"DockId=0x00000007,2\n"
"\n"
"[Window][Hello, world!]\n"
"Pos=0,131\n"
"Size=432,443\n"
"Collapsed=0\n"
"DockId=0x00000007,3\n"
"\n"
#endif

"[Window][Status]\n"
"Pos=0,0\n"
"Size=432,129\n"
"Collapsed=0\n"
"DockId=0x00000006,0\n"
"\n"
"[Window][Inspect]\n"
"Pos=0,131\n"
"Size=432,443\n"
"Collapsed=0\n"
"DockId=0x00000007,0\n"
"\n"
"[Window][Topology]\n"
"Pos=434,0\n"
"Size=948,288\n"
"Collapsed=0\n"
"DockId=0x00000003,0\n"
"\n"
"[Window][Train]\n"
"Pos=434,290\n"
"Size=948,284\n"
"Collapsed=0\n"
"DockId=0x0000000A,0\n"
"\n"
"[Window][Test]\n"
"Pos=434,290\n"
"Size=948,284\n"
"Collapsed=0\n"
"DockId=0x0000000A,1\n"
"\n"
"[Window][Activations]\n"
"Pos=0,576\n"
"Size=1382,201\n"
"Collapsed=0\n"
"DockId=0x00000002,0\n"
"\n"

#ifdef ALLOC_COUNT
"[Window][Diagnostics]\n"
"Pos=0,131\n"
"Size=432,443\n"
"Collapsed=0\n"
"DockId=0x00000007,1\n"
"\n"
#endif

"[Docking][Data]\n"
"DockSpace       ID=0x8B93E3BD Window=0xA787BDB4 Pos=0,0 Size=1382,777 Split=Y\n"
"  DockNode      ID=0x00000001 Parent=0x8B93E3BD SizeRef=1382,574 Split=X\n"
"    DockNode    ID=0x00000004 Parent=0x00000001 SizeRef=432,598 Split=Y Selected=0x559F4DD0\n"
"      DockNode  ID=0x00000006 Parent=0x00000004 SizeRef=338,129 Selected=0x559F4DD0\n"
"      DockNode  ID=0x00000007 Parent=0x00000004 SizeRef=338,443 Selected=0x7FB45FEC\n"
"    DockNode    ID=0x00000005 Parent=0x00000001 SizeRef=948,598 Split=Y\n"
"      DockNode  ID=0x00000003 Parent=0x00000005 SizeRef=948,288 Selected=0x411CE2EF\n"
"      DockNode  ID=0x0000000A Parent=0x00000005 SizeRef=948,284 CentralNode=1 Selected=0x44A6A033\n"
"  DockNode      ID=0x00000002 Parent=0x8B93E3BD SizeRef=1382,201 Selected=0x887087C1\n"
"\n"


/* end INI_STR */; 

} // ini_str

#endif // USE_INI_STR


#ifdef SAVE_INI_STR_SCRIPT

namespace ini_str
{
    // add a .bat (windows) file for converting imgui.ini to a string to paste above
    void ini_to_str_script();
}

#endif // SAVE_INI_STR_SCRIPT


#define IMGUI_IMPLEMENTATION
#define IMGUI_USE_STB_SPRINTF

#include "../../../../libs/imgui/misc/single_file/imgui_single_file.h"

#include "../../../../libs/imgui/backends/imgui_impl_sdl2.cpp"
#include "../../../../libs/imgui/backends/imgui_impl_opengl3.cpp"


#ifdef SAVE_INI_STR_SCRIPT

#include <fstream>

namespace ini_str
{
    // add a .bat (windows) file for converting imgui.ini to a string to paste above
    inline void ini_to_str_script()
    {
        constexpr auto bat_str = 

        "#!/bin/bash\n"
        "\n"
        "# chmod +x convert_ini.sh"
        "\n"
        "# Define the input and output files\n"
        "inputFile=\"imgui.ini\"\n"
        "outputFile=\"ini_str.txt\"\n"
        "\n"
        "# Clear the output file\n"
        "> \"$outputFile\"\n"
        "\n"
        "# Write text to the file\n"
        "while IFS= read -r line\n"
        "do\n"
        "echo \"\\\"$line\\\\n\\\"\" >> \"$outputFile\"\n"
        "done < \"$inputFile\"\n"
        "\n"
        "# Confirm the write operation\n"
        "if [ $? -eq 0 ]; then\n"
        "    echo \"Text written to $outputFile successfully.\"\n"
        "else\n"
        "    echo \"Failed to write to $outputFile.\"\n"
        "fi\n"
        /* end string */;

        std::ofstream file("convert_ini.sh");
        if (file.is_open())
        {
            file << bat_str;
            file.close();
        }

    }
}

#endif