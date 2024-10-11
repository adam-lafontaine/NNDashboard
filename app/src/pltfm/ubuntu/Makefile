GPP := g++-11

GPP += -std=c++20
GPP += -mavx
#GPP += -O3
#GPP += -DNDEBUG

GPP += -DALLOC_COUNT

NO_FLAGS := 
SDL2   := `sdl2-config --cflags --libs`
OPENGL := -lGL -ldl

ALL_LFLAGS := $(SDL2) $(OPENGL) -lpthread


root       := ../../../..

app   := $(root)/app
build := $(app)/build/ubuntu
src   := $(app)/src

pltfm := $(src)/pltfm/ubuntu

libs := $(root)/libs

exe := nn_dashboard

program_exe := $(build)/$(exe)


#*** imgui ***

imgui := $(pltfm)
imgui_options_h := $(imgui)/imgui_options.hpp

imgui_include_h := $(imgui)/imgui_include.hpp
imgui_include_h += $(imgui_options_h)

#**************


#*** libs/util ***

util := $(libs)/util

types_h        := $(util)/types.hpp
stopwatch_h    := $(util)/stopwatch.hpp
stack_buffer_h := $(util)/stack_buffer.hpp

numeric_h := $(util)/numeric.hpp
numeric_h += $(types_h)

#************


#*** alloc_type ***

alloc_type := $(libs)/alloc_type

alloc_type_h := $(alloc_type)/alloc_type.hpp
alloc_type_h += $(types_h)

alloc_type_c := $(alloc_type)/alloc_type.cpp
alloc_type_c += $(alloc_type_h)

#*************


#*** memory_buffer ***

memory_buffer_h := $(util)/memory_buffer.hpp
memory_buffer_h += $(alloc_type_h)

#***********


#*** qsprintf ***

qsprintf := $(libs)/qsprintf

qsprintf_h := $(qsprintf)/qsprintf.hpp

qsprintf_c := $(qsprintf)/qsprintf.cpp

#***********


#*** span ***

span := $(libs)/span

span_h := $(span)/span.hpp
span_h += $(memory_buffer_h)
span_h += $(stack_buffer_h)
span_h += $(qsprintf_h)

span_c := $(span)/span.cpp
span_c += $(span_h)

#************


#*** image ***

image := $(libs)/image

image_h := $(image)/image.hpp
image_h += $(span_h)

image_c := $(image)/image.cpp
image_c += $(image_h)
image_c += $(numeric_h)

#*************


#*** sdl ***

sdl := $(libs)/sdl

sdl_include_h := $(sdl)/sdl_include.hpp

#**********


#*** mnist ***

mnist := $(libs)/mnist

mnist_h := $(mnist)/mnist.hpp
mnist_c := $(mnist)/mnist.cpp

#************


#*** nn_mlp ***

nn := $(libs)/nn

nn_mlp_h := $(nn)/nn_mlp.hpp
nn_mlp_h += $(span_h)

nn_mlp_c := $(nn)/nn_mlp.cpp

#*************


#*** mlai ***

mlai := $(src)/mlai

mlai_h := $(mlai)/mlai.hpp

mlai_c := $(mlai)/mlai.cpp

#************


#*** display ***

display := $(src)/display

display_h := $(display)/display.hpp
display_h += $(image_h)
display_h += $(mlai_h)
display_h += $(numeric_h)

#*************


#*** diagnostics ***

diagnostics := $(src)/diagnostics

diagnostics_h := $(diagnostics)/diagnostics.hpp
diagnostics_h += $(alloc_type_h)
diagnostics_h += $(qsprintf_h)

#*************


#*** main cpp ***

main_c := $(pltfm)/nn_dashboard_main_ubuntu.cpp
main_o := $(build)/main.o
obj    := $(main_o)

main_dep := $(imgui_include_h)
main_dep += $(sdl_include_h)
main_dep += $(display_h)
main_dep += $(diagnostics_h)

# main_o.cpp
main_dep += $(mlai_c)
main_dep += $(image_c)
main_dep += $(mnist_c)
main_dep += $(nn_mlp_c)
main_dep += $(alloc_type_c)
main_dep += $(span_c)
main_dep += $(qsprintf_c)

#****************


#*** imgui cpp ***

imgui_c := $(imgui)/imgui_o.cpp
imgui_o := $(build)/imgui.o
obj     += $(imgui_o)

imgui_dep := $(imgui_options_h)

#****************


#*** app ***


$(main_o): $(main_c) $(main_dep)
	@echo "\n  main"
	$(GPP) -o $@ -c $< $(ALL_LFLAGS)


$(imgui_o): $(imgui_c) $(imgui_dep)
	@echo "\n  imgui"
	$(GPP) -o $@ -c $< $(SDL2) $(OPENGL)

#**************


$(program_exe): $(obj)
	@echo "\n  program_exe"
	$(GPP) -o $@ $+ $(ALL_LFLAGS)


build: $(program_exe)


run: build
	$(program_exe)
	@echo "\n"


clean:
	rm -fv $(build)/*


clean_main:
	rm -fv $(build)/main.o

setup:
	mkdir -p $(build)