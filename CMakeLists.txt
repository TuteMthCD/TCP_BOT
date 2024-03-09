cmake_minimum_required (VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

project(tcp)

add_executable(main src/main.cpp src/bot.cpp)
include_directories(src)
#target_link_libraries(main m)
target_link_libraries(main z)

add_custom_target(tcp ALL
    DEPENDS main
    COMMENT "VAMOOOOOOOOOOOOS PAPA COMPILO"
    COMMAND ./main
    COMMENT "EJECUTANAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAANDO"
)
