# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.13.1)

set(BOARD stm32f3_disco@E)
set(DTC_OVERLAY_FILE stm32f3disco.overlay)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(i3g4250d_tests)

add_compile_definitions(TEST)

zephyr_library_include_directories(
    ${ZEPHYR_BASE}/drivers/sensor/i3g4250d
    ${ZEPHYR_BASE}/../modules/hal/st/sensor/stmemsc/i3g4250d_STdC/driver
)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources} )
