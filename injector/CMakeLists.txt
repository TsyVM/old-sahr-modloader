# Check if source file exists
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/injector.cpp)
    message(FATAL_ERROR "injector.cpp not found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# Injector executable
add_executable(SHARInjector
    ${CMAKE_CURRENT_SOURCE_DIR}/injector.cpp
)

# Set output directory explicitly
set_target_properties(SHARInjector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)

target_compile_definitions(SHARInjector PRIVATE
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
)

if(MSVC)
    target_compile_options(SHARInjector PRIVATE /W4 /WX-)
endif()

install(TARGETS SHARInjector
    RUNTIME DESTINATION bin
)