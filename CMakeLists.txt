cmake_minimum_required(VERSION 3.23)
project(oai VERSION 4.0.1 LANGUAGES CXX)

# MSVC 特定设置
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# 查找依赖
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)

# 创建库
add_library(${PROJECT_NAME})
add_library(liboai::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# 定义源文件变量
set(CORE_SOURCES
    "src/core/authorization.cpp"
    "src/core/netimpl.cpp"
    "src/core/response.cpp"
)

set(COMPONENTS_SOURCES
    "src/components/chat.cpp"
)

# 定义头文件变量
set(HEADER_FILES
    "include/liboai/liboai.h"
    "include/liboai/components/chat.h"
    "include/liboai/core/authorization.h"
    "include/liboai/core/exception.h"
    "include/liboai/core/netimpl.h"
    "include/liboai/core/network.h"
    "include/liboai/core/response.h"
)

# 源文件配置
target_sources(${PROJECT_NAME}
    PRIVATE
    ${CORE_SOURCES}
    ${COMPONENTS_SOURCES}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "include"
    FILES ${HEADER_FILES}
)

# 包含目录
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

# IDE 组织
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group("components" FILES ${COMPONENTS_SOURCES})
source_group("core" FILES ${CORE_SOURCES})
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT liboai)

# 构建示例
option(BUILD_EXAMPLES "Build example applications" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# 引入独立安装规则模块
include(cmake/Install.cmake)
