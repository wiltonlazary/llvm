; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+avx2 | FileCheck %s --check-prefix=CHECK --check-prefix=AVX2
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+avx512f | FileCheck %s  --check-prefix=CHECK  --check-prefix=AVX512  --check-prefix=AVX512F
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+avx512f,+avx512vl | FileCheck %s  --check-prefix=CHECK  --check-prefix=AVX512  --check-prefix=AVX512VL

; fold (abs c1) -> c2
define <4 x i32> @combine_v4i32_abs_constant() {
; CHECK-LABEL: combine_v4i32_abs_constant:
; CHECK:       # BB#0:
; CHECK-NEXT:    vmovaps {{.*#+}} xmm0 = [0,1,3,2147483648]
; CHECK-NEXT:    retq
  %1 = call <4 x i32> @llvm.x86.ssse3.pabs.d.128(<4 x i32> <i32 0, i32 -1, i32 3, i32 -2147483648>)
  ret <4 x i32> %1
}

define <16 x i16> @combine_v16i16_abs_constant() {
; CHECK-LABEL: combine_v16i16_abs_constant:
; CHECK:       # BB#0:
; CHECK-NEXT:    vmovaps {{.*#+}} ymm0 = [0,1,1,3,3,7,7,255,255,4096,4096,32767,32767,32768,32768,0]
; CHECK-NEXT:    retq
  %1 = call <16 x i16> @llvm.x86.avx2.pabs.w(<16 x i16> <i16 0, i16 1, i16 -1, i16 3, i16 -3, i16 7, i16 -7, i16 255, i16 -255, i16 4096, i16 -4096, i16 32767, i16 -32767, i16 -32768, i16 32768, i16 65536>)
  ret <16 x i16> %1
}

; fold (abs (abs x)) -> (abs x)
define <8 x i16> @combine_v8i16_abs_abs(<8 x i16> %a) {
; CHECK-LABEL: combine_v8i16_abs_abs:
; CHECK:       # BB#0:
; CHECK-NEXT:    vpabsw %xmm0, %xmm0
; CHECK-NEXT:    retq
  %a1 = call <8 x i16> @llvm.x86.ssse3.pabs.w.128(<8 x i16> %a)
  %s2 = ashr <8 x i16> %a1, <i16 15, i16 15, i16 15, i16 15, i16 15, i16 15, i16 15, i16 15>
  %a2 = add <8 x i16> %a1, %s2
  %x2 = xor <8 x i16> %a2, %s2
  ret <8 x i16> %x2
}

define <32 x i8> @combine_v32i8_abs_abs(<32 x i8> %a) {
; CHECK-LABEL: combine_v32i8_abs_abs:
; CHECK:       # BB#0:
; CHECK-NEXT:    vpabsb %ymm0, %ymm0
; CHECK-NEXT:    retq
  %n1 = sub <32 x i8> zeroinitializer, %a
  %b1 = icmp slt <32 x i8> %a, zeroinitializer
  %a1 = select <32 x i1> %b1, <32 x i8> %n1, <32 x i8> %a
  %a2 = call <32 x i8> @llvm.x86.avx2.pabs.b(<32 x i8> %a1)
  ret <32 x i8> %a2
}

define <4 x i64> @combine_v4i64_abs_abs(<4 x i64> %a) {
; AVX2-LABEL: combine_v4i64_abs_abs:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpxor %xmm1, %xmm1, %xmm1
; AVX2-NEXT:    vpcmpgtq %ymm0, %ymm1, %ymm2
; AVX2-NEXT:    vpaddq %ymm2, %ymm0, %ymm0
; AVX2-NEXT:    vpxor %ymm2, %ymm0, %ymm0
; AVX2-NEXT:    vpcmpgtq %ymm0, %ymm1, %ymm1
; AVX2-NEXT:    vpaddq %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    vpxor %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512F-LABEL: combine_v4i64_abs_abs:
; AVX512F:       # BB#0:
; AVX512F-NEXT:    # kill: %YMM0<def> %YMM0<kill> %ZMM0<def>
; AVX512F-NEXT:    vpabsq %zmm0, %zmm0
; AVX512F-NEXT:    # kill: %YMM0<def> %YMM0<kill> %ZMM0<kill>
; AVX512F-NEXT:    retq
;
; AVX512VL-LABEL: combine_v4i64_abs_abs:
; AVX512VL:       # BB#0:
; AVX512VL-NEXT:    vpabsq %ymm0, %ymm0
; AVX512VL-NEXT:    retq
  %n1 = sub <4 x i64> zeroinitializer, %a
  %b1 = icmp slt <4 x i64> %a, zeroinitializer
  %a1 = select <4 x i1> %b1, <4 x i64> %n1, <4 x i64> %a
  %n2 = sub <4 x i64> zeroinitializer, %a1
  %b2 = icmp sgt <4 x i64> %a1, zeroinitializer
  %a2 = select <4 x i1> %b2, <4 x i64> %a1, <4 x i64> %n2
  ret <4 x i64> %a2
}

; fold (abs x) -> x iff not-negative
define <16 x i8> @combine_v16i8_abs_constant(<16 x i8> %a) {
; AVX2-LABEL: combine_v16i8_abs_constant:
; AVX2:       # BB#0:
; AVX2-NEXT:    vandps {{.*}}(%rip), %xmm0, %xmm0
; AVX2-NEXT:    retq
;
; AVX512F-LABEL: combine_v16i8_abs_constant:
; AVX512F:       # BB#0:
; AVX512F-NEXT:    vandps {{.*}}(%rip), %xmm0, %xmm0
; AVX512F-NEXT:    retq
;
; AVX512VL-LABEL: combine_v16i8_abs_constant:
; AVX512VL:       # BB#0:
; AVX512VL-NEXT:    vpand {{.*}}(%rip), %xmm0, %xmm0
; AVX512VL-NEXT:    retq
  %1 = insertelement <16 x i8> undef, i8 15, i32 0
  %2 = shufflevector <16 x i8> %1, <16 x i8> undef, <16 x i32> zeroinitializer
  %3 = and <16 x i8> %a, %2
  %4 = call <16 x i8> @llvm.x86.ssse3.pabs.b.128(<16 x i8> %3)
  ret <16 x i8> %4
}

define <8 x i32> @combine_v8i32_abs_pos(<8 x i32> %a) {
; CHECK-LABEL: combine_v8i32_abs_pos:
; CHECK:       # BB#0:
; CHECK-NEXT:    vpsrld $1, %ymm0, %ymm0
; CHECK-NEXT:    retq
  %1 = lshr <8 x i32> %a, <i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1>
  %2 = call <8 x i32> @llvm.x86.avx2.pabs.d(<8 x i32> %1)
  ret <8 x i32> %2
}

declare <16 x i8> @llvm.x86.ssse3.pabs.b.128(<16 x i8>) nounwind readnone
declare <4 x i32> @llvm.x86.ssse3.pabs.d.128(<4 x i32>) nounwind readnone
declare <8 x i16> @llvm.x86.ssse3.pabs.w.128(<8 x i16>) nounwind readnone

declare <32 x i8> @llvm.x86.avx2.pabs.b(<32 x i8>) nounwind readnone
declare <8 x i32> @llvm.x86.avx2.pabs.d(<8 x i32>) nounwind readnone
declare <16 x i16> @llvm.x86.avx2.pabs.w(<16 x i16>) nounwind readnone
