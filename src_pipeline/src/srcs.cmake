MESSAGE(STATUS "Set Libimp sources")

#SET(CMAKE_C_FLAGS " -fPIC -O2 -mmsa -march=mips32r2")
#SET(CMAKE_CXX_FLAGS "-fPIC -O2 -mmsa -march=mips32r2")
#
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -EL  -flax-vector-conversions")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -EL -std=c++11  -flax-vector-conversions -fpermissive")

#Add core sources
FILE(GLOB_RECURSE CORE_SRCS "core/*.c" "core/*.h")

SET(LIBIMP_SRCS ${CORE_SRCS})
#SET(LIBIMP_SRCS core/module.c)

#ADD_LIBRARY(imp_static_glibc STATIC ${LIBIMP_SRCS})
#ADD_LIBRARY(imp_shared_glibc SHARED ${LIBIMP_SRCS})

INCLUDE_DIRECTORIES(include)


#SET_TARGET_PROPERTIES(imp_static_glibc PROPERTIES
#    OUTPUT_NAME "imp"
#    LINK_FLAGS "-lpthread -ldl"
#    ARCHIVE_OUTPUT_DIRECTORY bin/lib-glibc
#)
#
#SET_TARGET_PROPERTIES(imp_shared_glibc PROPERTIES
#    OUTPUT_NAME "imp"
#    LINK_FLAGS "-lpthread -ldl"
#    LIBRARY_OUTPUT_DIRECTORY bin/lib-glibc
#)


