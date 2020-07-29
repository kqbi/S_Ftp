INCLUDE_DIRECTORIES("../../include")
INCLUDE_DIRECTORIES("./")

aux_source_directory(. CLIENTTEST_DIR)

add_executable(ClientTest ${CLIENTTEST_DIR})

if (WIN32)
    target_link_libraries(ClientTest ${CMAKE_PROJECT_NAME}Client_static ${LINK_LIB_LIST} winmm  wmcodecdspuuid dmoguids secur32 msdmo amstrmid)
else (WIN32)
    target_link_libraries(ClientTest ${CMAKE_PROJECT_NAME}Client_static ${LINK_LIB_LIST} uuid pthread dl stdc++ m crypt)
endif (WIN32)
