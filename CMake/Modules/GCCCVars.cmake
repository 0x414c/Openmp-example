message (STATUS GCCCVars)

## NOTE:  For GCC 9.2.

set (_GCC_C_WARNINGS
  -fdiagnostics-color=auto

  -Wpedantic
  -Wall
  -Wextra

  -Wdouble-promotion
  -Wformat=2
  -Wformat-overflow=2
  -Wformat-signedness
  -Wformat-truncation=2

  CACHE INTERNAL
  "Common GCC C warnings"
)

set (_GCC_C_WARNINGS_2
  -Wnull-dereference  ## -fdelete-null-pointer-checks.
  -Wimplicit-fallthrough=4
  -Wmissing-include-dirs
  -Wmissing-profile
  -Wshift-overflow=2
  -Wswitch-default
  -Wswitch-enum
  -Wunused-const-variable=2
  -Wstrict-aliasing=3  ## -fstrict-aliasing.
  -Wstrict-overflow=5  ## -fstrict-overflow.
  -Wstringop-overflow=4
  -Wstringop-truncation
  -Walloc-zero
  -Walloca
  -Warray-bounds=2  ## -ftree-vrp.
  -Wduplicated-branches
  -Wduplicated-cond
  -Wfloat-equal
  -Wshadow=global
  -Wshadow=local
  -Wunsafe-loop-optimizations  ## -funsafe-loop-optimizations.
  -Wundef
  -Wunused-macros
  -Wbad-function-cast
  -Wcast-qual
  -Wcast-align=strict
  -Wwrite-strings
  -Wconversion
  -Wdate-time
  -Wjump-misses-init
  -Wlogical-op
  -Waggregate-return
  # -Waggressive-loop-optimizations  ## -faggressive-loop-optimizations.
  -Wstrict-prototypes
  -Wold-style-definition
  # -Wmissing-prototypes
  -Wmissing-declarations
  # -Wopenmp-simd  ## -fsimd-cost-model=unlimited.
  -Wpadded
  -Wredundant-decls
  -Wrestrict
  -Wnested-externs
  -Winline
  -Winvalid-pch
  -Wvector-operation-performance
  -Wvla
  -Wdisabled-optimization
  -Wstack-protector  ## -fstack-protector.
  -Whsa

  CACHE INTERNAL
  "Extra GCC C warnings"
)

set (_GCC_C_OPTIONS_2
  -fdelete-null-pointer-checks
  -fstrict-aliasing
  -fstrict-overflow
  -ftree-vrp
  -funsafe-loop-optimizations
  # -faggressive-loop-optimizations
  # -fsimd-cost-model=unlimited
  -fstack-protector

  CACHE INTERNAL
  "Extra GCC C options"
)

set (_GCC_C_WARNINGS_3
  -Wsync-nand
  -Wsuggest-attribute=pure
  -Wsuggest-attribute=const
  -Wsuggest-attribute=noreturn
  -Wsuggest-attribute=format
  -Wmissing-format-attribute
  -Wsuggest-attribute=cold
  -Wsuggest-attribute=malloc
  -Wattribute-alias=2
  -Wtrampolines
  -Wpacked

  CACHE INTERNAL
  "Miscellaneous GCC C warnings"
)

set (_GCC_C_FP_SSE
  -ffp-contract=off
  -ffloat-store
  -frounding-math
  -fsignaling-nans
  -fno-fp-int-builtin-inexact

  -mfpmath=sse
  -msse2
  -mieee-fp
  -mno-fp-ret-in-387
  -mpc64

  CACHE INTERNAL
  "SSE FP GCC C options"
)

set (_GCC_C_FP_X87
  -fexcess-precision=standard
  -ffp-contract=off
  -frounding-math
  -fsignaling-nans
  -fno-fp-int-builtin-inexact

  -mfpmath=387
  -mieee-fp
  -mpc80

  CACHE INTERNAL
  "x87 FP GCC C options"
)
