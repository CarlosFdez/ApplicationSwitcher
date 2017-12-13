project(ApplicationSwitcher)
enable_language(C)
enable_language(CXX)

# CMake Settings
set(CMAKE_SUPPRESS_REGENERATION true)

# Sources
include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
)