cmake_minimum_required(VERSION 3.16)
project(AgentDashboardProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Bật công cụ tự động xử lý Q_OBJECT
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Tìm thư viện giao diện Qt6
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Nạp 3 file vào dự án
add_executable(app_main 
    main.cpp
    AgentDashboard.h
    AgentDashboard.cpp
)

target_link_libraries(app_main PRIVATE Qt6::Widgets)