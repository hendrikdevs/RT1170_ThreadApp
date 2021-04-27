#find Zephyr. THis also loads Zephyr's build system.
cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr)
project(threadapp)

include_directories(../defines)

#add sources, they must come after find_package(Zephyr)
target_sources(app PRIVATE src/main.c)

