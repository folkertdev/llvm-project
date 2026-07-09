/// -fcomplex-ppc-gnu-abi is a 32-bit PowerPC ELF driver flag that lowers to the
/// +complex-in-gpr target feature.
// RUN: %clang -### -S --target=powerpc-unknown-linux -fcomplex-ppc-gnu-abi %s 2>&1 \
// RUN:     | FileCheck %s -check-prefix=CHECK-GNU

/// It is enabled by default for GNU environments.
// RUN: %clang -### -S --target=powerpc-unknown-linux-gnu %s 2>&1 \
// RUN:     | FileCheck %s -check-prefix=CHECK-GNU

// CHECK-GNU: "-target-feature" "+complex-in-gpr"

/// It is rejected on non-ppc32 or non-ELF targets.
// RUN: not %clang -### --target=powerpc64-unknown-linux -fcomplex-ppc-gnu-abi %s 2>&1 \
// RUN:     | FileCheck %s -check-prefix=CHECK-ERROR

// RUN: not %clang -### --target=powerpc-unknown-unknown-coff -fcomplex-ppc-gnu-abi %s 2>&1 \
// RUN:     | FileCheck %s -check-prefix=CHECK-ERROR

// CHECK-ERROR: error: unsupported option '-fcomplex-ppc-gnu-abi' for target
