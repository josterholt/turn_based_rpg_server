cmake_minimum_required(VERSION 3.6)
project(DungeonPuzzler)

set(CMAKE_CXX_STANDARD 11)

set(PACKAGE "DungeonPuzzler")
set(CPACK_PACKAGE_NAME "${PACKAGE}")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_VENDOR "shroder@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PACKAGE} ${PACKAGE_VERSION}")
set(SOVERSION "8")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(VERSION "${CPACK_PACKAGE_VERSION}")

set(LWS_LIBRARY_VERSION ${CPACK_PACKAGE_VERSION})
set(LWS_LIBRARY_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR})
set(LWS_LIBRARY_VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR})
set(LWS_LIBRARY_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

message(STATUS "CMAKE_TOOLCHAIN_FILE='${CMAKE_TOOLCHAIN_FILE}'")

add_definitions(-D_DEBUG)
add_definitions(-DLWS_WITH_NO_LOGS)
add_definitions(-DLWS_USE_LIBUV)

##
## Add helper files
##
set(SOURCES)

# Add helper files for Windows.
if (WIN32)
    set(WIN32_HELPERS_PATH D:/libs/libwebsockets/win32port/win32helpers)
    include_directories(${WIN32_HELPERS_PATH})

    if (WIN32)
        message("#### Including gettimeofday")
        list(APPEND SOURCES
                ${WIN32_HELPERS_PATH}/gettimeofday.c
                )

        list(APPEND HDR_PRIVATE
                ${WIN32_HELPERS_PATH}/gettimeofday.h
                )
    endif(WIN32)

else()
    # Unix.
        message("### UNIX")
    #if (NOT LWS_WITHOUT_DAEMONIZE)
    #    list(APPEND SOURCES
    #            lib/daemonize.c)
    #endif()
endif()

if (WIN32)
    list(APPEND HDR_PRIVATE
        server.h
        D:/libs/libuv/include/uv.h
        D:/libs/libwebsockets/lib/libwebsockets.h)
else()
    list(APPEND HDR_PRIVATE
        server.h
        /usr/include/uv.h
        /usr/include/libwebsockets.h
    )
endif()

#list(APPEND SOURCES
#        D:/libs/libwebsockets/lib/libuv.c
#        D:/libs/libwebsockets/lib/lejp.c
#        D:/libs/libwebsockets/lib/lejp-conf.c)

message("Sources: ${SOURCES}")

set(SOURCE_FILES main.cpp ${SOURCES} ${HDR_PRIVATE} test-server-http.c.cpp)
#set(SOURCE_FILES main.cpp ${SOURCES} ${HDR_PRIVATE})
add_executable(DungeonPuzzler ${SOURCE_FILES})

# Set test app specific defines.
set_property(TARGET DungeonPuzzler
        PROPERTY COMPILE_DEFINITIONS
        INSTALL_DATADIR="${CMAKE_INSTALL_PREFIX}/share"
        )
if (WIN32)
    include_directories("D:/libs/libuv/include")
    include_directories("D:/MinGW/include")
    include_directories("D:/libs/libwebsockets/lib")
    include_directories("D:/libs/libwebsockets/win32port/win32helpers")
    include_directories("C:/Program Files (x86)/OpenSSL-Win32/include")
    include_directories("D:/libs/libwebsockets/win32port/zlib")
    include_directories("D:/libs/libwebsockets/build")

    include_directories("D:/libs/libuv/include")


    target_link_libraries(DungeonPuzzler D:/libs/libwebsockets/build/lib/libwebsockets_static.a)
    target_link_libraries(DungeonPuzzler "D:/libs/libwebsockets/build/lib/libzlib_internal.a")

    target_link_libraries(DungeonPuzzler D:/MinGW/lib/libws2_32.a)
    target_link_libraries(DungeonPuzzler "C:/Program Files (x86)/OpenSSL-Win32/lib/MinGW/libssl-1_1.a")
    target_link_libraries(DungeonPuzzler "C:/Program Files (x86)/OpenSSL-Win32/lib/MinGW/libcrypto-1_1.a")

    target_link_libraries(DungeonPuzzler "D:/libs/libuv/build/Debug/liblibuv.a")
else()
    include_directories("/usr/include")
    include_directories(" /usr/local/include")
endif()

###
###
###
if (WIN32)
    add_library(win32port STATIC ${SOURCES} ${HDR_PRIVATE})
    target_link_libraries(DungeonPuzzler win32port)
    add_dependencies(DungeonPuzzler win32port)
endif()
##
## End of Add Helper files
##
