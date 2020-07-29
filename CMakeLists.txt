project(S_Ftp)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Release")
set(BUILD_USE_64BITS on)
set(platform x64)

set(LINK_LIB_LIST)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(system linux)
    set(THIRDPARTY "/home/vmns/thirdparty/Linux")
    #set(BOOST_DATE_TIME_LIB libboost_date_time.a)
    #set(BOOST_THREAD_LIB libboost_thread.a)
    set(OXF_LIB liboxf.a)
    set(WSC_LIB libS_WebSocketClient.a)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(system win)
    set(THIRDPARTY "D:/work/projects/3rdparty")
    #set(BOOST_DATE_TIME_LIB libboost_date_time-vc141-mt-s-x64-1_72.lib)
    #set(BOOST_THREAD_LIB libboost_thread-vc141-mt-s-x64-1_72.lib)
    set(OXF_LIB oxf-mt.lib)
    set(WSC_LIB S_WebSocketClient-mt.lib)
    set(JSONCPP_LIB libjson-mt.lib)
    set(WEBRTC_LIB webrtc-mt.lib)
endif ()

#加载自定义模块
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#设置库文件路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
#设置可执行程序路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#设置子目录

#安装目录
set(INSTALL_PATH_LIB ${THIRDPARTY}/${CMAKE_PROJECT_NAME}/lib/${platform})
set(INSTALL_PATH_INCLUDE ${THIRDPARTY}/${CMAKE_PROJECT_NAME}/include)

#安装头文件至系统目录
install(DIRECTORY src DESTINATION ${INSTALL_PATH_INCLUDE} FILES_MATCHING PATTERN "*.h")

#oxf
set(OXF_ROOT_PATH ${THIRDPARTY}/oxf)
set(OXF_INCLUDE_PATH ${OXF_ROOT_PATH}/include)
set(OXF_LIB_PATH ${OXF_ROOT_PATH}/lib/${system}/${platform})
set(OXF_LIB ${OXF_LIB_PATH}/${OXF_LIB})
INCLUDE_DIRECTORIES("${OXF_INCLUDE_PATH}")
link_directories(${OXF_LIB_PATH})
list(APPEND LINK_LIB_LIST ${OXF_LIB})

#boost
set(BOOST_ROOT_PATH ${THIRDPARTY}/boost)
set(BOOST_INCLUDE_PATH ${BOOST_ROOT_PATH}/include)
set(BOOST_LIB_PATH ${BOOST_ROOT_PATH}/lib/${system}/${platform})
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_PATH})
link_directories(${BOOST_LIB_PATH})

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:sizedDealloc- /MP")
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} /sdl /W3 /MTd /Od /ZI")
    set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} /MT /O2")
   # set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} /sdl /W3 /MT /Od /ZI /DEBUG")
else (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fpermissive")
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb")
    set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")
endif (WIN32)

ADD_DEFINITIONS("-DS_FTP_CLIENT_USE_STATIC_LIB")
if (WIN32)
    ADD_DEFINITIONS("-DNOMINMAX")
    ADD_DEFINITIONS("-DWIN32_LEAN_AND_MEAN")
endif (WIN32)

add_subdirectory(src)