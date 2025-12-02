cmake_minimum_required(VERSION 3.16)
project(ChessGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_standard_project_setup()

qt6_add_executable(ChessGame
        src/main.cpp
        src/ChessGame.cpp
        src/ChessBoardWidget.cpp
        src/ChessLogic.cpp
)

qt6_add_resources(ChessGame "resources"
        PREFIX "/"
        FILES
        resources/images/black_piece.png
        resources/images/white_piece.png
)

# 添加 PRIVATE 关键字解决签名冲突问题
target_link_libraries(ChessGame PRIVATE Qt6::Core Qt6::Widgets)
