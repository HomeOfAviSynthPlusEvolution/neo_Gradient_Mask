#include <numeric>
#include <type_traits>
#include <ds_common.hpp>
#include "source_core.h"

template <typename T>
void fill_plane_C(unsigned char * dst_ptr0, int dst_stride_bytes, DSVideoInfo vi, float color)
{
  T * dst_ptr = reinterpret_cast<T *>(dst_ptr0);
  int dst_stride = dst_stride_bytes / vi.Format.BytesPerSample;
  if constexpr (std::is_same<T, uint16_t>::value) {
    color = (color * 0x101) / (1 << (16 - vi.Format.BitsPerSample));
  }
  int limit = (1 << vi.Format.BitsPerSample) - 1;
  for (int h = 0; h < vi.Height; h++)
  {
    for (int w = 0; w < vi.Width; w++)
    {
      int c = static_cast<int>((w * limit * 1.0 / vi.Width) + color);
      dst_ptr[w] = std::min(c, limit);
    }
    dst_ptr += dst_stride;
  }
};

template void fill_plane_C<uint8_t>(unsigned char * dst_ptr, int dst_stride_bytes, DSVideoInfo vi, float color);
template void fill_plane_C<uint16_t>(unsigned char * dst_ptr, int dst_stride_bytes, DSVideoInfo vi, float color);
