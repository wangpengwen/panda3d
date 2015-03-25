// Filename: convert_srgb.cxx
// Created by:  rdb (13Nov14)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "convert_srgb.h"

// Lookup tables for converting from unsigned char formats.
ALIGN_64BYTE const
unsigned char to_srgb8_table[256] = { 0x00, 0x0d, 0x16, 0x1c, 0x22, 0x26, 0x2a,
  0x2e, 0x32, 0x35, 0x38, 0x3b, 0x3d, 0x40, 0x42, 0x45, 0x47, 0x49, 0x4b, 0x4d,
  0x4f, 0x51, 0x53, 0x55, 0x56, 0x58, 0x5a, 0x5c, 0x5d, 0x5f, 0x60, 0x62, 0x63,
  0x65, 0x66, 0x68, 0x69, 0x6a, 0x6c, 0x6d, 0x6e, 0x70, 0x71, 0x72, 0x73, 0x75,
  0x76, 0x77, 0x78, 0x79, 0x7a, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83,
  0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90,
  0x91, 0x92, 0x93, 0x94, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9b,
  0x9c, 0x9d, 0x9e, 0x9f, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa3, 0xa4, 0xa5, 0xa6,
  0xa7, 0xa7, 0xa8, 0xa9, 0xaa, 0xaa, 0xab, 0xac, 0xad, 0xad, 0xae, 0xaf, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb2, 0xb3, 0xb4, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb9,
  0xb9, 0xba, 0xbb, 0xbb, 0xbc, 0xbd, 0xbd, 0xbe, 0xbe, 0xbf, 0xc0, 0xc0, 0xc1,
  0xc2, 0xc2, 0xc3, 0xc4, 0xc4, 0xc5, 0xc5, 0xc6, 0xc7, 0xc7, 0xc8, 0xc8, 0xc9,
  0xca, 0xca, 0xcb, 0xcb, 0xcc, 0xcd, 0xcd, 0xce, 0xce, 0xcf, 0xd0, 0xd0, 0xd1,
  0xd1, 0xd2, 0xd2, 0xd3, 0xd4, 0xd4, 0xd5, 0xd5, 0xd6, 0xd6, 0xd7, 0xd7, 0xd8,
  0xd8, 0xd9, 0xda, 0xda, 0xdb, 0xdb, 0xdc, 0xdc, 0xdd, 0xdd, 0xde, 0xde, 0xdf,
  0xdf, 0xe0, 0xe0, 0xe1, 0xe2, 0xe2, 0xe3, 0xe3, 0xe4, 0xe4, 0xe5, 0xe5, 0xe6,
  0xe6, 0xe7, 0xe7, 0xe8, 0xe8, 0xe9, 0xe9, 0xea, 0xea, 0xeb, 0xeb, 0xec, 0xec,
  0xed, 0xed, 0xee, 0xee, 0xee, 0xef, 0xef, 0xf0, 0xf0, 0xf1, 0xf1, 0xf2, 0xf2,
  0xf3, 0xf3, 0xf4, 0xf4, 0xf5, 0xf5, 0xf6, 0xf6, 0xf6, 0xf7, 0xf7, 0xf8, 0xf8,
  0xf9, 0xf9, 0xfa, 0xfa, 0xfb, 0xfb, 0xfb, 0xfc, 0xfc, 0xfd, 0xfd, 0xfe, 0xfe,
  0xff, 0xff};

ALIGN_64BYTE const
unsigned char to_linear_uchar_table[256] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x07,
  0x07, 0x07, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x0a, 0x0a, 0x0a, 0x0b,
  0x0b, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x10, 0x10,
  0x11, 0x11, 0x11, 0x12, 0x12, 0x13, 0x13, 0x14, 0x14, 0x15, 0x16, 0x16, 0x17,
  0x17, 0x18, 0x18, 0x19, 0x19, 0x1a, 0x1b, 0x1b, 0x1c, 0x1d, 0x1d, 0x1e, 0x1e,
  0x1f, 0x20, 0x20, 0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 0x25, 0x26, 0x27, 0x28,
  0x29, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,
  0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4c, 0x4d,
  0x4e, 0x4f, 0x50, 0x51, 0x52, 0x54, 0x55, 0x56, 0x57, 0x58, 0x5a, 0x5b, 0x5c,
  0x5d, 0x5f, 0x60, 0x61, 0x63, 0x64, 0x65, 0x67, 0x68, 0x69, 0x6b, 0x6c, 0x6d,
  0x6f, 0x70, 0x72, 0x73, 0x74, 0x76, 0x77, 0x79, 0x7a, 0x7c, 0x7d, 0x7f, 0x80,
  0x82, 0x83, 0x85, 0x86, 0x88, 0x8a, 0x8b, 0x8d, 0x8e, 0x90, 0x92, 0x93, 0x95,
  0x97, 0x98, 0x9a, 0x9c, 0x9d, 0x9f, 0xa1, 0xa3, 0xa4, 0xa6, 0xa8, 0xaa, 0xab,
  0xad, 0xaf, 0xb1, 0xb3, 0xb5, 0xb7, 0xb8, 0xba, 0xbc, 0xbe, 0xc0, 0xc2, 0xc4,
  0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde,
  0xe0, 0xe2, 0xe5, 0xe7, 0xe9, 0xeb, 0xed, 0xef, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa,
  0xfd, 0xff};

ALIGN_64BYTE
const float to_linear_float_table[256] = { 0, 0.000304f, 0.000607f, 0.000911f,
  0.001214f, 0.001518f, 0.001821f, 0.002125f, 0.002428f, 0.002732f, 0.003035f,
  0.003347f, 0.003677f, 0.004025f, 0.004391f, 0.004777f, 0.005182f, 0.005605f,
  0.006049f, 0.006512f, 0.006995f, 0.007499f, 0.008023f, 0.008568f, 0.009134f,
  0.009721f, 0.010330f, 0.010960f, 0.011612f, 0.012286f, 0.012983f, 0.013702f,
  0.014444f, 0.015209f, 0.015996f, 0.016807f, 0.017642f, 0.018500f, 0.019382f,
  0.020289f, 0.021219f, 0.022174f, 0.023153f, 0.024158f, 0.025187f, 0.026241f,
  0.027321f, 0.028426f, 0.029557f, 0.030713f, 0.031896f, 0.033105f, 0.034340f,
  0.035601f, 0.036889f, 0.038204f, 0.039546f, 0.040915f, 0.042311f, 0.043735f,
  0.045186f, 0.046665f, 0.048172f, 0.049707f, 0.051269f, 0.052861f, 0.054480f,
  0.056128f, 0.057805f, 0.059511f, 0.061246f, 0.063010f, 0.064803f, 0.066626f,
  0.068478f, 0.070360f, 0.072272f, 0.074214f, 0.076185f, 0.078187f, 0.080220f,
  0.082283f, 0.084376f, 0.086500f, 0.088656f, 0.090842f, 0.093059f, 0.095307f,
  0.097587f, 0.099899f, 0.102242f, 0.104616f, 0.107023f, 0.109462f, 0.111932f,
  0.114435f, 0.116971f, 0.119538f, 0.122139f, 0.124772f, 0.127438f, 0.130136f,
  0.132868f, 0.135633f, 0.138432f, 0.141263f, 0.144128f, 0.147027f, 0.149960f,
  0.152926f, 0.155926f, 0.158961f, 0.162029f, 0.165132f, 0.168269f, 0.171441f,
  0.174647f, 0.177888f, 0.181164f, 0.184475f, 0.187821f, 0.191202f, 0.194618f,
  0.198069f, 0.201556f, 0.205079f, 0.208637f, 0.212231f, 0.215861f, 0.219526f,
  0.223228f, 0.226966f, 0.230740f, 0.234551f, 0.238398f, 0.242281f, 0.246201f,
  0.250158f, 0.254152f, 0.258183f, 0.262251f, 0.266356f, 0.270498f, 0.274677f,
  0.278894f, 0.283149f, 0.287441f, 0.291771f, 0.296138f, 0.300544f, 0.304987f,
  0.309469f, 0.313989f, 0.318547f, 0.323143f, 0.327778f, 0.332452f, 0.337164f,
  0.341914f, 0.346704f, 0.351533f, 0.356400f, 0.361307f, 0.366253f, 0.371238f,
  0.376262f, 0.381326f, 0.386429f, 0.391572f, 0.396755f, 0.401978f, 0.407240f,
  0.412543f, 0.417885f, 0.423268f, 0.428690f, 0.434154f, 0.439657f, 0.445201f,
  0.450786f, 0.456411f, 0.462077f, 0.467784f, 0.473531f, 0.479320f, 0.485150f,
  0.491021f, 0.496933f, 0.502886f, 0.508881f, 0.514918f, 0.520996f, 0.527115f,
  0.533276f, 0.539479f, 0.545724f, 0.552011f, 0.558340f, 0.564712f, 0.571125f,
  0.577580f, 0.584078f, 0.590619f, 0.597202f, 0.603827f, 0.610496f, 0.617207f,
  0.623960f, 0.630757f, 0.637597f, 0.644480f, 0.651406f, 0.658375f, 0.665387f,
  0.672443f, 0.679542f, 0.686685f, 0.693872f, 0.701102f, 0.708376f, 0.715694f,
  0.723055f, 0.730461f, 0.737910f, 0.745404f, 0.752942f, 0.760525f, 0.768151f,
  0.775822f, 0.783538f, 0.791298f, 0.799103f, 0.806952f, 0.814847f, 0.822786f,
  0.830770f, 0.838799f, 0.846873f, 0.854993f, 0.863157f, 0.871367f, 0.879622f,
  0.887923f, 0.896269f, 0.904661f, 0.913099f, 0.921582f, 0.930111f, 0.938686f,
  0.947307f, 0.955973f, 0.964686f, 0.973445f, 0.982251f, 0.991102f, 1.000000f};


#if defined(__SSE2__) || (_M_IX86_FP >= 2) || defined(_M_X64) || defined(_M_AMD64)
// SSE2 support enabled at compile time.  No runtime detection mechanism needed.
bool
has_sse2_sRGB_encode() {
  return true;
}

#elif defined(__i386__) || defined(_M_IX86)
// SSE2 support not guaranteed.  Use a runtime detection mechanism.

#ifdef __GNUC__
#include <cpuid.h>
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

bool
has_sse2_sRGB_encode() {
#if defined(__GNUC__)
  unsigned int a, b, c, d;
  static const bool has_support =
    (__get_cpuid(1, &a, &b, &c, &d) == 1 && (d & 0x04000000) != 0);

#elif defined(_WIN32)
  static const bool has_support =
    (IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE) != FALSE);

#else
  static const bool has_support = false;
#endif

  if (pnmimage_cat.is_debug()) {
    static bool checked = false;
    if (!checked) {
#if defined(__GNUC__) || defined(_WIN32)
      if (has_support) {
        pnmimage_cat.debug()
          << "Runtime detection reports SSE2 instructions available: "
          << "SSE2-optimized sRGB encoding routines enabled.\n";
      } else {
        pnmimage_cat.debug()
          << "Runtime detection reports SSE2 instructions unavailable: "
          << "SSE2-optimized sRGB encoding routines disabled.\n";
      }
#else
      pnmimage_cat.debug()
        << "No runtime detection mechanism for SSE2 instructions available: "
        << "SSE2-optimized sRGB encoding routines disabled.\n";
#endif
      checked = true;
    }
  }

  return has_support;
}

#else
// Other architectures don't support SSE2 at all.

bool
has_sse2_sRGB_encode() {
  return false;
}

#endif  // __SSE2__
