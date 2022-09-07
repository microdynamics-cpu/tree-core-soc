SHELL=/bin/bash

ID ?= 000000
# opt config
# if want to ouput vcd wave, replace 'FST' to 'VCD', this script can switch trace
# format by replacing '--trace-fst' to '--trace' automatically
WAVE_FORMAT ?= FST #(FST, VCD)
TRACE_FORMAT ?= --trace-fst
ifeq ($(WAVE_FORMAT), VCD)
    TRACE_FORMAT := --trace
endif

SIM_PATH          := $(shell pwd)
ROOT_PATH         := $(SIM_PATH)/..
BUILD_DIR         := $(SIM_PATH)/build
SOC_CSRC_HOME     += $(SIM_PATH)/src
SOC_CSRC_LIB_HOME += $(ROOT_PATH)/peripheral/spiFlash
SOC_CXXFILES      += $(shell find $(SOC_CSRC_HOME) -name "*.cpp")
SOC_CXXFILES      += $(shell find $(SOC_CSRC_LIB_HOME) -name "*.cpp")

SOC_VSRC_HOME     += $(BUILD_DIR)
SOC_COMPILE_HOME  := $(SOC_VSRC_HOME)/emu-compile
SOC_VSRC_TOP      := ysyxSoCFull
SOC_VSRC_LIB_HOME += $(ROOT_PATH)/peripheral/
SOC_VSRC_LIB_HOME += $(ROOT_PATH)/ram/
SOC_VXXFILES      += $(shell find $(SOC_VSRC_HOME) -name "*.v")
SOC_VXXFILES      += $(shell find $(SOC_VSRC_LIB_HOME) -name "*.v")

SOC_VSRC_INCLPATH += -I$(SOC_VSRC_HOME)
SOC_VSRC_INCLPATH += -I$(ROOT_PATH)/peripheral/uart16550/rtl
SOC_VSRC_INCLPATH += -I$(ROOT_PATH)/peripheral/spi/rtl
SOC_VSRC_INCLPATH += -I$(ROOT_PATH)/peripheral/sdram/rtl
SOC_CSRC_INCLPATH += -I$(SOC_CSRC_HOME)
SOC_CSRC_INCLPATH += -I$(SOC_CSRC_LIB_HOME)

SOC_CXXFLAGS += -std=c++11 -static -Wall $(SOC_CSRC_INCLPATH) -DDUMP_WAVE_$(WAVE_FORMAT)
SOC_FLAGS    += --cc --exe --top-module $(SOC_VSRC_TOP) 
SOC_FLAGS    += --x-assign unique -O3 -CFLAGS "$(SOC_CXXFLAGS)"
SOC_FLAGS    += $(TRACE_FORMAT) --assert --stats-vars --output-split 30000 --output-split-cfuncs 30000 
SOC_FLAGS    += --timescale "1ns/1ns" -Wno-fatal
SOC_FLAGS    += -o $(BUILD_DIR)/emu
SOC_FLAGS    += -Mdir $(BUILD_DIR)/emu-compile
SOC_FLAGS    += $(SOC_VSRC_INCLPATH) $(SOC_CXXFILES) $(SOC_VXXFILES)

CCACHE := $(if $(shell which ccache),ccache,)
ifneq ($(CCACHE),)
export OBJCACHE = ccache
endif

# if only need to moidfy core, comment below two line.
# need to remove 'TestHarness' module from the ysyxSoCFull.v,
# because it is the test top module
pre-build:
	@mkdir -p $(BUILD_DIR)
	@cp $(ROOT_PATH)/soc/$(SOC_VSRC_TOP).v $(BUILD_DIR)
	@sed -i s/ysyx_000000/ysyx_$(ID)/g $(BUILD_DIR)/ysyxSoCFull.v
	@sed -i '/module TestHarness/,/endmodule/d' $(BUILD_DIR)/ysyxSoCFull.v
	verilator $(SOC_FLAGS)

# modify '-j1' to `-j$(nproc)` to improve compile speed
build: pre-build
	$(MAKE) VM_PARALLEL_BUILDS=1 OPT_FAST="-O3" -C $(SOC_COMPILE_HOME) -f V$(SOC_VSRC_TOP).mk -j1

# flash, loader
SOC_APP_TYPE ?= flash
# hello, memtest, rtthread
SOC_APP_NAME ?= hello
SOC_TARGET := $(SOC_APP_NAME)-$(SOC_APP_TYPE)
SOC_SIM_TIME ?= -1
test:
# TODO: check if the bin exist!
	$(SOC_VSRC_HOME)/emu -t $(SOC_SIM_TIME) -i $(ROOT_PATH)/prog/bin/$(SOC_APP_TYPE)/$(SOC_TARGET).bin

.PHONY: pre-build build test

