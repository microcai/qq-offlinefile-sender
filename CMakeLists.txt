cmake_minimum_required(VERSION 2.8.9)
project(QQOfflineFileSender)

# ============== cpack ===============
set(CPACK_GENERATOR "TBZ2;DEB;RPM")
set(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QQOfflineFileSender")
SET(CPACK_PACKAGE_VENDOR "avplayer.org")
SET(CPACK_PACKAGE_VERSION_MAJOR "8")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
if(WIN32)
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY "QQOfflineFileSender")
else()
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY "/")
endif()

IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "QQOfflineFileSender.exe")
  SET(CPACK_NSIS_CONTACT "microcaicai@gmail.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "QQOfflineFileSender")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "QQOfflineFileSender" "QQOfflineFileSender")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "mosir") #required

SET(CPACK_SOURCE_IGNORE_FILES "${CPACK_SOURCE_IGNORE_FILES};${CMAKE_CURRENT_BINARY_DIR};\\\\.git;\\\\.kdev.*;\\\\.travis\\\\.yml")
INCLUDE(CPack)

# =================================================
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
option(ENABLE_STATIC "link to boost staticaly" ON)
option(ENABLE_TEST "build xmpp/pop3 test" OFF)
option(ENABLE_CXX11 "build QQOfflineFileSender with cxx11" OFF)
option(ENABLE_LTO "build QQOfflineFileSender with LTO" OFF)
option(ENABLE_LIBSTDCXX_STATIC "link to static libstdc++" OFF)
option(ENABLE_LIBGCC_STATIC "link to static libgcc" OFF)
option(WITH_ICONV "boost is build with iconv" ON)
option(WITH_ICU "boost is build with icu " OFF)

if(WIN32)
set(ENABLE_LIBGCC_STATIC ON)
set(ENABLE_LIBSTDCXX_STATIC ON)
set(ENABLE_STATIC ON)
endif()



if(WIN32)
add_definitions(-D_WIN32_WINNT=0x0501 -DWINVER=0x0501)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g0")
link_directories(${PROJECT_SOURCE_DIR}/openssl/lib)
link_directories(${PROJECT_SOURCE_DIR}/openssl/lib64)
include_directories( ${PROJECT_SOURCE_DIR}/openssl/include)
link_directories(${PROJECT_SOURCE_DIR}/libsformingw32/mingw32)
link_directories(${PROJECT_SOURCE_DIR}/libsformingw32/mingw64)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g0")

set(CMAKE_C_FLAGS_TEST "-Og -g")
set(CMAKE_CXX_FLAGS_TEST "-Og -g0")

if(CMAKE_BUILD_TYPE EQUAL "TEST")
set(ENABLE_STATIC ON)
endif()

if(ENABLE_STATIC)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_STATIC_RUNTIME    ON)
endif()

if(ENABLE_LIBSTDCXX_STATIC OR WIN32)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ ")
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_STATIC_RUNTIME    ON)
endif()

if(ENABLE_LIBGCC_STATIC OR WIN32)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc ")
endif()

if(ENABLE_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
if(ENABLE_LTO)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -flto -Wl,-flto")
endif()

#find_package(Qt5Core REQUIRED)
#find_package(Qt5Gui REQUIRED)
#find_package(Qt5Widgets REQUIRED)

find_package(Qt4 REQUIRED)
find_package(Boost 1.54 REQUIRED COMPONENTS log date_time filesystem system regex locale thread)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

# add_definitions(-DBOOST_LOG_NO_THREADS)
if(NOT WIN32)
find_package(Threads)
else()
set(CMAKE_THREAD_LIBS_INIT "-lkernel32")
endif()

if(WIN32)
add_definitions(-DBOOST_THREAD_USE_LIB)
add_definitions(-DBOOST_FILESYSTEM_STATIC_LINK)
add_definitions(-DBOOST_ALL_STATIC_LINK)
add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

find_package(OpenSSL)

add_definitions(-DAVHTTP_ENABLE_OPENSSL -DAVHTTP_DISABLE_MULTI_DOWNLOAD)

add_subdirectory(sqlite)

set(SQLITE3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/sqlite/include/")
set(SQLITE3_LIBRARIES sqlite3)

file(GLOB SOCI_CORE_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} soci/src/core/*.cpp)

add_library(soci_core STATIC ${SOCI_CORE_SOURCES})

file(GLOB SOCI_SQLITE3_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} soci/src/backends/sqlite3/*.cpp)
add_library(soci_sqlite3 STATIC ${SOCI_SQLITE3_SOURCES})

if(NOT WIN32)
  set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/soci/src/cmake ${CMAKE_MODULE_PATH})
  set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/soci/src/cmake/modules ${CMAKE_MODULE_PATH})
  set(DL_FIND_QUIETLY TRUE)
  find_package(DL)
  if(DL_FOUND)
    include_directories(${DL_INCLUDE_DIR})
    target_link_libraries(soci_core ${DL_LIBRARY})
  endif()
endif()

add_definitions(-DSOCI_LIB_PREFIX="" -DSOCI_LIB_SUFFIX="")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(soci/src/core)
include_directories(soci/src/backends/sqlite3)
include_directories(${SQLITE3_INCLUDE_DIR})

target_link_libraries(soci_sqlite3 soci_core ${SQLITE3_LIBRARIES})

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(avboost/include)
include_directories(avhttp/include)

add_subdirectory(libwebqq)

set(QQOFFLINE_FILE_SENDER_SOURCES buddymodel.cpp
	main.cpp
	dialog.cpp
	askvc.cpp
)

QT4_WRAP_UI(UI_HEADERS dialog.ui askvc.ui)
#qt4_automoc(${QQOFFLINE_FILE_SENDER_SOURCES})

add_executable(QQOfflineFileSender ${UI_HEADERS} ${QQOFFLINE_FILE_SENDER_SOURCES})

set_target_properties(QQOfflineFileSender PROPERTIES AUTOMOC TRUE)


target_link_libraries(QQOfflineFileSender ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

target_link_libraries(QQOfflineFileSender ${OPENSSL_LIBRARIES} )

target_link_libraries(QQOfflineFileSender ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(QQOfflineFileSender webqq)

if( NOT Boost_USE_STATIC_RUNTIME )
	message(STATUS "dynamic link to boost is not recomanded")
endif()

check_library_exists(rt clock_gettime "" HAVE_CLOCK_GETTIME)
if(HAVE_CLOCK_GETTIME)
        target_link_libraries(QQOfflineFileSender -lrt)
endif(HAVE_CLOCK_GETTIME)
