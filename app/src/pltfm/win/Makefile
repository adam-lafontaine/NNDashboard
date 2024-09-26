GPP := g++

GPP += -std=c++20
GPP += -mavx
#GPP += -O3
#GPP += -DNDEBUG
GPP += -mwindows

#GPP += -DIMAGE_READ
#GPP += -DIMAGE_WRITE
#GPP += -DALLOC_COUNT

# directories
SDL := C:\SDL2\SDL2-2.30.5\x86_64-w64-mingw32

# include directory flags
ISDL := -I$(SDL)\include -I$(SDL)\include\SDL2

# link directory flags
LDSDL := -L$(SDL)\lib

# dll files
SDL_DLL := $(SDL)/bin/SDL2.dll

# flags
IFLAGS := $(ISDL)

LDX11 := -limm32 -ld3d11 -ld3dcompiler -ldxgi
LSDL  := $(LDSDL) -lmingw32 -lsdl2 -lSDL2main

ALL_LFLAGS :=  $(LSDL) $(LDX11)


root       := ../../../..

app   := $(root)/app
build := $(app)/build/win
src   := $(app)/src

pltfm := $(src)/pltfm/win

libs := $(root)/libs

exe := nn_dashboard.exe

program_exe := $(build)/$(exe)


#*** libs ***

alloc_type := $(libs)/alloc_type
span       := $(libs)/span
util       := $(libs)/util
sdl        := $(libs)/sdl

#************


#*** imgui ***

imgui := $(pltfm)
imgui_options_h := $(imgui)/imgui_options.hpp

imgui_include_h := $(imgui)/imgui_include.hpp
imgui_include_h += $(imgui_options_h)

#**************


#*** libs/util ***

types_h        := $(util)/types.hpp
stopwatch_h    := $(util)/stopwatch.hpp
stack_buffer_h := $(util)/stack_buffer.hpp

numeric_h := $(util)/numeric.hpp
numeric_h += $(types_h)

#************


#*** alloc_type ***

alloc_type_h := $(alloc_type)/alloc_type.hpp
alloc_type_h += $(types_h)

alloc_type_c := $(alloc_type)/alloc_type.cpp
alloc_type_c += $(alloc_type_h)

#*************


#*** memory_buffer ***

memory_buffer_h := $(util)/memory_buffer.hpp
memory_buffer_h += $(alloc_type_h)

#***********


#*** span ***

span_h := $(span)/span.hpp
span_h += $(memory_buffer_h)
span_h += $(stack_buffer_h)
span_h += $(qsprintf_h)

span_c := $(span)/span.cpp
span_c += $(span_h)

#************


#*** sdl ***

sdl_include_h := $(sdl)/sdl_include.hpp

#**********


#*** main cpp ***

main_c := $(pltfm)/nn_dashboard_main_win.cpp
main_o := $(build)/main.o
obj    := $(main_o)

main_dep := $(imgui_include_h)
main_dep += $(sdl_include_h)

# main_o.cpp
main_dep += $(alloc_type_c)
main_dep += $(span_c)


#****************


#*** imgui cpp ***

imgui_c := $(imgui)/imgui_o.cpp
imgui_o := $(build)/imgui.o
obj     += $(imgui_o)

imgui_dep := $(imgui_options_h)

#****************


#*** app ***

$(main_o): $(main_c) $(main_dep)
	@echo "  main"
	$(GPP) $(IFLAGS) -o $@ -c $< $(ALL_LFLAGS)


$(imgui_o): $(imgui_c) $(imgui_dep)
	@echo "  imgui"
	$(GPP) $(IFLAGS) -o $@ -c $< $(LSDL) $(LDX11)

#**************


$(program_exe): $(obj)
	@echo "  program_exe"
	$(GPP) -o $@ $+ $(ALL_LFLAGS)
	cp $(SDL_DLL) $(build)


build: $(program_exe)


run: build
	$(program_exe)
	@echo "\n"


clean:
	rm -rfv $(build)/*

setup:
	mkdir -p $(build)