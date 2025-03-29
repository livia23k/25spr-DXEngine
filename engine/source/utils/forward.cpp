#include "utils/forward.h"
#include <format>

std::string MatrixToString(const DirectX::XMMATRIX &matrix)
{
    // print in col-major
    return std::format(
        "(col-major) \n[{:.3f}, {:.3f}, {:.3f}, {:.3f}]\n"
        "[{:.3f}, {:.3f}, {:.3f}, {:.3f}]\n"
        "[{:.3f}, {:.3f}, {:.3f}, {:.3f}]\n"
        "[{:.3f}, {:.3f}, {:.3f}, {:.3f}]",
        matrix.r[0].m128_f32[0], matrix.r[1].m128_f32[0], matrix.r[2].m128_f32[0], matrix.r[3].m128_f32[0],
        matrix.r[0].m128_f32[1], matrix.r[1].m128_f32[1], matrix.r[2].m128_f32[1], matrix.r[3].m128_f32[1],
        matrix.r[0].m128_f32[2], matrix.r[1].m128_f32[2], matrix.r[2].m128_f32[2], matrix.r[3].m128_f32[2],
        matrix.r[0].m128_f32[3], matrix.r[1].m128_f32[3], matrix.r[2].m128_f32[3], matrix.r[3].m128_f32[3]);
}
