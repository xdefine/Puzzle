add_library(common_utils "")

target_sources(common_utils
        PRIVATE
        ${PUZZLE_SRC_DIR}/common/utils/assert.cpp
        ${PUZZLE_SRC_DIR}/common/utils/log.cpp
        PUBLIC
        ${PUZZLE_SRC_INCLUDE_DIR}/common/utils/config.h
        ${PUZZLE_SRC_INCLUDE_DIR}/common/utils/spin_lock.h
        ${PUZZLE_SRC_INCLUDE_DIR}/common/utils/assert.h
        ${PUZZLE_SRC_INCLUDE_DIR}/common/utils/log.h
)