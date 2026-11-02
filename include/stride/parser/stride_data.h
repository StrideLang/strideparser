#ifndef STRIDE_DATA_H
#define STRIDE_DATA_H

/**
 * @file stride_data.h
 * @brief Umbrella header for standalone, zero-dependency, header-only Stride data parsing.
 *
 * Includes:
 *   - strd::StrideReader (SAX streaming parser)
 *   - strd::StrideDocument (DOM parser)
 *   - strd::StrideValue (DOM value handle)
 *
 * Usage:
 *   #include "stride/parser/stride_data.h"
 *   // or just #include "stride_data.h" if placed in include root
 */

#if __has_include("stride/parser/stridedocument.h")
#include "stride/parser/stridereader.h"
#include "stride/parser/stridedocument.h"
#else
#include "stridereader.h"
#include "stridedocument.h"
#endif

#endif // STRIDE_DATA_H
