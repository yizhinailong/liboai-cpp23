cmake_minimum_required(VERSION 3.15)
project(liboai VERSION 4.0.1 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC特定设置
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# 查找依赖
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)

# 创建库
add_library(liboai)
add_library(liboai::liboai ALIAS liboai)

# 使用target_sources添加源文件
target_sources(liboai
    PRIVATE
    "src/components/audio.cpp"
    "src/components/azure.cpp"
    "src/components/chat.cpp"
    "src/components/completions.cpp"
    "src/components/edits.cpp"
    "src/components/embeddings.cpp"
    "src/components/files.cpp"
    "src/components/fine_tunes.cpp"
    "src/components/images.cpp"
    "src/components/models.cpp"
    "src/components/moderations.cpp"
    "src/core/authorization.cpp"
    "src/core/netimpl.cpp"
    "src/core/response.cpp"
)

# 设置公共头文件
target_sources(liboai
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "include"
    FILES
    "include/liboai/liboai.h"
    "include/liboai/components/audio.h"
    "include/liboai/components/azure.h"
    "include/liboai/components/chat.h"
    "include/liboai/components/completions.h"
    "include/liboai/components/edits.h"
    "include/liboai/components/embeddings.h"
    "include/liboai/components/files.h"
    "include/liboai/components/fine_tunes.h"
    "include/liboai/components/images.h"
    "include/liboai/components/models.h"
    "include/liboai/components/moderations.h"
    "include/liboai/core/authorization.h"
    "include/liboai/core/exception.h"
    "include/liboai/core/netimpl.h"
    "include/liboai/core/network.h"
    "include/liboai/core/response.h"
)

# 设置包含目录
target_include_directories(liboai
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖
target_link_libraries(liboai
    PUBLIC
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

# 组织Visual Studio中的文件
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 将源文件分组用于IDE
source_group("components" FILES
    "src/components/audio.cpp"
    "src/components/azure.cpp"
    "src/components/chat.cpp"
    "src/components/completions.cpp"
    "src/components/edits.cpp"
    "src/components/embeddings.cpp"
    "src/components/files.cpp"
    "src/components/fine_tunes.cpp"
    "src/components/images.cpp"
    "src/components/models.cpp"
    "src/components/moderations.cpp"
)

source_group("core" FILES
    "src/core/authorization.cpp"
    "src/core/netimpl.cpp"
    "src/core/response.cpp"
)

# IDE设置
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT liboai)

# 构建示例选项
option(BUILD_EXAMPLES "Build example applications" OFF)
if(BUILD_EXAMPLES)
    add_subdirectory(documentation)
endif()

# 安装规则
include(CMakePackageConfigHelpers)

install(TARGETS liboai
    EXPORT liboaiTargets
    FILE_SET HEADERS
    DESTINATION "include"
)

install(EXPORT liboaiTargets
    FILE "liboaiTargets.cmake"
    NAMESPACE "liboai::"
    DESTINATION "lib/cmake/liboai"
)

# 创建配置文件内容
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/liboaiConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/liboai"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/liboaiConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY "AnyNewerVersion"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/liboaiConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/liboaiConfigVersion.cmake"
    DESTINATION "lib/cmake/liboai"
)
