cmake_minimum_required(VERSION 3.16)
project(s3_springboard_web C CXX)

# Emscripten build of the simulator: emcmake cmake -S web -B build-web && cmake --build build-web -j
get_filename_component(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.. ABSOLUTE)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(LV_CONF_PATH ${ROOT}/lv_conf.h)
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE)
add_compile_options(-sUSE_SDL=2 -O2)
add_link_options(-sUSE_SDL=2)
include_directories(${ROOT} ${ROOT}/src)

add_subdirectory(${ROOT}/lvgl lvgl_build)
target_include_directories(lvgl PUBLIC ${ROOT})

file(GLOB SHELL_SOURCES ${ROOT}/src/os/*.c ${ROOT}/src/apps/*.c)

# Doom (doomgeneric)
set(DOOM_DIR ${ROOT}/src/third_party/doomgeneric)
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
set_source_files_properties(${DOOM_SOURCES} PROPERTIES COMPILE_OPTIONS "-w;-fno-strict-aliasing;-fwrapv;-DDOOMGENERIC_RESX=320;-DDOOMGENERIC_RESY=200")
set_source_files_properties(${DOOM_DIR}/r_data.c ${DOOM_DIR}/r_things.c PROPERTIES COMPILE_OPTIONS "-w;-O1;-fno-strict-aliasing;-fwrapv;-DDOOMGENERIC_RESX=320;-DDOOMGENERIC_RESY=200")
add_executable(index ${ROOT}/src/main.c ${ROOT}/src/hal/hal.c ${ROOT}/src/mouse_cursor_icon.c ${SHELL_SOURCES} ${DOOM_SOURCES})
target_include_directories(index PRIVATE ${DOOM_DIR})
target_compile_definitions(index PRIVATE S3_DOOM)
target_link_libraries(index lvgl lvgl::thorvg m)
set_target_properties(index PROPERTIES SUFFIX ".html")
target_link_options(index PRIVATE
    -sFORCE_FILESYSTEM=1 -sEXPORTED_FUNCTIONS=_main,_malloc,_free,_s3_web_picked,_s3_web_http_done -sEXPORTED_RUNTIME_METHODS=ccall,FS,HEAPU8
    -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=100663296 -sSTACK_SIZE=1048576
    --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell.html)
