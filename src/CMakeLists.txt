add_subdirectory(common)

set(LIB_COMMON
        common_utils
)

set(PUZZLE_LIBS
        ${LIB_COMMON}

    PARENT_SCOPE
)