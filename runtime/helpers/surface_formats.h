/*
 * Copyright (C) 2017-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "CL/cl.h"

#ifndef SUPPORT_YUV
#define SUPPORT_YUV 1
#include "CL/cl_ext.h"
#endif

#include "core/utilities/arrayref.h"
#include "runtime/gmm_helper/gmm_lib.h"

namespace NEO {
enum GFX3DSTATE_SURFACEFORMAT : unsigned short {
    GFX3DSTATE_SURFACEFORMAT_R32G32B32A32_FLOAT = 0x000,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32A32_SINT = 0x001,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32A32_UINT = 0x002,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32A32_UNORM = 0x003,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32A32_SNORM = 0x004,
    GFX3DSTATE_SURFACEFORMAT_R64G64_FLOAT = 0x005,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32X32_FLOAT = 0x006,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32A32_SSCALED = 0x007,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32A32_USCALED = 0x008,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32_FLOAT = 0x040,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32_SINT = 0x041,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32_UINT = 0x042,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32_UNORM = 0x043,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32_SNORM = 0x044,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32_SSCALED = 0x045,
    GFX3DSTATE_SURFACEFORMAT_R32G32B32_USCALED = 0x046,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16A16_UNORM = 0x080,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16A16_SNORM = 0x081,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16A16_SINT = 0x082,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16A16_UINT = 0x083,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16A16_FLOAT = 0x084,
    GFX3DSTATE_SURFACEFORMAT_R32G32_FLOAT = 0x085,
    GFX3DSTATE_SURFACEFORMAT_R32G32_SINT = 0x086,
    GFX3DSTATE_SURFACEFORMAT_R32G32_UINT = 0x087,
    GFX3DSTATE_SURFACEFORMAT_R32_FLOAT_X8X24_TYPELESS = 0x088,
    GFX3DSTATE_SURFACEFORMAT_X32_TYPELESS_G8X24_UINT = 0x089,
    GFX3DSTATE_SURFACEFORMAT_L32A32_FLOAT = 0x08A,
    GFX3DSTATE_SURFACEFORMAT_R32G32_UNORM = 0x08B,
    GFX3DSTATE_SURFACEFORMAT_R32G32_SNORM = 0x08C,
    GFX3DSTATE_SURFACEFORMAT_R64_FLOAT = 0x08D,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16X16_UNORM = 0x08E,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16X16_FLOAT = 0x08F,
    GFX3DSTATE_SURFACEFORMAT_A32X32_FLOAT = 0x090,
    GFX3DSTATE_SURFACEFORMAT_L32X32_FLOAT = 0x091,
    GFX3DSTATE_SURFACEFORMAT_I32X32_FLOAT = 0x092,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16A16_SSCALED = 0x093,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16A16_USCALED = 0x094,
    GFX3DSTATE_SURFACEFORMAT_R32G32_SSCALED = 0x095,
    GFX3DSTATE_SURFACEFORMAT_R32G32_USCALED = 0x096,
    GFX3DSTATE_SURFACEFORMAT_B8G8R8A8_UNORM = 0x0C0,
    GFX3DSTATE_SURFACEFORMAT_B8G8R8A8_UNORM_SRGB = 0x0C1,
    GFX3DSTATE_SURFACEFORMAT_R10G10B10A2_UNORM = 0x0C2,
    GFX3DSTATE_SURFACEFORMAT_R10G10B10A2_UNORM_SRGB = 0x0C3,
    GFX3DSTATE_SURFACEFORMAT_R10G10B10A2_UINT = 0x0C4,
    GFX3DSTATE_SURFACEFORMAT_R10G10B10_SNORM_A2_UNORM = 0x0C5,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8A8_UNORM = 0x0C7,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8A8_UNORM_SRGB = 0x0C8,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8A8_SNORM = 0x0C9,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8A8_SINT = 0x0CA,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8A8_UINT = 0x0CB,
    GFX3DSTATE_SURFACEFORMAT_R16G16_UNORM = 0x0CC,
    GFX3DSTATE_SURFACEFORMAT_R16G16_SNORM = 0x0CD,
    GFX3DSTATE_SURFACEFORMAT_R16G16_SINT = 0x0CE,
    GFX3DSTATE_SURFACEFORMAT_R16G16_UINT = 0x0CF,
    GFX3DSTATE_SURFACEFORMAT_R16G16_FLOAT = 0x0D0,
    GFX3DSTATE_SURFACEFORMAT_B10G10R10A2_UNORM = 0x0D1,
    GFX3DSTATE_SURFACEFORMAT_B10G10R10A2_UNORM_SRGB = 0x0D2,
    GFX3DSTATE_SURFACEFORMAT_R11G11B10_FLOAT = 0x0D3,
    GFX3DSTATE_SURFACEFORMAT_R32_SINT = 0x0D6,
    GFX3DSTATE_SURFACEFORMAT_R32_UINT = 0x0D7,
    GFX3DSTATE_SURFACEFORMAT_R32_FLOAT = 0x0D8,
    GFX3DSTATE_SURFACEFORMAT_R24_UNORM_X8_TYPELESS = 0x0D9,
    GFX3DSTATE_SURFACEFORMAT_X24_TYPELESS_G8_UINT = 0x0DA,
    GFX3DSTATE_SURFACEFORMAT_L16A16_UNORM = 0x0DF,
    GFX3DSTATE_SURFACEFORMAT_I24X8_UNORM = 0x0E0,
    GFX3DSTATE_SURFACEFORMAT_L24X8_UNORM = 0x0E1,
    GFX3DSTATE_SURFACEFORMAT_A24X8_UNORM = 0x0E2,
    GFX3DSTATE_SURFACEFORMAT_I32_FLOAT = 0x0E3,
    GFX3DSTATE_SURFACEFORMAT_L32_FLOAT = 0x0E4,
    GFX3DSTATE_SURFACEFORMAT_A32_FLOAT = 0x0E5,
    GFX3DSTATE_SURFACEFORMAT_B8G8R8X8_UNORM = 0x0E9,
    GFX3DSTATE_SURFACEFORMAT_B8G8R8X8_UNORM_SRGB = 0x0EA,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8X8_UNORM = 0x0EB,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8X8_UNORM_SRGB = 0x0EC,
    GFX3DSTATE_SURFACEFORMAT_R9G9B9E5_SHAREDEXP = 0x0ED,
    GFX3DSTATE_SURFACEFORMAT_B10G10R10X2_UNORM = 0x0EE,
    GFX3DSTATE_SURFACEFORMAT_L16A16_FLOAT = 0x0F0,
    GFX3DSTATE_SURFACEFORMAT_R32_UNORM = 0x0F1,
    GFX3DSTATE_SURFACEFORMAT_R32_SNORM = 0x0F2,
    GFX3DSTATE_SURFACEFORMAT_R10G10B10X2_USCALED = 0x0F3,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8A8_SSCALED = 0x0F4,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8A8_USCALED = 0x0F5,
    GFX3DSTATE_SURFACEFORMAT_R16G16_SSCALED = 0x0F6,
    GFX3DSTATE_SURFACEFORMAT_R16G16_USCALED = 0x0F7,
    GFX3DSTATE_SURFACEFORMAT_R32_SSCALED = 0x0F8,
    GFX3DSTATE_SURFACEFORMAT_R32_USCALED = 0x0F9,
    GFX3DSTATE_SURFACEFORMAT_B5G6R5_UNORM = 0x100,
    GFX3DSTATE_SURFACEFORMAT_B5G6R5_UNORM_SRGB = 0x101,
    GFX3DSTATE_SURFACEFORMAT_B5G5R5A1_UNORM = 0x102,
    GFX3DSTATE_SURFACEFORMAT_B5G5R5A1_UNORM_SRGB = 0x103,
    GFX3DSTATE_SURFACEFORMAT_B4G4R4A4_UNORM = 0x104,
    GFX3DSTATE_SURFACEFORMAT_B4G4R4A4_UNORM_SRGB = 0x105,
    GFX3DSTATE_SURFACEFORMAT_R8G8_UNORM = 0x106,
    GFX3DSTATE_SURFACEFORMAT_R8G8_SNORM = 0x107,
    GFX3DSTATE_SURFACEFORMAT_R8G8_SINT = 0x108,
    GFX3DSTATE_SURFACEFORMAT_R8G8_UINT = 0x109,
    GFX3DSTATE_SURFACEFORMAT_R16_UNORM = 0x10A,
    GFX3DSTATE_SURFACEFORMAT_R16_SNORM = 0x10B,
    GFX3DSTATE_SURFACEFORMAT_R16_SINT = 0x10C,
    GFX3DSTATE_SURFACEFORMAT_R16_UINT = 0x10D,
    GFX3DSTATE_SURFACEFORMAT_R16_FLOAT = 0x10E,
    GFX3DSTATE_SURFACEFORMAT_I16_UNORM = 0x111,
    GFX3DSTATE_SURFACEFORMAT_L16_UNORM = 0x112,
    GFX3DSTATE_SURFACEFORMAT_A16_UNORM = 0x113,
    GFX3DSTATE_SURFACEFORMAT_L8A8_UNORM = 0x114,
    GFX3DSTATE_SURFACEFORMAT_I16_FLOAT = 0x115,
    GFX3DSTATE_SURFACEFORMAT_L16_FLOAT = 0x116,
    GFX3DSTATE_SURFACEFORMAT_A16_FLOAT = 0x117,
    GFX3DSTATE_SURFACEFORMAT_L8A8_UNORM_SRGB = 0x118,
    GFX3DSTATE_SURFACEFORMAT_R5G5_SNORM_B6_UNORM = 0x119,
    GFX3DSTATE_SURFACEFORMAT_B5G5R5X1_UNORM = 0x11A,
    GFX3DSTATE_SURFACEFORMAT_B5G5R5X1_UNORM_SRGB = 0x11B,
    GFX3DSTATE_SURFACEFORMAT_R8G8_SSCALED = 0x11C,
    GFX3DSTATE_SURFACEFORMAT_R8G8_USCALED = 0x11D,
    GFX3DSTATE_SURFACEFORMAT_R16_SSCALED = 0x11E,
    GFX3DSTATE_SURFACEFORMAT_R16_USCALED = 0x11F,
    GFX3DSTATE_SURFACEFORMAT_R8_UNORM = 0x140,
    GFX3DSTATE_SURFACEFORMAT_R8_SNORM = 0x141,
    GFX3DSTATE_SURFACEFORMAT_R8_SINT = 0x142,
    GFX3DSTATE_SURFACEFORMAT_R8_UINT = 0x143,
    GFX3DSTATE_SURFACEFORMAT_A8_UNORM = 0x144,
    GFX3DSTATE_SURFACEFORMAT_I8_UNORM = 0x145,
    GFX3DSTATE_SURFACEFORMAT_L8_UNORM = 0x146,
    GFX3DSTATE_SURFACEFORMAT_P4A4_UNORM = 0x147,
    GFX3DSTATE_SURFACEFORMAT_A4P4_UNORM = 0x148,
    GFX3DSTATE_SURFACEFORMAT_R8_SSCALED = 0x149,
    GFX3DSTATE_SURFACEFORMAT_R8_USCALED = 0x14A,
    GFX3DSTATE_SURFACEFORMAT_P8_UNORM = 0x14B,
    GFX3DSTATE_SURFACEFORMAT_L8_UNORM_SRGB = 0x14C,
    GFX3DSTATE_SURFACEFORMAT_DXT1_RGB_SRGB = 0x180,
    GFX3DSTATE_SURFACEFORMAT_R1_UINT = 0x181,
    GFX3DSTATE_SURFACEFORMAT_YCRCB_NORMAL = 0x182,
    GFX3DSTATE_SURFACEFORMAT_YCRCB_SWAPUVY = 0x183,
    GFX3DSTATE_SURFACEFORMAT_P2_UNORM = 0x184,
    GFX3DSTATE_SURFACEFORMAT_BC1_UNORM = 0x186,
    GFX3DSTATE_SURFACEFORMAT_BC2_UNORM = 0x187,
    GFX3DSTATE_SURFACEFORMAT_BC3_UNORM = 0x188,
    GFX3DSTATE_SURFACEFORMAT_BC4_UNORM = 0x189,
    GFX3DSTATE_SURFACEFORMAT_BC5_UNORM = 0x18A,
    GFX3DSTATE_SURFACEFORMAT_BC1_UNORM_SRGB = 0x18B,
    GFX3DSTATE_SURFACEFORMAT_BC2_UNORM_SRGB = 0x18C,
    GFX3DSTATE_SURFACEFORMAT_BC3_UNORM_SRGB = 0x18D,
    GFX3DSTATE_SURFACEFORMAT_MONO8 = 0x18E,
    GFX3DSTATE_SURFACEFORMAT_YCRCB_SWAPUV = 0x18F,
    GFX3DSTATE_SURFACEFORMAT_YCRCB_SWAPY = 0x190,
    GFX3DSTATE_SURFACEFORMAT_DXT1_RGB = 0x191,
    GFX3DSTATE_SURFACEFORMAT_FXT1 = 0x192,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8_UNORM = 0x193,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8_SNORM = 0x194,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8_SSCALED = 0x195,
    GFX3DSTATE_SURFACEFORMAT_R8G8B8_USCALED = 0x196,
    GFX3DSTATE_SURFACEFORMAT_R64G64B64A64_FLOAT = 0x197,
    GFX3DSTATE_SURFACEFORMAT_R64G64B64_FLOAT = 0x198,
    GFX3DSTATE_SURFACEFORMAT_BC4_SNORM = 0x199,
    GFX3DSTATE_SURFACEFORMAT_BC5_SNORM = 0x19A,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16_FLOAT = 0x19B,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16_UNORM = 0x19C,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16_SNORM = 0x19D,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16_SSCALED = 0x19E,
    GFX3DSTATE_SURFACEFORMAT_R16G16B16_USCALED = 0x19F,
    GFX3DSTATE_SURFACEFORMAT_BC6H_SF16 = 0x1A1,
    GFX3DSTATE_SURFACEFORMAT_BC7_UNORM = 0x1A2,
    GFX3DSTATE_SURFACEFORMAT_BC7_UNORM_SRGB = 0x1A3,
    GFX3DSTATE_SURFACEFORMAT_BC6H_UF16 = 0x1A4,
    GFX3DSTATE_SURFACEFORMAT_NV12 = 0x1A5,
    GFX3DSTATE_SURFACEFORMAT_RAW = 0x1FF,
    NUM_GFX3DSTATE_SURFACEFORMATS
};

enum class OCLPlane {
    NO_PLANE = 0,
    PLANE_Y,
    PLANE_U,
    PLANE_V,
    PLANE_UV
};

struct SurfaceFormatInfo {
    cl_image_format OCLImageFormat;
    GMM_RESOURCE_FORMAT GMMSurfaceFormat;
    GFX3DSTATE_SURFACEFORMAT GenxSurfaceFormat;
    cl_uint GMMTileWalk;
    cl_uint NumChannels;
    cl_uint PerChannelSizeInBytes;
    size_t ImageElementSizeInBytes;
};

struct ImageInfo {
    const cl_image_desc *imgDesc;
    const SurfaceFormatInfo *surfaceFormat;
    size_t size;
    size_t rowPitch;
    size_t slicePitch;
    uint32_t qPitch;
    size_t offset;
    uint32_t xOffset;
    uint32_t yOffset;
    uint32_t yOffsetForUVPlane;
    GMM_YUV_PLANE_ENUM plane;
    uint32_t baseMipLevel;
    uint32_t mipCount;
    bool linearStorage;
    bool preferRenderCompression;
    bool useLocalMemory;
};

struct McsSurfaceInfo {
    uint32_t pitch;
    uint32_t qPitch;
    uint32_t multisampleCount;
};

class SurfaceFormats {
  private:
    static const SurfaceFormatInfo readOnlySurfaceFormats[];
    static const SurfaceFormatInfo writeOnlySurfaceFormats[];
    static const SurfaceFormatInfo readWriteSurfaceFormats[];
    static const SurfaceFormatInfo readOnlyDepthSurfaceFormats[];
    static const SurfaceFormatInfo readWriteDepthSurfaceFormats[];
#ifdef SUPPORT_YUV
    static const SurfaceFormatInfo packedYuvSurfaceFormats[];
    static const SurfaceFormatInfo planarYuvSurfaceFormats[];
#endif

  public:
    static ArrayRef<const SurfaceFormatInfo> readOnly() noexcept;
    static ArrayRef<const SurfaceFormatInfo> writeOnly() noexcept;
    static ArrayRef<const SurfaceFormatInfo> readWrite() noexcept;
    static ArrayRef<const SurfaceFormatInfo> packedYuv() noexcept;
    static ArrayRef<const SurfaceFormatInfo> planarYuv() noexcept;
    static ArrayRef<const SurfaceFormatInfo> readOnlyDepth() noexcept;
    static ArrayRef<const SurfaceFormatInfo> readWriteDepth() noexcept;

    static ArrayRef<const SurfaceFormatInfo> surfaceFormats(cl_mem_flags flags) noexcept;
    static ArrayRef<const SurfaceFormatInfo> surfaceFormats(cl_mem_flags flags, const cl_image_format *imageFormat) noexcept;
};

} // namespace NEO
