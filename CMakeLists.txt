cmake_minimum_required (VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

project(tcp)

#bsuco la existencia de external_includes
file(GLOB external_includes ${CMAKE_CURRENT_BINARY_DIR}/external_includes)

if(external_includes)
    message(STATUS "external_includes encontrado")
else()
    
    message(STATUS "Creando external_includes")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external_includes)

    message(STATUS "Descargando dependencia -ImGui")
    execute_process(
        COMMAND git clone "https://github.com/ocornut/imgui" imgui
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external_includes
    )
    message(STATUS "Descargando dependencia -mem-editor")
    execute_process(
        COMMAND git clone "https://github.com/ocornut/imgui_club" mem-editor
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external_includes
    )

endif()

# Agregar los archivos fuente de ImGui a tu lista de fuentes
file(GLOB IMGUI_SOURCES 
    ${CMAKE_CURRENT_BINARY_DIR}/external_includes/imgui/*.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/external_includes/imgui/backends/imgui_impl_glfw.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/external_includes/imgui/backends/imgui_impl_opengl3.cpp
    )
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/external_includes/imgui
    ${CMAKE_CURRENT_BINARY_DIR}/external_includes/imgui/backends
    ${CMAKE_CURRENT_BINARY_DIR}/external_includes/mem-editor/imgui_memory_editor
    )

file(GLOB INCLUDE_SOURCES src/include/*.cpp)
include_directories(src/include)

set(SOURCES
    src/main.cpp
    ${INCLUDE_SOURCES}
    ${IMGUI_SOURCES}  # Agregar aquí los archivos fuente de ImGui
)

add_executable(main ${SOURCES})
#target_link_libraries(main m)
target_link_libraries(main z)
target_link_libraries(main glfw)
target_link_libraries(main GL) #POR ALGUNA RAZON VA EN MAYUSCULAS AAAAAAAAAAAAAAAAAAAAAAAHHHHHHHH

add_custom_target(tcp ALL
    DEPENDS main
    COMMENT "VAMOOOOOOOOOOOOS PAPA COMPILO"
    COMMAND ./main
    COMMENT "EJECUTANAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAANDO"
)
