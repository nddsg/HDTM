project(hdtm)
add_graphlab_executable(hdtm concurrent_hdtm.cpp concurrent_hdtm.hpp opts.cpp opts.hpp)
#TARGET_LINK_LIBRARIES(hdtm /usr/local/lib/libhiredis.so /usr/local/lib/libredis3m.so)
