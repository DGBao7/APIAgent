cmake_minimum_required(VERSION 3.20)
project(APIAgentUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(app_main
    main.cpp
    ui/theme/Color.h
    ui/theme/Color.cpp
    ui/Conversation.h
    ui/Conversation.cpp
    ui/History.h
    ui/History.cpp
    ui/InputBar.h
    ui/InputBar.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
)

target_include_directories(app_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app_main PRIVATE Qt6::Widgets)
