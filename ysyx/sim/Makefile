SHELL=/bin/bash


# opt config
WAVE_FORMAT ?= FST #(FST, VCD)


SIM_PATH          := $(shell pwd)
ROOT_PATH         := $(SIM_PATH)/../
BUILD_DIR         := $(SIM_PATH)/build
SOC_CSRC_HOME     += $(SIM_PATH)/src
SOC_CSRC_LIB_HOME += $(ROOT_PATH)/peripheral/spiFlash
SOC_CXXFILES      += $(shell find $(SOC_CSRC_HOME) -name "*.cpp")
SOC_CXXFILES      += $(shell find $(SOC_CSRC_LIB_HOME) -name "*.cpp")

SOC_VSRC_HOME     += $(BUILD_DIR)
SOC_COMPILE_HOME  := $(SOC_VSRC_HOME)/emu-compile
SOC_VSRC_TOP      := ysyxSoCFull
SOC_VSRC_LIB_HOME += $(ROOT_PATH)/peripheral/
SOC_VXXFILES      += $(shell find $(SOC_VSRC_HOME) -name "*.v")
SOC_VXXFILES      += $(shell find $(SOC_VSRC_LIB_HOME) -name "*.v")

SOC_VSRC_INCLPATH += -I$(SOC_VSRC_HOME)
SOC_VSRC_INCLPATH += -I$(ROOT_PATH)/peripheral/uart16550/rtl
SOC_VSRC_INCLPATH += -I$(ROOT_PATH)/peripheral/spi/rtl
SOC_CSRC_INCLPATH += -I$(SOC_CSRC_HOME)
SOC_CSRC_INCLPATH += -I$(SOC_CSRC_LIB_HOME)

# if want to ouput vcd wave, replace '-DDUMP_WAVE_FST' to '-DDUMP_WAVE_VCD',
# replace '--trace-fst' to '--trace'
SOC_CXXFLAGS += -std=c++11 -static -Wall $(SOC_CSRC_INCLPATH) -DDUMP_WAVE_$(WAVE_FORMAT)
SOC_FLAGS    += --cc --exe --top-module $(SOC_VSRC_TOP) 
SOC_FLAGS    += --x-assign unique -O3 -CFLAGS "$(SOC_CXXFLAGS)"
SOC_FLAGS    += --trace-fst --assert --stats-vars --output-split 30000 --output-split-cfuncs 30000 
SOC_FLAGS    += --timescale "1ns/1ns" -Wno-fatal
SOC_FLAGS    += -o $(BUILD_DIR)/emu
SOC_FLAGS    += -Mdir $(BUILD_DIR)/emu-compile
SOC_FLAGS    += $(SOC_VSRC_INCLPATH) $(SOC_CXXFILES) $(SOC_VXXFILES)

CCACHE := $(if $(shell which ccache),ccache,)
ifneq ($(CCACHE),)
export OBJCACHE = ccache
endif

pre-build:
	verilator $(SOC_FLAGS)

# modify '-j1' to `-j$(nproc)` to improve compile speed
build:
	$(MAKE) VM_PARALLEL_BUILDS=1 OPT_FAST="-O3" -C $(SOC_COMPILE_HOME) -f V$(SOC_VSRC_TOP).mk -j1

.PHONY: pre-build build