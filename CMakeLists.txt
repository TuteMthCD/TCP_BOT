cmake_minimum_required (VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

project(tcp)

set(SOURCES
    src/main.cpp
    src/include/bot.cpp
    src/include/varTypes.cpp
)

add_executable(main ${SOURCES})
include_directories(src/include)
#target_link_libraries(main m)
target_link_libraries(main z)

add_custom_target(tcp ALL
    DEPENDS main
    COMMENT "VAMOOOOOOOOOOOOS PAPA COMPILO"
    COMMAND ./main
    COMMENT "EJECUTANAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAANDO"
)
