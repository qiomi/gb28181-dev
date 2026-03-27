cmake_minimum_required(VERSION 3.14)
project(gb28181-dev VERSION 1.0.0 LANGUAGES CXX C)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置默认构建类型为 Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# 生成编译数据库 (compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 选项设置
option(GB28181_BUILD_EXAMPLES "Build examples" ON)
option(GB28181_BUILD_TESTS "Build tests" ON)
option(GB28181_BUILD_SHARED_LIBS "Build shared library" ON)

# 编译选项
add_compile_options(-Wall -Wextra -Wpedantic)

# 如果是 Debug 模式，添加调试选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g -O0)
  add_definitions(-DDEBUG)
else()
  add_compile_options(-O2)
endif()

# 查找必需的库
find_package(Threads REQUIRED)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# ============================================================================
# 库定义
# ============================================================================

# 收集源文件
file(GLOB_RECURSE GB28181_SOURCES 
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.c"
)

# 创建库
if(GB28181_BUILD_SHARED_LIBS)
  add_library(gb28181-dev SHARED ${GB28181_SOURCES})
  target_compile_definitions(gb28181-dev PRIVATE GB28181_DEV_SHARED)
else()
  add_library(gb28181-dev STATIC ${GB28181_SOURCES})
endif()

target_include_directories(gb28181-dev
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(gb28181-dev
  PRIVATE
    Threads::Threads
)

# 设置库的属性
set_target_properties(gb28181-dev PROPERTIES
  OUTPUT_NAME "gb28181-dev"
  VERSION ${PROJECT_VERSION}
  SOVERSION 1
  POSITION_INDEPENDENT_CODE ON
)

# ============================================================================
# 示例程序
# ============================================================================

if(GB28181_BUILD_EXAMPLES)
  file(GLOB EXAMPLE_SOURCES "${CMAKE_SOURCE_DIR}/examples/*.cpp")
  foreach(EXAMPLE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE})
    target_link_libraries(${EXAMPLE_NAME} gb28181-dev)
  endforeach()
endif()

# ============================================================================
# 测试
# ============================================================================

if(GB28181_BUILD_TESTS)
  enable_testing()
  file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")
  foreach(TEST ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST} NAME_WE)
    add_executable(${TEST_NAME} ${TEST})
    target_link_libraries(${TEST_NAME} gb28181-dev)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
  endforeach()
endif()

# ============================================================================
# 安装规则
# ============================================================================

install(TARGETS gb28181-dev
  EXPORT gb28181-dev-targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT gb28181-dev-targets
  FILE gb28181-dev-config.cmake
  NAMESPACE gb28181-dev::
  DESTINATION lib/cmake/gb28181-dev
)

# ============================================================================
# 打包配置
# ============================================================================

set(CPACK_PACKAGE_NAME "gb28181-dev")
set(CPACK_PACKAGE_VENDOR "qiomi")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lightweight GB28181 Device SDK")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

include(CPack)
