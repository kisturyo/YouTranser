#ifndef EYERLIB_EYERAVDECODER_HPP
#define EYERLIB_EYERAVDECODER_HPP

#include "EyerAVStream.hpp"
#include "EyerAVPacket.hpp"
#include "EyerAVFrame.hpp"

namespace Eyer
{
    class EyerAVDecoderPrivate;

    enum EyerAVDecoderOption
    {
        NONE,
        D3D11VA
    };

    class EyerAVDecoder
    {
    public:
        EyerAVDecoder();
        ~EyerAVDecoder();

        int Init(const EyerAVStream & stream, int threadnum = 4, EyerAVDecoderOption option = NONE);

        int GetTimebase(EyerAVRational & timebase);
        int GetSampleRate();
        EyerAVChannelLayout GetAVChannelLayout();
        EyerAVSampleFormat GetAVSampleFormat();

        int SendPacket(EyerAVPacket * packet);
        int SendPacket(EyerAVPacket & packet);
        int SendPacket(EyerSmartPtr<EyerAVPacket> & packet);

        int SendPacketNull();
        int RecvFrame(EyerAVFrame & frame);
        EyerSmartPtr<EyerAVFrame> RecvFrame(int & ret);

        const int GetAngle() const;

    private:
        // 禁止拷贝
        EyerAVDecoder(const EyerAVDecoder &) = delete;
        EyerAVDecoder & operator = (const EyerAVDecoder &) = delete;

    public:
        EyerAVDecoderPrivate * piml = nullptr;
    };
}

#endif //EYERLIB_EYERAVDECODER_HPP
