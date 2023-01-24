cmake_minimum_required(VERSION 3.23)
project(Xshell7_Xftp7_Mod C)

set(CMAKE_C_STANDARD 99)

add_executable(Xshell7_Xftp7_Mod_NoUAC main.c)
add_executable(Xshell7_Xftp7_Mod_UAC main.c)

set_target_properties(Xshell7_Xftp7_Mod_UAC PROPERTIES LINK_FLAGS " /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")

if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_definitions(-DDEBUG_OUTPUT=1)
else ()
    add_definitions(-DDEBUG_OUTPUT=0)
endif ()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)
set_target_properties (Xshell7_Xftp7_Mod_NoUAC Xshell7_Xftp7_Mod_UAC PROPERTIES DEBUG_POSTFIX _d)
