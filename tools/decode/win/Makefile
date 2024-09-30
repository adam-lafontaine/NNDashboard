GPP := g++

GPP += -std=c++20
#GPP += -mavx
#GPP += -O3
#GPP += -DNDEBUG
#GPP += -mwindows

root       := ../../..

tools := $(root)/tools
decode := $(tools)/decode

build := $(tools)/build/win

pltfm := $(decode)/win

libs := $(root)/libs

exe := decode.exe

program_exe := $(build)/$(exe)


#*** libs ***

alloc_type := $(libs)/alloc_type
util       := $(libs)/util

#************



#*** libs/util ***

types_h        := $(util)/types.hpp

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


#*** main cpp ***

main_c := $(decode)/decode_mnist.cpp
main_o := $(build)/main.o
obj    := $(main_o)

main_dep := $(memory_buffer_h)
main_dep += $(alloc_type_c)

#****************



$(main_o): $(main_c) $(main_dep)
	@echo "  main"
	$(GPP) -o $@ -c $<


$(program_exe): $(obj)
	@echo "  program_exe"
	$(GPP) -o $@ $+


build: $(program_exe)


run: build
	$(program_exe)


clean:
	rm -rfv $(build)/*

setup:
	mkdir -p $(build)