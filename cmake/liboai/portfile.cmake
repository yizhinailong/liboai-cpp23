vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO yizhinailong/liboai
    REF v4.0.1
    SHA512 2f75bc79b6cabd1024f4891aba58d0cdc91ecc959e7e4e79abf8e9e5af69935486a4a1052cbc72fad1e12a3ea50083bc8df08b19c9356ed39e768c57fa24438b
    HEAD_REF main
)

# 修复Windows平台上的符号导出问题
if(VCPKG_TARGET_IS_WINDOWS)
    set(liboai_defines -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON)
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
    -DBUILD_EXAMPLES=OFF
    ${liboai_defines}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/liboai)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_copy_pdbs()

file(INSTALL
    "${SOURCE_PATH}/LICENSE"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
    RENAME copyright
)
