INCLUDE_DIRECTORIES("../include")
INCLUDE_DIRECTORIES("../src/api")
INCLUDE_DIRECTORIES("../src/ftp")
INCLUDE_DIRECTORIES("../src/ftp/detail")
INCLUDE_DIRECTORIES("../src/Utils")

aux_source_directory(../src/api API_DIR)
aux_source_directory(../src/ftp FTP_DIR)
aux_source_directory(../src/ftp/detail DETAIL_DIR)
aux_source_directory(../src/Utils UTILS_DIR)

if (WIN32)
 #  ADD_DEFINITIONS("-DUSE_BUILTIN_SW_CODECS")
endif (WIN32)

#静态库
add_library(${CMAKE_PROJECT_NAME}Client_static STATIC ${API_DIR} ${DETAIL_DIR} ${FTP_DIR} ${UTILS_DIR})

set_target_properties(${CMAKE_PROJECT_NAME}Client_static PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}Client")
#安装静态库至系统目录
install(TARGETS ${CMAKE_PROJECT_NAME}Client_static ARCHIVE DESTINATION ${INSTALL_PATH_LIB})

add_subdirectory(test)
