message (STATUS GCCCXXVars)

## NOTE:  For GCC 9.2.

set (_GCC_CXX_WARNINGS
  -fdiagnostics-color=auto
  -fdiagnostics-show-template-tree

  -Wpedantic
  -Wall
  -Wextra

  -Wdouble-promotion
  -Wformat=2
  -Wformat-overflow=2
  -Wformat-signedness
  -Wformat-truncation=2

  CACHE INTERNAL
  "Common GCC CXX warnings"
)

set (_GCC_CXX_WARNINGS_2
  -Wnull-dereference  ## -fdelete-null-pointer-checks.
  -Wimplicit-fallthrough=5
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
  -Wsuggest-final-types
  -Wsuggest-final-methods
  -Wsuggest-override
  -Walloc-zero
  -Walloca
  -Warray-bounds=2  ## -ftree-vrp.
  -Wduplicated-branches
  -Wduplicated-cond
  -Wfloat-equal
  -Wshadow=global
  -Wshadow=local
  -Wunsafe-loop-optimizations  ## -funsafe-loop-optimizations.
  -Wplacement-new=2
  -Wundef
  -Wunused-macros
  -Wcast-qual
  -Wcast-align=strict
  -Wcatch-value=3
  -Wconditionally-supported
  -Wconversion
  -Wzero-as-null-pointer-constant
  -Wdate-time
  -Wuseless-cast
  -Wextra-semi
  -Wsign-conversion
  # -Wsized-deallocation  ## -fsized-deallocation.
  -Wlogical-op
  -Waggregate-return
  # -Waggressive-loop-optimizations  ## -faggressive-loop-optimizations.
  # -Wmissing-declarations
  # -Wopenmp-simd  ## -fsimd-cost-model=unlimited.
  -Wpadded
  -Wredundant-decls
  -Wrestrict
  -Winline
  -Winvalid-pch
  -Wvector-operation-performance
  -Wvla
  -Wdisabled-optimization
  -Wstack-protector  ## -fstack-protector.
  -Whsa

  -Wctor-dtor-privacy
  -Wnoexcept
  -Wnon-virtual-dtor
  -Wstrict-null-sentinel
  -Wold-style-cast
  -Woverloaded-virtual
  -Wsign-promo

  CACHE INTERNAL
  "Extra GCC CXX warnings"
)

set (_GCC_CXX_OPTIONS_2
  -fdelete-null-pointer-checks
  -fstrict-aliasing
  -fstrict-overflow
  -ftree-vrp
  -funsafe-loop-optimizations
  # -fsized-deallocation
  # -faggressive-loop-optimizations
  # -fsimd-cost-model=unlimited
  -fstack-protector

  CACHE INTERNAL
  "Extra GCC CXX options"
)

set (_GCC_CXX_WARNINGS_3
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

  # -Wabi=11
  -Wabi-tag

  CACHE INTERNAL
  "Miscellaneous GCC CXX warnings"
)

set (_GCC_CXX_FP_SSE
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
  "SSE FP GCC CXX options"
)

set (_GCC_CXX_FP_X87
  -ffp-contract=off
  -frounding-math
  -fsignaling-nans
  -fno-fp-int-builtin-inexact

  -mfpmath=387
  -mieee-fp
  -mpc80

  CACHE INTERNAL
  "x87 FP GCC CXX options"
)
