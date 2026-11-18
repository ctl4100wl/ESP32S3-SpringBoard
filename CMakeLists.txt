cmake_minimum_required(VERSION 3.12.4)
project(lvgl C CXX)

# Set the correct FreeRTOS port for your system (e.g., Posix for WSL)
set(FREERTOS_PORT GCC_POSIX CACHE STRING "Port for FreeRTOS on Posix environment")

set(LVGL_PRO_PROJECT_DIR
    ""
    CACHE PATH "Path to LVGL Pro Project folder")

option(USE_FREERTOS "Enable FreeRTOS" OFF) # Turn this on to enable FreeRTOS

if(USE_FREERTOS AND WIN32)
    message(STATUS "FreeRTOS is enabled (Windows MinGW port)")
    set(RTOS_DIR ${PROJECT_SOURCE_DIR}/FreeRTOS)
    set(FREERTOS_SOURCES
        ${RTOS_DIR}/croutine.c ${RTOS_DIR}/event_groups.c ${RTOS_DIR}/list.c ${RTOS_DIR}/queue.c
        ${RTOS_DIR}/stream_buffer.c ${RTOS_DIR}/tasks.c ${RTOS_DIR}/timers.c
        ${RTOS_DIR}/portable/MemMang/heap_4.c ${RTOS_DIR}/portable/MSVC-MingW/port.c)
    include_directories(${RTOS_DIR}/include ${RTOS_DIR}/portable/MSVC-MingW ${PROJECT_SOURCE_DIR}/config)
elseif(USE_FREERTOS)
    message(STATUS "FreeRTOS is enabled")

    # FreeRTOS integration when USE_FREERTOS is enabled
    add_library(freertos_config INTERFACE)
    target_include_directories(freertos_config SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/config)
    target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

    # Add FreeRTOS as a subdirectory
    add_subdirectory(FreeRTOS)

    # FreeRTOS-specific include directories
    include_directories(${PROJECT_SOURCE_DIR}/FreeRTOS/include)
    include_directories(${PROJECT_SOURCE_DIR}/FreeRTOS/portable/ThirdParty/GCC/Posix)
    include_directories(${PROJECT_SOURCE_DIR}/config)

    # Add FreeRTOS sources
    file(GLOB FREERTOS_SOURCES
        "${PROJECT_SOURCE_DIR}/FreeRTOS/*.c"
        "${PROJECT_SOURCE_DIR}/FreeRTOS/portable/MemMang/heap_4.c"
        "${PROJECT_SOURCE_DIR}/FreeRTOS/portable/ThirdParty/GCC/Posix/*.c"
    )
else()
    message(STATUS "FreeRTOS is disabled")
    set(FREERTOS_SOURCES "")  # No FreeRTOS sources if FreeRTOS is disabled
endif()

# Main include files of the project
include_directories(${PROJECT_SOURCE_DIR}/main/inc)

# Define options for LVGL with default values (OFF)
option(LV_USE_DRAW_SDL "Use SDL draw unit" OFF)
option(LV_USE_LIBPNG "Use libpng to decode PNG" OFF)
option(LV_USE_LIBJPEG_TURBO "Use libjpeg turbo to decode JPEG" OFF)
option(LV_USE_FFMPEG "Use libffmpeg to display video using lv_ffmpeg" OFF)
option(LV_USE_FREETYPE "Use freetype library" OFF)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output path for the executable
if(NOT DEFINED EXECUTABLE_OUTPUT_PATH)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
endif()
set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Find and include SDL2 library
find_package(SDL2 REQUIRED)

# Add compile definitions based on the selected options
add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)
add_compile_definitions($<$<BOOL:${LV_USE_LIBPNG}>:LV_USE_LIBPNG=1>)
add_compile_definitions($<$<BOOL:${LV_USE_LIBJPEG_TURBO}>:LV_USE_LIBJPEG_TURBO=1>)
add_compile_definitions($<$<BOOL:${LV_USE_FFMPEG}>:LV_USE_FFMPEG=1>)

# Add LVGL subdirectory
add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS})

file(GLOB SHELL_SOURCES CONFIGURE_DEPENDS src/os/*.c src/apps/*.c)
set(MAIN_SOURCES src/mouse_cursor_icon.c src/hal/hal.c ${SHELL_SOURCES})
if(NOT ESP_PLATFORM)
    # QuickJS: modern C JavaScript engine for the PC-only Lite Browser (MIT licence).
    set(QJS_DIR src/third_party/quickjs)
    set(QJS_SOURCES ${QJS_DIR}/quickjs.c ${QJS_DIR}/dtoa.c ${QJS_DIR}/libregexp.c
                    ${QJS_DIR}/libunicode.c ${QJS_DIR}/cutils.c ${QJS_DIR}/browser_js.c)
    list(APPEND MAIN_SOURCES ${QJS_SOURCES})
    set_source_files_properties(${QJS_SOURCES} PROPERTIES COMPILE_OPTIONS "-w")
    # litehtml (BSD) + gumbo (Apache 2.0): real HTML/CSS layout for the PC-only Lite Browser
    file(GLOB LH_SOURCES CONFIGURE_DEPENDS src/third_party/litehtml/src/*.cpp src/third_party/litehtml/src/gumbo/*.c)
    list(APPEND MAIN_SOURCES src/apps/browser_rich.cpp ${LH_SOURCES})
    set_source_files_properties(${LH_SOURCES} PROPERTIES COMPILE_OPTIONS "-w")
endif()
set(MAIN_LIBS lvgl lvgl::examples lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES})

# Create the main executable, depending on the FreeRTOS option
if(USE_FREERTOS AND WIN32)
    list(APPEND MAIN_SOURCES src/main.c src/freertos/freertos_win_port.c ${FREERTOS_SOURCES})
    set_source_files_properties(${FREERTOS_SOURCES} PROPERTIES COMPILE_OPTIONS "-w")
elseif(USE_FREERTOS)
    list(APPEND MAIN_SOURCES src/freertos_main.c src/freertos/freertos_posix_port.c ${FREERTOS_SOURCES})
    list(APPEND MAIN_LIBS freertos_config freertos_kernel)
else()
    list(APPEND MAIN_SOURCES src/main.c)
endif()


if(NOT MSVC)
    list(APPEND MAIN_LIBS m pthread)
endif()
if(WIN32)
    list(APPEND MAIN_LIBS winhttp)
endif()

# Custom target to run the executable
add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)

# Conditionally include and link SDL2_image if LV_USE_DRAW_SDL is enabled
if(LV_USE_DRAW_SDL)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
    find_package(SDL2_image REQUIRED)
    target_include_directories(lvgl PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
    list(APPEND MAIN_LIBS ${SDL2_IMAGE_LIBRARIES})
endif()

# Conditionally include and link libpng if LV_USE_LIBPNG is enabled
if(LV_USE_LIBPNG)
    find_package(PNG REQUIRED)
    target_include_directories(lvgl PUBLIC ${PNG_INCLUDE_DIRS})
    list(APPEND MAIN_LIBS ${PNG_LIBRARIES})
endif()

# Conditionally include and link libjpeg-turbo if LV_USE_LIBJPEG_TURBO is enabled
if(LV_USE_LIBJPEG_TURBO)
    find_package(JPEG REQUIRED)
    target_include_directories(lvgl PUBLIC ${JPEG_INCLUDE_DIRS})
    list(APPEND MAIN_LIBS ${JPEG_LIBRARIES})
endif()

# Conditionally include and link FFmpeg libraries if LV_USE_FFMPEG is enabled
if(LV_USE_FFMPEG)
    list(APPEND MAIN_LIBS avformat avcodec avutil swscale z)
endif()

# Conditionally include and link FreeType if LV_USE_FREETYPE is enabled
if(LV_USE_FREETYPE)
    find_package(Freetype REQUIRED)
    target_include_directories(lvgl PUBLIC ${FREETYPE_INCLUDE_DIRS})
    list(APPEND MAIN_LIBS ${FREETYPE_LIBRARIES})
endif()

if(LVGL_PRO_PROJECT_DIR)
  if(EXISTS "${LVGL_PRO_PROJECT_DIR}/CMakeLists.txt")
    add_subdirectory(${LVGL_PRO_PROJECT_DIR} ui)
    list(APPEND MAIN_LIBS lib-ui)

    target_compile_definitions(lib-ui PUBLIC LV_LVGL_H_INCLUDE_SIMPLE)
    target_include_directories(lib-ui PUBLIC
      ${LVGL_PRO_PROJECT_DIR}  # local headers like ui.h, generated files, etc.
      ${CMAKE_SOURCE_DIR}/lvgl # include "lvgl.h" works
    )
    message(STATUS "LVGL Pro Project enabled from: ${LVGL_PRO_PROJECT_DIR}")
  else()
    message(
      WARNING
        "LVGL_PRO_PROJECT_DIR specified but no CMakeLists.txt found at: ${LVGL_PRO_PROJECT_DIR}"
    )
  endif()
endif()

# Single self-contained .exe for Windows (MinGW): SDL2, libgcc, libstdc++ and winpthread linked statically,
# GUI subsystem (no console window). Use: cmake -DSTATIC_EXE=ON -DCMAKE_BUILD_TYPE=Release
option(STATIC_EXE "Link a standalone Windows executable" OFF)
if(STATIC_EXE AND MINGW)
    get_filename_component(_tc_dir ${CMAKE_C_COMPILER} DIRECTORY)
    set(_old_suffixes ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
    find_library(SDL2_STATIC_LIB NAMES SDL2 HINTS ${_tc_dir}/../lib REQUIRED)
    find_library(SDL2MAIN_STATIC_LIB NAMES SDL2main HINTS ${_tc_dir}/../lib REQUIRED)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_old_suffixes})
    list(REMOVE_ITEM MAIN_LIBS ${SDL2_LIBRARIES})
    list(APPEND MAIN_LIBS mingw32 ${SDL2MAIN_STATIC_LIB} ${SDL2_STATIC_LIB}
         kernel32 user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32 dinput8 comdlg32)
endif()

add_executable(main ${MAIN_SOURCES})
if(NOT ESP_PLATFORM)
    target_include_directories(main PRIVATE src/third_party/litehtml/include src/third_party/litehtml/include/litehtml src/third_party/litehtml/src/gumbo/include src/third_party/litehtml/src/gumbo/include/gumbo src/third_party/litehtml/src/gumbo)
    set_target_properties(main PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
endif()
if(NOT ESP_PLATFORM AND NOT EMSCRIPTEN)
    # Doom (doomgeneric, GPL-2.0) for fun on the PC build
    set(DOOM_DIR src/third_party/doomgeneric)
    set(DOOM_NAMES dummy am_map doomdef doomstat dstrings d_event d_items d_iwad d_loop d_main d_mode d_net f_finale f_wipe g_game
        hu_lib hu_stuff info i_cdmus i_endoom i_joystick i_scale i_sound i_system i_timer memio m_argv m_bbox m_cheat m_config
        m_controls m_fixed m_menu m_misc m_random p_ceilng p_doors p_enemy p_floor p_inter p_lights p_map p_maputl p_mobj p_plats
        p_pspr p_saveg p_setup p_sight p_spec p_switch p_telept p_tick p_user r_bsp r_data r_draw r_main r_plane r_segs r_sky
        r_things sha1 sounds statdump st_lib st_stuff s_sound tables v_video wi_stuff w_checksum w_file w_main w_wad z_zone
        w_file_stdc i_input i_video doomgeneric mus2mid)
    set(DOOM_SOURCES "")
    foreach(n ${DOOM_NAMES})
        list(APPEND DOOM_SOURCES ${DOOM_DIR}/${n}.c)
    endforeach()
    target_sources(main PRIVATE ${DOOM_SOURCES})
    set_source_files_properties(${DOOM_SOURCES} PROPERTIES COMPILE_OPTIONS "-w;-fno-strict-aliasing;-fwrapv;-DDOOMGENERIC_RESX=320;-DDOOMGENERIC_RESY=200")
    # the old texture code breaks under -O2/-O3 (startup hangs): these two files stay at -O1
    set_source_files_properties(${DOOM_DIR}/r_data.c ${DOOM_DIR}/r_things.c PROPERTIES COMPILE_OPTIONS "-w;-O1;-fno-strict-aliasing;-fwrapv;-DDOOMGENERIC_RESX=320;-DDOOMGENERIC_RESY=200")
    target_include_directories(main PRIVATE ${DOOM_DIR})
    target_compile_definitions(main PRIVATE S3_DOOM)
endif()
target_compile_definitions(main PRIVATE LV_CONF_INCLUDE_SIMPLE)
if(USE_FREERTOS AND WIN32)
    target_compile_definitions(main PRIVATE S3_RTOS)
    target_link_libraries(main winmm)
endif()

if(NOT ESP_PLATFORM AND NOT EMSCRIPTEN)
    # FFmpeg linked into the program (video, music tags and cover art). Falls back to ffmpeg.exe on PATH when absent.
    find_program(PKG_CONFIG_EXE pkg-config)
    if(PKG_CONFIG_EXE)
        set(FF_MODS libavformat libavcodec libswscale libswresample libavutil)
        set(FF_STATIC_FLAG "")
        if(STATIC_EXE)
            set(FF_STATIC_FLAG --static)
        endif()
        set(FF_LOCAL ${PROJECT_SOURCE_DIR}/third_party_bin/ffmpeg)
        if(EXISTS ${FF_LOCAL}/lib/pkgconfig/libavformat.pc)
            # lean static FFmpeg from scripts/build_ffmpeg.sh: no external codec libraries
            set(FF_ENV PKG_CONFIG_PATH=${FF_LOCAL}/lib/pkgconfig PKG_CONFIG_LIBDIR=${FF_LOCAL}/lib/pkgconfig)
        else()
            set(FF_ENV A=B)
        endif()
        execute_process(COMMAND ${CMAKE_COMMAND} -E env ${FF_ENV} ${PKG_CONFIG_EXE} ${FF_STATIC_FLAG} --libs ${FF_MODS}
                        OUTPUT_VARIABLE FF_LIBS RESULT_VARIABLE FF_RC OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
        execute_process(COMMAND ${CMAKE_COMMAND} -E env ${FF_ENV} ${PKG_CONFIG_EXE} --cflags ${FF_MODS}
                        OUTPUT_VARIABLE FF_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
        if(FF_RC EQUAL 0)
            separate_arguments(FF_LIBS_LIST NATIVE_COMMAND "${FF_LIBS}")
            separate_arguments(FF_CFLAGS_LIST NATIVE_COMMAND "${FF_CFLAGS}")
            target_compile_options(main PRIVATE ${FF_CFLAGS_LIST})
            target_link_libraries(main ${FF_LIBS_LIST})
            target_compile_definitions(main PRIVATE S3_FFMPEG)
            message(STATUS "FFmpeg found: media backend = libav")
        endif()
    endif()
endif()
if(NOT ESP_PLATFORM)
    target_compile_definitions(main PRIVATE CONFIG_VERSION="2025-09-13")
endif()
target_link_libraries(main ${MAIN_LIBS})

# On Windows, GUI applications do not show a console by default,
# which hides log output. This ensures a console is available for logging.
if (STATIC_EXE AND MINGW)
    target_link_options(main PRIVATE -mwindows -static -static-libgcc -static-libstdc++)
elseif (WIN32)
    if (MSVC)
        target_link_options(main PRIVATE "/SUBSYSTEM:CONSOLE")
    else()
        target_link_options(main PRIVATE "-mconsole")
    endif()
endif()

# Apply additional compile options if the build type is Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug mode enabled")

    target_compile_options(lvgl PRIVATE
        -pedantic-errors
        -Wall
        -Wclobbered
        -Wdeprecated
        -Wdouble-promotion
        -Wempty-body
        -Wextra
        -Wformat-security
        -Wmaybe-uninitialized
        # -Wmissing-prototypes
        -Wpointer-arith
        -Wmultichar
        -Wno-pedantic # ignored for now, we convert functions to pointers for properties table.
        -Wreturn-type
        -Wshadow
        -Wshift-negative-value
        -Wsizeof-pointer-memaccess
        -Wtype-limits
        -Wundef
        -Wuninitialized
        -Wunreachable-code
        -Wfloat-conversion
        -Wstrict-aliasing
    )

if (ASAN)
    message(STATUS "AddressSanitizer enabled")

    # Add AddressSanitizer flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
else()
    message(STATUS "AddressSanitizer disabled")
endif()
endif()
