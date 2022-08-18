cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(ff)

pico_sdk_init()

ADD_LIBRARY(feeder feeder.c)

target_link_libraries(feeder pico_stdlib pico_audio_i2s)

add_executable(ff
	ff.c
)
add_subdirectory(pico-ss-oled build/ssoled)
add_subdirectory(littlefs-lib build/littlefs)
add_subdirectory(cell build/cell)

# enable usb output, disable uart output
    pico_enable_stdio_usb (ff 1)
    pico_enable_stdio_uart(ff 0)

target_link_libraries(ff PRIVATE
	pico_stdlib
	pico_audio_i2s
	cell
	feeder
	pico_multicore
	hardware_adc
	hardware_i2c
	pico_ss_oled
	littlefs-lib
)


target_compile_definitions(ff PRIVATE

    PICO_NO_FPGA_CHECK=1
	USE_AUDIO_I2S=1
	PICO_AUDIO_I2S_MONO_INPUT=1
)

set(PICO_DEFAULT_FLOAT_IMPL pico_float_pico)

target_compile_options(ff PRIVATE

    -O3
	-march=armv6-m
	-mcpu=cortex-m0plus
	-mtune=cortex-m0plus
	-mfloat-abi=softfp
	# -falign-functions=16 
	# -falign-loops=16
	# -MMD 
	# -MP 
	# -MF
	# -fno-builtin
	# -ffunction-sections
    #-frename-registers
    #-funroll-loops
)

pico_set_binary_type(ff copy_to_ram)

pico_add_extra_outputs(ff)