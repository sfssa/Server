# 最低所需cmake版本
cmake_minimum_required(VERSION 3.0)

# 项目名称
project(atpdxy)

# 包含自己设置的cmake
include (cmake/utils.cmake)

# 显式指定C++标准版本
set(CMAKE_CXX_STANDARD 11)

# 设置编译标志
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

# 设置源文件
set(LIB_SRC
    atpdxy/log.cpp
    atpdxy/util.cpp
    atpdxy/config.cpp
    atpdxy/thread.cpp
    atpdxy/mutex.cpp
    )

# 创建共享库
add_library(${PROJECT_NAME} SHARED ${LIB_SRC})

force_redefine_file_macro_for_sources(atpdxy)

set(LIB_LIB
    atpdxy
    pthread
    yaml-cpp)
# 创建可执行文件
add_executable(test_log ${PROJECT_SOURCE_DIR}/tests/test_log.cpp)
# 添加依赖项
add_dependencies(test_log ${PROJECT_NAME})
# 设置__FILE__
force_redefine_file_macro_for_sources(test_log)
# 链接库文件
target_link_libraries(test_log ${LIB_LIB})

add_executable(test_config ${PROJECT_SOURCE_DIR}/tests/test_config.cpp)
add_dependencies(test_config ${PROJECT_NAME})
force_redefine_file_macro_for_sources(test_config)
target_link_libraries(test_config ${LIB_LIB})

add_executable(test_thread ${PROJECT_SOURCE_DIR}/tests/test_thread.cpp)
add_dependencies(test_thread ${PROJECT_NAME})
force_redefine_file_macro_for_sources(test_thread)
target_link_libraries(test_thread ${LIB_LIB})

add_executable(test_util ${PROJECT_SOURCE_DIR}/tests/test_util.cpp)
add_dependencies(test_util ${PROJECT_NAME})
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util ${LIB_LIB})

# 指定输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 安装目标文件
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(TARGETS test_log DESTINATION bin)
