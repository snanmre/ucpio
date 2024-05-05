
set(S-LIB ucpio)

add_library(${S-LIB}
    STATIC
    ucpio-priv.h
    ucpio.c
    ucpio_entry.c
)

add_library(ucpio::ucpio ALIAS ${S-LIB})

target_compile_options(${S-LIB} PRIVATE -Wall -Wpedantic -Wextra)
target_include_directories(${S-LIB} PUBLIC ../include)