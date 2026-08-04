cmake_minimum_required(VERSION 3.15)
project(SHARModLoader VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force 32-bit build (SHAR is 32-bit)
if(MSVC)
    set(CMAKE_GENERATOR_PLATFORM Win32)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Per-configuration output directories
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
endforeach()

# Fetch MinHook
include(FetchContent)
FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG v1.3.3
)
FetchContent_GetProperties(minhook)
if(NOT minhook_POPULATED)
    FetchContent_Populate(minhook)
    
    # Build MinHook manually as a static library
    add_library(minhook STATIC
        ${minhook_SOURCE_DIR}/src/buffer.c
        ${minhook_SOURCE_DIR}/src/hook.c
        ${minhook_SOURCE_DIR}/src/trampoline.c
        ${minhook_SOURCE_DIR}/src/hde/hde32.c
        ${minhook_SOURCE_DIR}/src/hde/hde64.c
    )
    
    target_include_directories(minhook PUBLIC
        ${minhook_SOURCE_DIR}/include
        ${minhook_SOURCE_DIR}/src/hde
    )
    
    # Set proper output directories for minhook
    set_target_properties(minhook PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif()

message(STATUS "MinHook source directory: ${minhook_SOURCE_DIR}")

# ModLoader DLL
add_library(SHARModLoader SHARED
    src/main.cpp
    src/mod_manager.cpp
    src/file_hooks.cpp
    src/utils.cpp
)

target_include_directories(SHARModLoader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(SHARModLoader PRIVATE
    minhook
    dbghelp
)

target_compile_definitions(SHARModLoader PRIVATE
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
)

# Compiler warnings and options
if(MSVC)
    target_compile_options(SHARModLoader PRIVATE 
        /W4 
        /WX-
        /EHa
    )
else()
    target_compile_options(SHARModLoader PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Injector executable
add_subdirectory(injector)

# Install rules
install(TARGETS SHARModLoader
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

# Create example mod structure
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/mods/01_example_mod/art/frontend)

message(STATUS "=== SHAR ModLoader Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Generator platform: ${CMAKE_GENERATOR_PLATFORM}")
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "=====================================")