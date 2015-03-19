# libcommon
file(GLOB_RECURSE libcommon_srcs *.cpp *.c *.cc)

#message(STATUS ${libcommon_srcs})

build_static_lib(libutil
    ${libcommon_srcs}
    OUTPUT_NAME util
    OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib
    INCLUDE_DIRS ${PROJECT_DIR}/comm
    LIBS pthread tinyxml mysqlclient log4cpp hiredis protobuf
    DEPENDS /usr/local/lib/libsasl2.so
    )

