// REQUIRES: powerpc-registered-target
// RUN: %clang_cc1 -triple powerpc-unknown-linux-gnu -target-feature +altivec -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,HARD
// RUN: %clang_cc1 -triple powerpc-unknown-linux-gnu -target-feature +altivec -msoft-float -mfloat-abi soft -emit-llvm -o - %s | FileCheck %s --check-prefixes=CHECK,SOFT

typedef int v4si __attribute__((vector_size(16)));

// IBM long double takes two FPRs and is 8-byte aligned in the overflow area.
// With soft-float it takes four GPRs, without aligning to an even register,
// and is only 4-byte aligned in the overflow area.
long double test_ldouble(__builtin_va_list ap) {
  return __builtin_va_arg(ap, long double);
}
// CHECK-LABEL: @test_ldouble(
// HARD:       [[FPRPTR:%.+]] = getelementptr inbounds nuw %struct.__va_list_tag, ptr {{%.+}}, i32 0, i32 1
// HARD-NEXT:  [[FPR:%.+]] = load i8, ptr [[FPRPTR]]
// HARD-NEXT:  icmp ult i8 [[FPR]], 7
// HARD:       mul i8 [[FPR]], 8
// HARD:       add i8 [[FPR]], 2
// HARD:       store i8 8, ptr [[FPRPTR]]
// HARD:       getelementptr inbounds i8, ptr %argp.cur, i32 7
// HARD-NEXT:  call ptr @llvm.ptrmask.p0.i32(ptr %{{.+}}, i32 -8)
// SOFT:       [[GPRPTR:%.+]] = getelementptr inbounds nuw %struct.__va_list_tag, ptr {{%.+}}, i32 0, i32 0
// SOFT-NEXT:  [[GPR:%.+]] = load i8, ptr [[GPRPTR]]
// SOFT-NEXT:  icmp ult i8 [[GPR]], 5
// SOFT:       mul i8 [[GPR]], 4
// SOFT:       add i8 [[GPR]], 4
// SOFT:       store i8 8, ptr [[GPRPTR]]
// SOFT-NOT:   @llvm.ptrmask
// CHECK:      getelementptr inbounds i8, ptr %{{.+}}, i32 16
// CHECK:      ret ppc_fp128

// AltiVec vectors never go in registers, and do not touch the GPR count.
v4si test_vector(__builtin_va_list ap) {
  return __builtin_va_arg(ap, v4si);
}
// CHECK-LABEL: @test_vector(
// CHECK:       br i1 false, label %using_regs, label %using_overflow
// CHECK:       using_overflow:
// CHECK-NOT:   store i8
// CHECK:       getelementptr inbounds i8, ptr %argp.cur, i32 15
// CHECK-NEXT:  call ptr @llvm.ptrmask.p0.i32(ptr %{{.+}}, i32 -16)
// CHECK-NOT:   store i8
// CHECK:       ret <4 x i32>
