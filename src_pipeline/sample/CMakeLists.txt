SET(NAME test)
SET(SAMPLE_PREFIX sample-)
SET(SAMPLE_DEPLIB imp)

SET(SAMPLE_NAME ${SAMPLE_PREFIX}${NAME})
SET(${SAMPLE_NAME}_SRCS ${SAMPLE_NAME}.c)

ADD_EXECUTABLE(${SAMPLE_NAME}-uclibc ${${SAMPLE_NAME}_SRCS})
SET_TARGET_PROPERTIES(${SAMPLE_NAME}-uclibc PROPERTIES
        OUTPUT_NAME "${SAMPLE_NAME}"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/uclibc
        LINK_FLAGS "-muclibc -ldl"
        COMPILE_FLAGS "-muclibc -ldl"
        )
ADD_DEPENDENCIES(${SAMPLE_NAME}-uclibc ${SAMPLE_DEPLIB}_static_uclibc)
TARGET_LINK_LIBRARIES(${SAMPLE_NAME}-uclibc
        ${PROJECT_BINARY_DIR}/lib-uclibc/lib${SAMPLE_DEPLIB}.a
        pthread m rt stdc++
        )

# target_link_libraries(main ${SAMPLE_DEPLIB})

