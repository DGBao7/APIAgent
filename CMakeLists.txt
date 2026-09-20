cmake_minimum_required(VERSION 3.20)

project(APIAgent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(APIAgent
    main.cpp
    ${SOURCES}
)

target_include_directories(APIAgent PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(APIAgent PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
)