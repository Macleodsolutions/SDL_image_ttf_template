cmake_minimum_required (VERSION 3.8)
project ("CMakeProject1")

# Custom function to copy a list of files to a destination folder
function(copy_files SOURCE_FILES DESTINATION_FOLDER TARGET_NAME)
    set(copied_files_list)
    foreach(file_path IN LISTS SOURCE_FILES)
        get_filename_component(file_name "${file_path}" NAME)
        set(destination_file "${DESTINATION_FOLDER}/${file_name}")
        list(APPEND copied_files_list "${destination_file}")

        add_custom_command(OUTPUT "${destination_file}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${file_path}" "${destination_file}"
            DEPENDS "${file_path}" COMMENT "Copying file: ${file_name}" VERBATIM)
    endforeach()

    add_custom_target("${TARGET_NAME}" ALL DEPENDS ${copied_files_list})
endfunction()

# Enable Hot Reload for MSVC compilers if supported
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    if(MSVC)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image>=2.0.0)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf>=2.0.0)
else()
    include(FetchContent)

    # FetchContent for SDL2
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-2.28.5
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(SDL2)

    # FetchContent for SDL2_image
    FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG        release-2.6.3
        GIT_PROGRESS   TRUE
    )
    set(SDL2IMAGE_INSTALL OFF)
    FetchContent_MakeAvailable(SDL2_image)

    # FetchContent for SDL2_ttf
    FetchContent_Declare(
        SDL2_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG        release-2.20.2
        GIT_PROGRESS   TRUE
    )
    set(SDL2TTF_INSTALL OFF)
    FetchContent_MakeAvailable(SDL2_ttf)
endif()

if(WIN32)
    # Copy specific files for SDL2_image
    copy_files(
        "${SDL2_image_SOURCE_DIR}/SDL_image.h"
        "${SDL2_BINARY_DIR}/include/SDL2"
        sdl2-image-files
    )
    add_dependencies(sdl2-image-files SDL2_image)

    # Copy specific files for SDL2_ttf
    copy_files(
        "${SDL2_ttf_SOURCE_DIR}/SDL_ttf.h"
        "${SDL2_BINARY_DIR}/include/SDL2"
        sdl2-ttf-files
    )
    add_dependencies(sdl2-ttf-files SDL2_ttf)
endif()

add_executable("${PROJECT_NAME}" "CMakeProject1.cpp" "CMakeProject1.h")

# Link libraries and include directories
if(UNIX AND NOT APPLE)
    target_include_directories("${PROJECT_NAME}" PUBLIC 
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )
    target_link_libraries("${PROJECT_NAME}" PUBLIC 
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
    )
else()
    target_link_libraries("${PROJECT_NAME}" PUBLIC SDL2 SDL2main SDL2_image SDL2_ttf)
    target_include_directories("${PROJECT_NAME}" PUBLIC 
        "${SDL2_BINARY_DIR}/include"
        "${SDL2_IMAGE_INCLUDE_DIRS}"
        "${SDL2_TTF_INCLUDE_DIRS}"
    )
endif()

if(WIN32)
    # Copy DLLs to the build folder either debug SDL2d.dll SDL2_imaged.dll SDL2_ttfsd.dll or release SDL2.dll SDL2_image.dll SDL2_ttf.dll
    set(SDL2_DLL_DEBUG "${SDL2_BINARY_DIR}/SDL2d.dll")
    set(SDL2_IMAGE_DLL_DEBUG "${SDL2_image_BINARY_DIR}/SDL2_imaged.dll")
    set(SDL2_TTF_DLL_DEBUG "${SDL2_ttf_BINARY_DIR}/SDL2_ttfd.dll")
    set(SDL2_DLL_RELEASE "${SDL2_BINARY_DIR}/SDL2.dll")
    set(SDL2_IMAGE_DLL_RELEASE "${SDL2_image_BINARY_DIR}/SDL2_image.dll")
    set(SDL2_TTF_DLL_RELEASE "${SDL2_ttf_BINARY_DIR}/SDL2_ttf.dll")

    # Copy DLLs to build output folder
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<$<CONFIG:Debug>:${SDL2_DLL_DEBUG}>$<$<CONFIG:Release>:${SDL2_DLL_RELEASE}>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<$<CONFIG:Debug>:${SDL2_IMAGE_DLL_DEBUG}>$<$<CONFIG:Release>:${SDL2_IMAGE_DLL_RELEASE}>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<$<CONFIG:Debug>:${SDL2_TTF_DLL_DEBUG}>$<$<CONFIG:Release>:${SDL2_TTF_DLL_RELEASE}>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()

# Copy assets folder to build output folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
