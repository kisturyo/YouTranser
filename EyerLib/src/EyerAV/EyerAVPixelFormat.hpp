#ifndef EYERLIB_EYERAVPIXELFORMAT_HPP
#define EYERLIB_EYERAVPIXELFORMAT_HPP

#include "EyerCore/EyerCore.hpp"

namespace Eyer
{
    class EyerAVPixelFormat
    {
    public:
        static EyerAVPixelFormat EYER_KEEP_SAME;
        static EyerAVPixelFormat EYER_NONE;
        static EyerAVPixelFormat EYER_YUV420P;
        static EyerAVPixelFormat EYER_YUYV422;
        static EyerAVPixelFormat EYER_RGB24;
        static EyerAVPixelFormat EYER_BGR24;
        static EyerAVPixelFormat EYER_YUV422P;
        static EyerAVPixelFormat EYER_YUV444P;
        static EyerAVPixelFormat EYER_YUV410P;
        static EyerAVPixelFormat EYER_YUV411P;
        static EyerAVPixelFormat EYER_GRAY8;
        static EyerAVPixelFormat EYER_MONOWHITE;
        static EyerAVPixelFormat EYER_MONOBLACK;
        static EyerAVPixelFormat EYER_PAL8;
        static EyerAVPixelFormat EYER_YUVJ420P;
        static EyerAVPixelFormat EYER_YUVJ422P;
        static EyerAVPixelFormat EYER_YUVJ444P;
        static EyerAVPixelFormat EYER_UYVY422;
        static EyerAVPixelFormat EYER_UYYVYY411;
        static EyerAVPixelFormat EYER_BGR8;
        static EyerAVPixelFormat EYER_BGR4;
        static EyerAVPixelFormat EYER_BGR4_BYTE;
        static EyerAVPixelFormat EYER_RGB8;
        static EyerAVPixelFormat EYER_RGB4;
        static EyerAVPixelFormat EYER_RGB4_BYTE;
        static EyerAVPixelFormat EYER_NV12;
        static EyerAVPixelFormat EYER_NV21;
        static EyerAVPixelFormat EYER_ARGB;
        static EyerAVPixelFormat EYER_RGBA;
        static EyerAVPixelFormat EYER_ABGR;
        static EyerAVPixelFormat EYER_BGRA;
        static EyerAVPixelFormat EYER_GRAY16BE;
        static EyerAVPixelFormat EYER_GRAY16LE;
        static EyerAVPixelFormat EYER_YUV440P;
        static EyerAVPixelFormat EYER_YUVJ440P;
        static EyerAVPixelFormat EYER_YUVA420P;
        static EyerAVPixelFormat EYER_RGB48BE;
        static EyerAVPixelFormat EYER_RGB48LE;
        static EyerAVPixelFormat EYER_RGB565BE;
        static EyerAVPixelFormat EYER_RGB565LE;
        static EyerAVPixelFormat EYER_RGB555BE;
        static EyerAVPixelFormat EYER_RGB555LE;
        static EyerAVPixelFormat EYER_BGR565BE;
        static EyerAVPixelFormat EYER_BGR565LE;
        static EyerAVPixelFormat EYER_BGR555BE;
        static EyerAVPixelFormat EYER_BGR555LE;
        static EyerAVPixelFormat EYER_YUV420P16LE;
        static EyerAVPixelFormat EYER_YUV420P16BE;
        static EyerAVPixelFormat EYER_YUV422P16LE;
        static EyerAVPixelFormat EYER_YUV422P16BE;
        static EyerAVPixelFormat EYER_YUV444P16LE;
        static EyerAVPixelFormat EYER_YUV444P16BE;
        static EyerAVPixelFormat EYER_RGB444LE;
        static EyerAVPixelFormat EYER_RGB444BE;
        static EyerAVPixelFormat EYER_BGR444LE;
        static EyerAVPixelFormat EYER_BGR444BE;
        static EyerAVPixelFormat EYER_YA8;
        static EyerAVPixelFormat EYER_BGR48BE;
        static EyerAVPixelFormat EYER_BGR48LE;
        static EyerAVPixelFormat EYER_YUV420P9BE;
        static EyerAVPixelFormat EYER_YUV420P9LE;
        static EyerAVPixelFormat EYER_YUV420P10BE;
        static EyerAVPixelFormat EYER_YUV420P10LE;
        static EyerAVPixelFormat EYER_YUV422P10BE;
        static EyerAVPixelFormat EYER_YUV422P10LE;
        static EyerAVPixelFormat EYER_YUV444P9BE;
        static EyerAVPixelFormat EYER_YUV444P9LE;
        static EyerAVPixelFormat EYER_YUV444P10BE;
        static EyerAVPixelFormat EYER_YUV444P10LE;
        static EyerAVPixelFormat EYER_YUV422P9BE;
        static EyerAVPixelFormat EYER_YUV422P9LE;
        static EyerAVPixelFormat EYER_GBRP;
        static EyerAVPixelFormat EYER_GBRP9BE;
        static EyerAVPixelFormat EYER_GBRP9LE;
        static EyerAVPixelFormat EYER_GBRP10BE;
        static EyerAVPixelFormat EYER_GBRP10LE;
        static EyerAVPixelFormat EYER_GBRP16BE;
        static EyerAVPixelFormat EYER_GBRP16LE;
        static EyerAVPixelFormat EYER_YUVA422P;
        static EyerAVPixelFormat EYER_YUVA444P;
        static EyerAVPixelFormat EYER_YUVA420P9BE;
        static EyerAVPixelFormat EYER_YUVA420P9LE;
        static EyerAVPixelFormat EYER_YUVA422P9BE;
        static EyerAVPixelFormat EYER_YUVA422P9LE;
        static EyerAVPixelFormat EYER_YUVA444P9BE;
        static EyerAVPixelFormat EYER_YUVA444P9LE;
        static EyerAVPixelFormat EYER_YUVA420P10BE;
        static EyerAVPixelFormat EYER_YUVA420P10LE;
        static EyerAVPixelFormat EYER_YUVA422P10BE;
        static EyerAVPixelFormat EYER_YUVA422P10LE;
        static EyerAVPixelFormat EYER_YUVA444P10BE;
        static EyerAVPixelFormat EYER_YUVA444P10LE;
        static EyerAVPixelFormat EYER_YUVA420P16BE;
        static EyerAVPixelFormat EYER_YUVA420P16LE;
        static EyerAVPixelFormat EYER_YUVA422P16BE;
        static EyerAVPixelFormat EYER_YUVA422P16LE;
        static EyerAVPixelFormat EYER_YUVA444P16BE;
        static EyerAVPixelFormat EYER_YUVA444P16LE;
        static EyerAVPixelFormat EYER_XYZ12LE;
        static EyerAVPixelFormat EYER_XYZ12BE;
        static EyerAVPixelFormat EYER_NV16;
        static EyerAVPixelFormat EYER_NV20LE;
        static EyerAVPixelFormat EYER_NV20BE;
        static EyerAVPixelFormat EYER_RGBA64BE;
        static EyerAVPixelFormat EYER_RGBA64LE;
        static EyerAVPixelFormat EYER_BGRA64BE;
        static EyerAVPixelFormat EYER_BGRA64LE;
        static EyerAVPixelFormat EYER_YVYU422;
        static EyerAVPixelFormat EYER_YA16BE;
        static EyerAVPixelFormat EYER_YA16LE;
        static EyerAVPixelFormat EYER_GBRAP;
        static EyerAVPixelFormat EYER_GBRAP16BE;
        static EyerAVPixelFormat EYER_GBRAP16LE;
        static EyerAVPixelFormat EYER_0RGB;
        static EyerAVPixelFormat EYER_RGB0;
        static EyerAVPixelFormat EYER_0BGR;
        static EyerAVPixelFormat EYER_BGR0;
        static EyerAVPixelFormat EYER_YUV420P12BE;
        static EyerAVPixelFormat EYER_YUV420P12LE;
        static EyerAVPixelFormat EYER_YUV420P14BE;
        static EyerAVPixelFormat EYER_YUV420P14LE;
        static EyerAVPixelFormat EYER_YUV422P12BE;
        static EyerAVPixelFormat EYER_YUV422P12LE;
        static EyerAVPixelFormat EYER_YUV422P14BE;
        static EyerAVPixelFormat EYER_YUV422P14LE;
        static EyerAVPixelFormat EYER_YUV444P12BE;
        static EyerAVPixelFormat EYER_YUV444P12LE;
        static EyerAVPixelFormat EYER_YUV444P14BE;
        static EyerAVPixelFormat EYER_YUV444P14LE;
        static EyerAVPixelFormat EYER_GBRP12BE;
        static EyerAVPixelFormat EYER_GBRP12LE;
        static EyerAVPixelFormat EYER_GBRP14BE;
        static EyerAVPixelFormat EYER_GBRP14LE;
        static EyerAVPixelFormat EYER_YUVJ411P;
        static EyerAVPixelFormat EYER_BAYER_BGGR8;
        static EyerAVPixelFormat EYER_BAYER_RGGB8;
        static EyerAVPixelFormat EYER_BAYER_GBRG8;
        static EyerAVPixelFormat EYER_BAYER_GRBG8;
        static EyerAVPixelFormat EYER_BAYER_BGGR16LE;
        static EyerAVPixelFormat EYER_BAYER_BGGR16BE;
        static EyerAVPixelFormat EYER_BAYER_RGGB16LE;
        static EyerAVPixelFormat EYER_BAYER_RGGB16BE;
        static EyerAVPixelFormat EYER_BAYER_GBRG16LE;
        static EyerAVPixelFormat EYER_BAYER_GBRG16BE;
        static EyerAVPixelFormat EYER_BAYER_GRBG16LE;
        static EyerAVPixelFormat EYER_BAYER_GRBG16BE;
        static EyerAVPixelFormat EYER_YUV440P10LE;
        static EyerAVPixelFormat EYER_YUV440P10BE;
        static EyerAVPixelFormat EYER_YUV440P12LE;
        static EyerAVPixelFormat EYER_YUV440P12BE;
        static EyerAVPixelFormat EYER_AYUV64LE;
        static EyerAVPixelFormat EYER_AYUV64BE;
        static EyerAVPixelFormat EYER_P010LE;
        static EyerAVPixelFormat EYER_P010BE;
        static EyerAVPixelFormat EYER_GBRAP12BE;
        static EyerAVPixelFormat EYER_GBRAP12LE;
        static EyerAVPixelFormat EYER_GBRAP10BE;
        static EyerAVPixelFormat EYER_GBRAP10LE;
        static EyerAVPixelFormat EYER_GRAY12BE;
        static EyerAVPixelFormat EYER_GRAY12LE;
        static EyerAVPixelFormat EYER_GRAY10BE;
        static EyerAVPixelFormat EYER_GRAY10LE;
        static EyerAVPixelFormat EYER_P016LE;
        static EyerAVPixelFormat EYER_P016BE;
        static EyerAVPixelFormat EYER_GRAY9BE;
        static EyerAVPixelFormat EYER_GRAY9LE;
        static EyerAVPixelFormat EYER_GBRPF32BE;
        static EyerAVPixelFormat EYER_GBRPF32LE;
        static EyerAVPixelFormat EYER_GBRAPF32BE;
        static EyerAVPixelFormat EYER_GBRAPF32LE;
        static EyerAVPixelFormat EYER_GRAY14BE;
        static EyerAVPixelFormat EYER_GRAY14LE;
        static EyerAVPixelFormat EYER_GRAYF32BE;
        static EyerAVPixelFormat EYER_GRAYF32LE;
        static EyerAVPixelFormat EYER_YUVA422P12BE;
        static EyerAVPixelFormat EYER_YUVA422P12LE;
        static EyerAVPixelFormat EYER_YUVA444P12BE;
        static EyerAVPixelFormat EYER_YUVA444P12LE;
        static EyerAVPixelFormat EYER_NV24;
        static EyerAVPixelFormat EYER_NV42;
        static EyerAVPixelFormat EYER_Y210BE;
        static EyerAVPixelFormat EYER_Y210LE;
        static EyerAVPixelFormat EYER_X2RGB10LE;
        static EyerAVPixelFormat EYER_X2RGB10BE;

        static EyerAVPixelFormat EYER_D3D11;
        static EyerAVPixelFormat EYER_D3D11VA_VLD;

        static EyerAVPixelFormat EYER_EXT_CVPIXELBUFFER;

        EyerAVPixelFormat();
        EyerAVPixelFormat(const EyerAVPixelFormat & pixelFormat);
        EyerAVPixelFormat(int id, int ffmpegId, const EyerString & _name, const EyerString & _descName);

        EyerAVPixelFormat & operator = (const EyerAVPixelFormat & pixelFormat);

        bool operator == (const EyerAVPixelFormat & format) const;
        bool operator != (const EyerAVPixelFormat & format) const;

        static EyerAVPixelFormat GetByFFmpegId(int ffmpegId);
        static EyerAVPixelFormat GetById(int id);

        const int GetId() const;
        const int GetFFmpegId() const;
        const EyerString GetName() const;
        const EyerString GetDescName() const;

        const int GetPixelBitDepth(int index) const;

        const int GetPixelLog2ChromaW() const;
        const int GetPixelLog2ChromaH() const;
    private:
        int id = 0;
        int ffmpegId = 0;
        EyerString name = "";
        EyerString descName = "";
    };
}

#endif //EYERLIB_EYERAVPIXELFORMAT_HPP