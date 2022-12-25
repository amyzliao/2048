cmake_minimum_required(VERSION 3.3)
project(Catch2
        VERSION 2020.2.0
        DESCRIPTION "A reusable main for Catch2"
        LANGUAGES CXX)

###
### MAIN LIBRARY SETUP
###

add_library(catch
        include/catch.hxx
        src/catch.cxx)

set_target_properties(catch PROPERTIES
        CXX_EXTENSIONS  Off
        VERSION         ${PROJECT_VERSION})

target_compile_features(catch PUBLIC
    cxx_std_14)

target_include_directories(catch SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_include_directories(catch PRIVATE
        include)

###
### LIBRARY INSTALLATION
###

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CATCH2_INSTALL_EXPORT_NAME
    Catch2
    CACHE STRING "Name for installing Catch2 CMake files")
mark_as_advanced(CATCH2_INSTALL_EXPORT_NAME)
set(pkg_name ${CATCH2_INSTALL_EXPORT_NAME})

set(config_version_cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${pkg_name}/${pkg_name}ConfigVersion.cmake)
set(config_install_dir
    ${CMAKE_INSTALL_DATADIR}/cmake/${pkg_name})

install(TARGETS catch
        EXPORT ${pkg_name}Config)
install(DIRECTORY   include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT      ${pkg_name}Config
        DESTINATION ${config_install_dir})
install(FILES       ${config_version_cmake}
        DESTINATION ${config_install_dir})

export( TARGETS catch
        FILE ${pkg_name}Config.cmake)
write_basic_package_version_file(${config_version_cmake}
        VERSION         ${CMAKE_PROJECT_VERSION}
        COMPATIBILITY   SameMajorVersion)

