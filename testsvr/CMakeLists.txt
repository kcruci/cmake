file(GLOB_RECURSE main_srcs *.cpp)

build_exe(pvp_match_svr
    ${main_srcs}
    OUTPUT_DIR ${ROOT_DIR}/bin/
    LIBS libutil
    DEPENDS libutil
    )
