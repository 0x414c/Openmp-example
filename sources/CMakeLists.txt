include (GCCCVars)

## -----------------------------------------------------------------------------

set (_TARGET_SOURCES
  main.c
)

add_executable (${_TARGET_NAME} ${_TARGET_SOURCES})

## -----------------------------------------------------------------------------

set_property (TARGET ${_TARGET_NAME} PROPERTY C_STANDARD 99)
set_property (TARGET ${_TARGET_NAME} PROPERTY C_STANDARD_REQUIRED TRUE)
set_property (TARGET ${_TARGET_NAME} PROPERTY C_EXTENSIONS TRUE)

set (_TARGET_COMPILE_OPTIONS
  -pipe

  -march=native
  -m64
)

set (_TARGET_COMPILE_OPTIONS_DEBUG
  -g
  -Og
)

target_compile_options (${_TARGET_NAME}
  PRIVATE
    ${_TARGET_COMPILE_OPTIONS}
    ${_TARGET_COMPILE_OPTIONS_DEBUG}
    ${_GCC_C_WARNINGS}
    ${_GCC_C_WARNINGS_2}
    ${_GCC_C_WARNINGS_3}
    ${_GCC_C_FP_SSE}
)

## -----------------------------------------------------------------------------

set (_TARGET_COMPILE_DEFINITIONS
  INPUT=2
  METHOD=2
  WITH_OMP
)

target_compile_definitions (${_TARGET_NAME}
  PRIVATE
    ${_TARGET_COMPILE_DEFINITIONS}
)

## -----------------------------------------------------------------------------

set (_TARGET_LINK_OPTIONS
  -fopenmp

  -static
  -static-libgcc
)

target_link_options(${_TARGET_NAME}
  PRIVATE
    ${_TARGET_LINK_OPTIONS}
)
