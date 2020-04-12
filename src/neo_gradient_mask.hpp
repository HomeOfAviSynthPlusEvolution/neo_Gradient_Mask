/*
 * Copyright 2020 Xinyue Lu
 *
 * Gradient Mask SDK Sample.
 *
 */

#pragma once

#include <numeric>
#include <execution>
#include "source_core.h"
#include "version.hpp"

int GetCPUFlags(); // From cpuid.cpp

struct NGM final : Filter {
  InDelegator* _in;
  DSVideoInfo out_vi;
  int color {0x99ccff};
  float luma;

  void (*fill_plane)(unsigned char * dst_ptr, int dst_stride_bytes, DSVideoInfo vi, float color);


  const char* VSName() const override { return "GM"; }
  const char* AVSName() const override { return "neo_gm"; }
  const MtMode AVSMode() const override { return MT_NICE_FILTER; }
  const VSFilterMode VSMode() const override { return fmParallel; }
  const std::vector<Param> Params() const override {
    return std::vector<Param> {
      Param {"width", Integer},
      Param {"height", Integer},
      Param {"color", Integer},
      Param {"depth", Integer}
    };
  }
  void Initialize(InDelegator* in, DSVideoInfo in_vi, FetchFrameFunctor* fetch_frame) override
  {
    // in_vi and fetch_frame are useless for source filter
    Filter::Initialize(in, in_vi, fetch_frame);

    int width = 640, height = 480, depth = 8;

    in->Read("width", width);
    in->Read("height", height);
    in->Read("color", color);
    in->Read("depth", depth);
    luma = ((color && 0xFF) + ((color >> 8) & 0xFF) + ((color >> 16) & 0xFF)) / 3;

    #define INVALID_PARAM_IF(cond) \
    do { if (cond) { throw("Invalid parameter: " #cond); } } while (0)

    INVALID_PARAM_IF(width < 0);
    INVALID_PARAM_IF(height < 0);
    INVALID_PARAM_IF(depth < 8);
    INVALID_PARAM_IF(depth > 16);
    INVALID_PARAM_IF(depth % 2 == 1);

    auto out_format = DSFormat(VideoInfo::CS_RGBP);
    out_format.BitsPerSample = depth;
    out_format.BytesPerSample = depth == 8 ? 1 : 2;
    out_vi = { out_format, 30000LL, 1001LL, width, height, 10000 };

    int CPUFlags = GetCPUFlags();

    switch (out_format.BytesPerSample) {
      case 1: fill_plane = fill_plane_C<uint8_t>; break;
      case 2: fill_plane = fill_plane_C<uint16_t>; break;
    }

    // if (CPUFlags & CPUF_SSE4_1)
    //   fill_plane = fill_plane_SSE;
  }

  DSFrame GetFrame(int n, std::unordered_map<int, DSFrame> in_frames) override
  {
    // in_frames contain dummy DSFrame that has APIs but not frame data
    // Use out_vi to create a real frame
    auto dst = in_frames[n].Create(out_vi);

    // for (int p = 0; p < out_vi.Format.Planes; p++) {

    // std::execution::par_unseq for unsequenced parallel execution
    std::for_each_n(std::execution::par_unseq, reinterpret_cast<char*>(0), out_vi.Format.Planes, [&](char&idx) {
      int p = static_cast<int>(reinterpret_cast<intptr_t>(&idx));
      auto dst_stride = dst.StrideBytes[p];
      auto dst_ptr = dst.DstPointers[p];

      float color_component = 0.0f;
      switch(p) {
        case 0: color_component = (color >> 16) & 0xFF; break;
        case 1: color_component = (color >> 8) & 0xFF; break;
        case 2: color_component = color & 0xFF; break;
      }

      color_component = std::max(0.0f, color_component - luma);

      fill_plane(dst_ptr, dst_stride, out_vi, color_component);
    });

    return dst;
  }

  DSVideoInfo GetOutputVI()
  {
    return out_vi;
  }

  ~NGM() = default;
};


namespace Plugin {
  const char* Identifier = "in.7086.neo_gm";
  const char* Namespace = "neo_gm";
  const char* Description = "Neo Gradient Mask Deband Filter " PLUGIN_VERSION;
}

std::vector<register_vsfilter_proc> RegisterVSFilters()
{
  return std::vector<register_vsfilter_proc> { VSInterface::RegisterFilter<NGM> };
}

std::vector<register_avsfilter_proc> RegisterAVSFilters()
{
  return std::vector<register_avsfilter_proc> { AVSInterface::RegisterFilter<NGM> };
}
