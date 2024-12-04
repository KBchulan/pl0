cmake_minimum_required(VERSION 3.10)

project(PL0 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 源文件列表
set(SOURCES
    src/main.cpp
    src/TokenInterpreter.cpp
    src/Parser.cpp
    src/AST.cpp
    src/SemanticAnalyzer.cpp
    src/Compiler.cpp
    src/ASTPrinter.cpp
)

# 头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 编译选项
if(CMAKE_BUILD_TYPE MATCHES "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -march=native)
    endif()
endif()

# 复制资源文件
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
endif()
