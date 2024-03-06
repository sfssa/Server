# 最低所需cmake版本
cmake_minimum_required(VERSION 3.0)

# 项目名称
project(atpdxy)

# 显式指定C++标准版本
set(CMAKE_CXX_STANDARD 11)

# 设置编译标志
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

# 设置源文件
set(LIB_SRC
    atpdxy/log.cpp
    atpdxy/util.cpp
    )

# 创建共享库
add_library(${PROJECT_NAME} SHARED ${LIB_SRC})

# 创建可执行文件
add_executable(test_log ${PROJECT_SOURCE_DIR}/tests/test_log.cpp)

# 添加依赖项
add_dependencies(test_log ${PROJECT_NAME})

# 链接库文件
target_link_libraries(test_log ${PROJECT_NAME})

# 指定输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 安装目标文件
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(TARGETS test_log DESTINATION bin)