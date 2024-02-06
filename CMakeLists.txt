cmake_minimum_required(VERSION 3.23)

file(STRINGS "version" version)
project(SCLab VERSION ${version} LANGUAGES "CXX")

#---- set Qt-deps
set(CMAKE_CXX_STANDARD 20)

# if (CMAKE_BUILD_TYPE MATCHES "Debug")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -O1")
# endif ()

# ----------- set version
configure_file(${PROJECT_SOURCE_DIR}/version.h.in ${PROJECT_SOURCE_DIR}/src/global_manager/global_config/version.h)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QT_VERSION_MAJOR 6)

# define function
function(include_sub_directories_recursively root_dir)
    if (IS_DIRECTORY ${root_dir})
        include_directories(${root_dir})
        #message(STATUS " include dir ${root_dir} ")
    endif()
    file(GLOB ALL_SUB RELATIVE ${root_dir} ${root_dir}/*)
    foreach(sub ${ALL_SUB})
        if (IS_DIRECTORY ${root_dir}/${sub})
            include_sub_directories_recursively(${root_dir}/${sub})
        endif()
    endforeach()
endfunction()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.2.4/msvc2019_64")

    find_package(Qt6 COMPONENTS
            Core
            Gui
            Widgets
            Charts
            REQUIRED)


    #---- set third-deps
    set(deps_root ${CMAKE_CURRENT_SOURCE_DIR}/deps_win64)
    include_sub_directories_recursively(deps_root)

    set(nlohmann_root ${deps_root}/nlohmann)
    include_directories(${nlohmann_root}/include)

    set(saribbon_root ${deps_root}/saribbon)
    include_directories(${saribbon_root}/include)
    link_directories(${saribbon_root}/lib ${saribbon_root}/lib)

    set(zmq_root ${deps_root}/zmq)
    include_directories(${zmq_root}/include)
    link_directories(${zmq_root}/lib ${zmq_root}/bin)

    #---- set inner-deps
    include_sub_directories_recursively(${CMAKE_CURRENT_SOURCE_DIR}/src)

    #---- set resource-deps
    set(QRC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/resource/ribbon_resource.qrc)
    SOURCE_GROUP("Resource Files" FILES ${QRC_SOURCE})

    file(GLOB_RECURSE HDRS_${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
    file(GLOB_RECURSE SRCS_${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
    file(GLOB_RECURSE UIS_${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui)

    # generate execute file
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        add_executable(${PROJECT_NAME} WIN32
                ${QRC_SOURCE}
                ${HDRS_${PROJECT_NAME}}
                ${SRCS_${PROJECT_NAME}}
                ${UIS_${PROJECT_NAME}}
        )
    else()
        add_executable(${PROJECT_NAME}
                ${QRC_SOURCE}
                ${HDRS_${PROJECT_NAME}}
                ${SRCS_${PROJECT_NAME}}
                ${UIS_${PROJECT_NAME}}
        )
    endif ()
    target_link_libraries(${PROJECT_NAME}
            Qt::Core
            Qt::Gui
            Qt::Widgets
            Qt::Charts
    )
    target_link_libraries(${PROJECT_NAME}
            SARibbonBar)

    if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(DEBUG_SUFFIX)
        if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
            set(DEBUG_SUFFIX "d")
        endif ()
        set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
                set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            endif ()
        endif ()
        if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        endif ()
        foreach (QT_LIB Core Gui Widgets)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
        endforeach (QT_LIB)

        file(GLOB_RECURSE SHARED_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/deps_win64/*.dll)
        foreach (lib ${SHARED_LIBS})
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${lib}"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
        endforeach ()
    endif ()
else ()
    message(FATAL_ERROR "The compiler should be MSVC")
endif ()