# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.22.1)

# プロジェクト名と使用する言語を設定
project(SoukobanGame C)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# a.outという実行ファイルをmain.cppとhello.cppから作成
add_executable(soukoban soukoban.c autoresolve.c)
target_link_libraries(soukoban ${CURSES_LIBRARIES})

# message(STATUS "CURSES_INCLUDE_DIR: ${CURSES_INCLUDE_DIR}")
# message(STATUS "CURSES_LIBRARIES: ${CURSES_LIBRARIES}")
