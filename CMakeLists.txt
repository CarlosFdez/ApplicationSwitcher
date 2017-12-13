project(ApplicationSwitcher)
enable_language(C)
enable_language(CXX)

# CMake Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources
file(GLOB_RECURSE source_list "*.cpp" "*.hpp" "*.h")

add_executable(
    ${PROJECT_NAME}
    ${source_list}
)