cmake_minimum_required(VERSION 3.3)

function(add_installer name)
    cmake_parse_arguments(pa "NO_RESOURCES" "TARGET" "RESOURCES" ${ARGN})
    default_to(pa_TARGET "${name}")

    set(search_path
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
            ${GE211_RESOURCE_PATH})

    if(pa_NO_RESOURCES)
        set(resource_files)
    elseif(pa_RESOURCES)
        find_all(resource_files VERBOSE
                FILES   ${pa_RESOURCES}
                CALLED  Resource
                IN      ${search_path}
                AS      add_installer)
    else()
        glob_dirs(resource_files ${search_path})
    endif()

    ge211_installer_name("${name}")
    ge211_installer_add("${pa_TARGET}" ${resource_files})
endfunction(add_installer)

# vim: ft=cmake
