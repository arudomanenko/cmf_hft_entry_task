cmake_minimum_required(VERSION 3.20)
project(hft_task)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(HFT_TASK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE SOURCES "src/*.cpp")

message(STATUS "Building as library (hft_task)")
add_library(hft_task STATIC ${SOURCES})

target_include_directories(hft_task
    PUBLIC
        $<BUILD_INTERFACE:${HFT_TASK_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)
target_compile_features(hft_task PRIVATE cxx_std_20)

# --- Установка ---
install(TARGETS hft_task
        EXPORT hft_taskTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

install(DIRECTORY ${HFT_TASK_INCLUDE_DIR}/ DESTINATION include)

install(EXPORT hft_taskTargets
        FILE hft_taskTargets.cmake
        NAMESPACE hft_task::
        DESTINATION lib/cmake/hft_task)
