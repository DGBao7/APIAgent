cmake_minimum_required(VERSION 3.20)

project(APIAgent LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(APIAgent
    main.cpp
    ${SOURCES}

    include/maf/ui/MainWindow.h
    include/maf/ui/InputBar.h
    include/maf/ui/Conversation.h
    include/maf/ui/History.h
    include/maf/ui/theme/Color.h
)

target_include_directories(APIAgent PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(APIAgent PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
    Qt6::Widgets
)