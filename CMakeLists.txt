cmake_minimum_required(VERSION 3.9)

project(ApplicationSwitcher)
enable_language(C)
enable_language(CXX)

# CMake Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
	vendor/json/
	include/
)

# Sources
file(GLOB_RECURSE source_list "src/*")

add_executable(
    ${PROJECT_NAME}
	#WIN32
    ${source_list}
)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} Comctl32)
endif()