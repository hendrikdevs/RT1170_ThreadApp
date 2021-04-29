#find Zephyr. THis also loads Zephyr's build system.
cmake_minimum_required(VERSION 3.13.1)
set(BOARD mimxrt1064_evk)
find_package(Zephyr)
project(threadapp)

include_directories(../defines)

#add sources, they must come after find_package(Zephyr)
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/threads.c)
target_sources(app PRIVATE src/usb.c)
target_sources(app PRIVATE src/peripheral_can.c)