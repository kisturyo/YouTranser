#include "EyerAVTranscoder.hpp"

#include <vector>

#include "EyerAVTranscodeStream.hpp"
#include "EyerAVTranscoderSupport.hpp"

namespace Eyer
{
    EyerAVTranscoder::EyerAVTranscoder(const EyerString & _inputPath)
    {
        inputPath = _inputPath;
        status = EyerAVTranscoderStatus::PREPARE;
    }

    EyerAVTranscoder::~EyerAVTranscoder()
    {

    }

    int EyerAVTranscoder::SetOutputPath(const EyerString & _outputPath)
    {
        outputPath = _outputPath;
        return 0;
    }

    int EyerAVTranscoder::SetParams(const EyerAVTranscoderParams & _params)
    {
        params = _params;
        return 0;
    }

    int EyerAVTranscoder::Transcode_(EyerAVTranscoderInterrupt * interrupt)
    {
        Eyer::EyerAVReader reader(inputPath);
        int ret = reader.Open();
        if(ret){
            EyerLog("Open AV file fail\n");
            status = EyerAVTranscoderStatus::FAIL;
            errorDesc = "打开视频文件失败";
            if(listener != nullptr) {
                listener->OnFail(EyerAVTranscoderError::OPEN_INPUT_FAIL);
            }
            return -1;
        }

        return 0;
    }

    int EyerAVTranscoder::Transcode(EyerAVTranscoderInterrupt * interrupt, EyerAVReaderCustomIO * customIO)
    {
        long long startTime = Eyer::EyerTime::GetTimeNano();

        status = EyerAVTranscoderStatus::ING;
        Eyer::EyerAVReader reader(inputPath, customIO);
        int ret = reader.Open();
        if(ret){
            EyerLog("Open AV file fail\n");
            status = EyerAVTranscoderStatus::FAIL;
            errorDesc = "打开视频文件失败";
            if(listener != nullptr) {
                listener->OnFail(EyerAVTranscoderError::OPEN_INPUT_FAIL);
            }
            return -1;
        }

        duration = reader.GetDuration();

        Eyer::EyerAVWriter write(outputPath);
        ret = write.Open();
        if(ret){
            EyerLog("Open Output file fail\n");
            status = EyerAVTranscoderStatus::FAIL;
            errorDesc = "输出路径不存在或不可写";
            if(listener != nullptr){
                listener->OnFail(EyerAVTranscoderError::OPEN_OUTPUT_FAIL);
            }
            return -1;
        }

        // Init Decoder and Encoder
        int streamCount = reader.GetStreamCount();
        std::vector<EyerAVTranscodeStream *> transcodeStream;

        for(int i=0;i<streamCount;i++){
            EyerAVStream stream = reader.GetStream(i);

            //转码视频还是音频
            if((stream.GetType() == EyerAVMediaType::MEDIA_TYPE_AUDIO && !params.GetCareAudio()) ||
               (stream.GetType() == EyerAVMediaType::MEDIA_TYPE_VIDEO && !params.GetCareVideo())){
                EyerAVTranscodeStream * ts = new EyerAVTranscodeStream();
                transcodeStream.push_back(ts);
                continue;
            }

            // Init ts
            EyerAVTranscodeStream * ts = new EyerAVTranscodeStream();
            transcodeStream.push_back(ts);
            ts->readStreamId = stream.GetStreamId();

            // 初始化解码器
            EyerAVDecoder * decoder = new EyerAVDecoder();
            ret = decoder->Init(stream, params.GetDecodeThreadNum());
            if(ret){
                EyerLog("Init decoder error, stream id: %d\n", stream.GetStreamId());
                delete decoder;
                ts->decoder = nullptr;
                ts->encoder = nullptr;
                ts->resample = nullptr;
                continue;
            }
            ts->decoder = decoder;

            // 初始化编码器
            EyerAVEncoder * encoder = new EyerAVEncoder();
            ret = InitEncoder(encoder, stream);
            if(ret){
                EyerLog("Init encoder error, stream id: %d\n", stream.GetStreamId());
                delete encoder;
                ts->encoder = nullptr;
                if(listener != nullptr){
                    listener->OnFail(EyerAVTranscoderError::INIT_ENCODER_FAIL);
                }
                return -1;
            }
            ts->encoder = encoder;

            ts->writeStreamId = write.AddStream(*encoder);
            EyerLog("outputStreamId: %d\n", ts->writeStreamId);

            if(stream.GetType() == EyerAVMediaType::MEDIA_TYPE_AUDIO){
                EyerAVResample * resample = new EyerAVResample();

                EyerAVChannelLayout inputChannelLayout = stream.GetChannelLayout();
                if(inputChannelLayout == EyerAVChannelLayout::UNKNOW){
                    inputChannelLayout = EyerAVChannelLayout::GetDefaultChannelLayout(stream.GetChannels());
                }

                EyerLog("ChannelLayout: %s\n", inputChannelLayout.GetName().c_str());

                resample->Init(
                        encoder->GetChannelLayout(),
                        encoder->GetSampleFormat(),
                        encoder->GetSampleRate(),

                        inputChannelLayout,
                        stream.GetSampleFormat(),
                        stream.GetSampleRate()
                );
                ts->resample = resample;
            }
            else {
                ts->resample = nullptr;
            }
        }

        ret = write.WriteHand();
        if(ret){
            status = EyerAVTranscoderStatus::FAIL;
            errorDesc = "写入视频头失败";
            if(listener != nullptr){
                listener->OnFail(EyerAVTranscoderError::OPEN_WRITE_HEAD_FAIL);
            }
            return -1;
        }

        // Start transcode
        if(params.GetStartTime() != 0.0){
            reader.Seek(params.GetStartTime());
        }
        bool isInterrupt = false;
        bool isRangeEnd = false;
        double time = 0;
        while(1){
            EyerAVPacket packet;


            long long startTime = Eyer::EyerTime::GetTimeNano();
            int ret = reader.Read(packet);
            long long endTime = Eyer::EyerTime::GetTimeNano();
            ioReadTime += (endTime - startTime);

            if(ret){
                break;
            }

            int streamIndex = packet.GetStreamIndex();

            EyerAVTranscodeStream * ts = transcodeStream[streamIndex];
            EyerAVDecoder * decoder = ts->decoder;
            if(decoder == nullptr){
                continue;
            }

            EyerAVEncoder * encoder = ts->encoder;
            if(encoder == nullptr){
                continue;
            }

            decoder->SendPacket(packet);
            while(1){
                EyerAVFrame frame;
                ret = decoder->RecvFrame(frame);
                if(ret){
                    break;
                }
                //  EyerLog("Frame PTS: %f, Stream ID: %d\n", frame.GetSecPTS(), streamIndex);

                //range处理
                if((params.GetStartTime() != 0.0) && (frame.GetSecPTS() < params.GetStartTime())){
                    continue;
                }
                if((params.GetEndTime() != 0.0) && (frame.GetSecPTS() > params.GetEndTime())){
                    int packetStreamId = packet.GetStreamIndex();
                    int usefulTsNum = 0;
                    int rangeEndNum = 0;
                    for(int i=0; i<transcodeStream.size(); i++){
                        EyerAVTranscodeStream * ts = transcodeStream[i];
                        if(ts->encoder != nullptr){
                            usefulTsNum++;

                            if(ts->isRangeEnd == 1){
                                rangeEndNum++;
                            }
                            if(ts->isRangeEnd == 0 && ts->readStreamId == packetStreamId ){
                                ts->isRangeEnd = 1;
                                rangeEndNum++;
                            }
                        }
                    }

                    if(usefulTsNum == rangeEndNum){
                        isRangeEnd = true;
                        break;
                    }
                    continue;
                }
                time = frame.GetSecPTS();
                // EyerLog("Frame PTS: %f, Stream ID: %d\n", frame.GetSecPTS(), streamIndex);
                EncodeFrame(&write, ts, frame);
            }

            if(isRangeEnd){
                break;
            }

            if(interrupt != nullptr){
                if(interrupt->interrupt()){
                    isInterrupt = true;
                    break;
                }
            }

            // EyerLog("time: %f\n", time);
        }

        // Clear Decoder
        for(int i = 0; i < transcodeStream.size(); i++){
            EyerAVTranscodeStream * ts = transcodeStream[i];
            EyerAVDecoder *decoder = ts->decoder;
            if (decoder != nullptr) {
                decoder->SendPacketNull();
                while(1){
                    EyerAVFrame frame;
                    ret = decoder->RecvFrame(frame);
                    if(ret){
                        break;
                    }
                    if((params.GetEndTime() != 0.0) && (frame.GetSecPTS() > params.GetEndTime())){
                        break;
                    }
                    //  EyerLog("Flush Frame PTS: %f , Stream ID: %d\n", frame.GetSecPTS(), ts->readStreamId);
                    EncodeFrame(&write, ts, frame);
                }
            }
        }

        if(!isInterrupt) {
            // Clear Encode
            for(int i = 0; i < transcodeStream.size(); i++) {
                EyerAVTranscodeStream *ts = transcodeStream[i];
                ClearFrame(&write, ts);
            }
        }

        // Free Decoder and Encoder
        for(int i = 0; i < transcodeStream.size(); i++){
            EyerAVTranscodeStream * ts = transcodeStream[i];
            EyerAVDecoder * decoder = ts->decoder;
            if(decoder != nullptr){
                delete decoder;
                decoder = nullptr;
            }

            EyerAVEncoder * encoder = ts->encoder;
            if(encoder != nullptr){
                delete encoder;
                encoder = nullptr;
            }

            EyerAVResample * resample = ts->resample;
            if(resample != nullptr){
                delete resample;
                resample = nullptr;
            }
        }

        for(int i = 0; i < transcodeStream.size(); i++){
            EyerAVTranscodeStream * ts = transcodeStream[i];
            delete ts;
        }
        transcodeStream.clear();

        {
            long long startTime = Eyer::EyerTime::GetTimeNano();
            write.WriteTrailer();
            write.Close();
            long long endTime = Eyer::EyerTime::GetTimeNano();
            ioWriteTime += (endTime - startTime);
        }

        reader.Close();

        if(isInterrupt){
            status = EyerAVTranscoderStatus::FAIL;
            if(listener != nullptr){
                errorDesc = "被取消";
                listener->OnFail(EyerAVTranscoderError::INTERRUPT_FAIL);
            }
        }
        else{
            status = EyerAVTranscoderStatus::SUCC;
            if(listener != nullptr){
                listener->OnSuccess();
            }
        }

        long long endTime = Eyer::EyerTime::GetTimeNano();

        totleTime = endTime - startTime;

        EyerLog("==================Transcoder Finish Start==================\n");
        EyerLog("inputPath: %s\n", inputPath.c_str());
        EyerLog("outputPath: %s\n", outputPath.c_str());
        EyerLog("Transcode Totle time: %f s\n", totleTime * 1.0 / 1000000000);
        EyerLog("Transcode IO Read Time: %f s\n", ioReadTime * 1.0 / 1000000000);
        EyerLog("Transcode IO Write Time: %f s\n", ioWriteTime * 1.0 / 1000000000);
        EyerLog("==================Transcoder Finish End==================\n");

        return 0;
    }

    int EyerAVTranscoder::InitEncoder(EyerAVEncoder * encoder, const EyerAVStream & stream)
    {
        EyerLog("InitEncoder codecID: %s\n", params.GetVideoCodecId().GetDescName().c_str());
        if(stream.GetType() == EyerAVMediaType::MEDIA_TYPE_VIDEO) {
            EyerLog("InitEncoder codecID: %s\n", params.GetVideoCodecId().GetDescName().c_str());
            EyerAVRational encoderTimebase;
            encoderTimebase.den = 1000;
            encoderTimebase.num = 1;

            EyerAVEncoderParam encoderParam;
            if(params.GetVideoCodecId() == EyerAVCodecID::CODEC_ID_H264){
                EyerAVRational encoderTimebase;
                encoderTimebase.den = 1000;
                encoderTimebase.num = 1;

                int distWidth = stream.GetWidth();
                int distHeight = stream.GetHeight();
                if(params.GetWidth() > 0){
                    distWidth = params.GetWidth();
                }
                if(params.GetHeight() > 0){
                    distHeight = params.GetHeight();
                }
                EyerAVPixelFormat distPixelFormat = stream.GetPixelFormat();
                if(params.GetVideoPixelFormat() != EyerAVPixelFormat::EYER_KEEP_SAME){
                    distPixelFormat = params.GetVideoPixelFormat();
                }

                EyerAVTranscoderSupport support;
                bool isSupport = support.IsPixelFmtSupports(params.GetVideoCodecId(), distPixelFormat);
                if(!isSupport){
                    errorDesc = "视频像素格式编码器不支持";
                    return -2;
                }

                EyerAVEncoderParam encoderParam;
                encoderParam.InitH264(
                        distWidth,
                        distHeight,
                        encoderTimebase,
                        distPixelFormat,
                        params.GetCRF()
                        );
                encoderParam.threadnum = params.GetEncodeThreadNum();
                return encoder->Init(encoderParam);
            }
            else if(params.GetVideoCodecId() == EyerAVCodecID::CODEC_ID_H265){
                EyerAVRational encoderTimebase;
                encoderTimebase.den = 1000;
                encoderTimebase.num = 1;

                int distWidth = stream.GetWidth();
                int distHeight = stream.GetHeight();
                if(params.GetWidth() > 0){
                    distWidth = params.GetWidth();
                }
                if(params.GetHeight() > 0){
                    distHeight = params.GetHeight();
                }
                EyerAVPixelFormat distPixelFormat = stream.GetPixelFormat();
                if(params.GetVideoPixelFormat() != EyerAVPixelFormat::EYER_KEEP_SAME){
                    distPixelFormat = params.GetVideoPixelFormat();
                }

                EyerAVTranscoderSupport support;
                bool isSupport = support.IsPixelFmtSupports(params.GetVideoCodecId(), distPixelFormat);
                if(!isSupport){
                    errorDesc = "视频像素格式编码器不支持";
                    return -2;
                }

                EyerAVEncoderParam encoderParam;
                encoderParam.InitH265(
                        distWidth,
                        distHeight,
                        encoderTimebase,
                        distPixelFormat,
                        params.GetCRF()
                );
                encoderParam.threadnum = params.GetEncodeThreadNum();
                return encoder->Init(encoderParam);
            }
            else if(params.GetVideoCodecId() == EyerAVCodecID::CODEC_ID_PRORES){
                
                EyerAVRational encoderTimebase;
                encoderTimebase.den = 1000;
                encoderTimebase.num = 1;

                int distWidth = stream.GetWidth();
                int distHeight = stream.GetHeight();
                if(params.GetWidth() > 0){
                    distWidth = params.GetWidth();
                }
                if(params.GetHeight() > 0){
                    distHeight = params.GetHeight();
                }
                EyerAVPixelFormat distPixelFormat = stream.GetPixelFormat();
                if(params.GetVideoPixelFormat() != EyerAVPixelFormat::EYER_KEEP_SAME){
                    distPixelFormat = params.GetVideoPixelFormat();
                }

                EyerLog("InitEncoder codecID: %s, %d, %d, %s\n", params.GetVideoCodecId().GetDescName().c_str(), distWidth, distHeight, distPixelFormat.GetDescName().c_str());

                EyerAVTranscoderSupport support;
                bool isSupport = support.IsPixelFmtSupports(params.GetVideoCodecId(), distPixelFormat);
                if(!isSupport){
                    errorDesc = "视频像素格式编码器不支持";
                    return -2;
                }

                EyerLog("InitEncoder codecID: %s, %d, %d, %s\n", params.GetVideoCodecId().GetDescName().c_str(), distWidth, distHeight, distPixelFormat.GetDescName().c_str());

                EyerAVEncoderParam encoderParam;
                encoderParam.InitProres(distWidth, distHeight, encoderTimebase, distPixelFormat);
                encoderParam.threadnum = params.GetEncodeThreadNum();
                return encoder->Init(encoderParam);
            }
            return -1;
        }
        else if(stream.GetType() == EyerAVMediaType::MEDIA_TYPE_AUDIO) {
            EyerAVTranscoderSupport support;
            EyerAVEncoderParam encoderParam;
            if(params.GetAudioCodecId() == EyerAVCodecID::CODEC_ID_AAC){
                EyerAVCodecID audioCodec = params.GetAudioCodecId();
                EyerAVChannelLayout channelLayout = params.GetAudioChannelLayout();
                int sampleRate = params.GetSampleRate();

                if(channelLayout == EyerAVChannelLayout::EYER_KEEP_SAME){
                    channelLayout = stream.GetChannelLayout();
                    if(channelLayout == EyerAVChannelLayout::UNKNOW){
                        channelLayout = EyerAVChannelLayout::GetDefaultChannelLayout(stream.GetChannels());
                    }
                }
                EyerLog("Audio channelLayout: %s\n", channelLayout.GetDescName().c_str());

                bool isSupport = support.IsAudioChannelSupports(audioCodec, channelLayout);
                if(!isSupport){
                    errorDesc = "声道布局编码器不支持";
                    return -1;
                }


                if(sampleRate == SAMPLE_RATE_KEEP_SAME){
                    sampleRate = stream.GetSampleRate();
                }

                isSupport = support.IsSampleRateSupports(audioCodec, sampleRate);
                if(!isSupport){
                    errorDesc = "采样率编码器不支持";
                    return -1;
                }
                encoderParam.InitFDKAAC(
                        channelLayout,
                        support.GetHighestSampleFmt(audioCodec),
                        sampleRate
                        );
                return encoder->Init(encoderParam);
            }
            else if(params.GetAudioCodecId() == EyerAVCodecID::CODEC_ID_MP3){
                EyerAVCodecID audioCodec = params.GetAudioCodecId();
                EyerAVChannelLayout channelLayout = params.GetAudioChannelLayout();
                int sampleRate = params.GetSampleRate();

                if(channelLayout == EyerAVChannelLayout::EYER_KEEP_SAME){
                    channelLayout = stream.GetChannelLayout();
                    if(channelLayout == EyerAVChannelLayout::UNKNOW){
                        channelLayout = EyerAVChannelLayout::GetDefaultChannelLayout(stream.GetChannels());
                    }
                }
                EyerLog("Audio channelLayout: %s\n", channelLayout.GetDescName().c_str());

                bool isSupport = support.IsAudioChannelSupports(audioCodec, channelLayout);
                if(!isSupport){
                    errorDesc = "声道布局编码器不支持";
                    return -1;
                }


                if(sampleRate == SAMPLE_RATE_KEEP_SAME){
                    sampleRate = stream.GetSampleRate();
                }

                isSupport = support.IsSampleRateSupports(audioCodec, sampleRate);
                if(!isSupport){
                    errorDesc = "采样率编码器不支持";
                    return -1;
                }
                encoderParam.InitMP3(
                        channelLayout,
                        support.GetHighestSampleFmt(audioCodec),
                        sampleRate
                );
                return encoder->Init(encoderParam);
            }

            else if(params.GetAudioCodecId() == EyerAVCodecID::CODEC_ID_PCM_S16LE || params.GetAudioCodecId() == EyerAVCodecID::CODEC_ID_PCM_S32LE){
                EyerAVCodecID audioCodec = params.GetAudioCodecId();
                EyerAVChannelLayout channelLayout = params.GetAudioChannelLayout();
                int sampleRate = params.GetSampleRate();

                if(channelLayout == EyerAVChannelLayout::EYER_KEEP_SAME){
                    channelLayout = stream.GetChannelLayout();
                    if(channelLayout == EyerAVChannelLayout::UNKNOW){
                        channelLayout = EyerAVChannelLayout::GetDefaultChannelLayout(stream.GetChannels());
                    }
                }
                EyerLog("Audio channelLayout: %s\n", channelLayout.GetDescName().c_str());

                bool isSupport = support.IsAudioChannelSupports(audioCodec, channelLayout);
                if(!isSupport){
                    errorDesc = "声道布局编码器不支持";
                    return -1;
                }

                if(sampleRate == SAMPLE_RATE_KEEP_SAME){
                    sampleRate = stream.GetSampleRate();
                }

                isSupport = support.IsSampleRateSupports(audioCodec, sampleRate);
                if(!isSupport){
                    errorDesc = "采样率编码器不支持";
                    return -1;
                }

                if(params.GetAudioCodecId() == EyerAVCodecID::CODEC_ID_PCM_S16LE){
                    encoderParam.InitPCM_S16LE(
                            channelLayout,
                            support.GetHighestSampleFmt(audioCodec),
                            sampleRate
                    );
                }
                if(params.GetAudioCodecId() == EyerAVCodecID::CODEC_ID_PCM_S32LE){
                    encoderParam.InitPCM_S32LE(
                            channelLayout,
                            support.GetHighestSampleFmt(audioCodec),
                            sampleRate
                    );
                }
                return encoder->Init(encoderParam);
            }
            encoder->Init(encoderParam);
            return 0;
        }
        else {

        }
        return -1;
    }

    int EyerAVTranscoder::EncodeFrame(Eyer::EyerAVWriter * write, EyerAVTranscodeStream * ts, EyerAVFrame & frame)
    {
        EyerAVEncoder * encoder = ts->encoder;
        EyerAVResample * resample = ts->resample;

        if(encoder == nullptr){
            return -1;
        }

        if(ts->writeStreamId < 0){
            return -1;
        }

        EyerAVRational encodeTimebase = encoder->GetTimebase();
        EyerAVMediaType mediaType = encoder->GetMediaType();

        double currentSecPTS = 0;

        if(mediaType == EyerAVMediaType::MEDIA_TYPE_AUDIO && params.GetCareAudio()){
            if(resample == nullptr){
                EyerLog("Resample is null\n");
                return -1;
            }

            currentSecPTS = frame.GetSecPTS();

            // EyerLog("frame: %s\n", frame.GetSampleFormat().GetName().c_str());
            resample->PutAVFrame(frame);
            while(1){
                EyerAVFrame encodeFrame;
                int framesize = encoder->GetFrameSize();
                if(framesize <= 0){
                    framesize = 1024;
                }
                int ret = resample->GetFrame(encodeFrame, framesize);
                if(ret){
                    break;
                }
                encodeFrame.SetPTS(ts->audioPts);
                ts->audioPts += encodeFrame.GetSampleNB();

                ret = encoder->SendFrame(encodeFrame);
                while(1){
                    EyerAVPacket packet;
                    int ret = encoder->RecvPacket(packet);
                    if(ret){
                        break;
                    }
                    packet.SetStreamIndex(ts->writeStreamId);
                    packet.RescaleTs(encodeTimebase, write->GetTimebase(ts->writeStreamId));

                    long long startTime = Eyer::EyerTime::GetTimeNano();
                    write->WritePacket(packet);
                    long long endTime = Eyer::EyerTime::GetTimeNano();
                    ioWriteTime += (endTime - startTime);
                }
            }
        }
        else if(mediaType == EyerAVMediaType::MEDIA_TYPE_VIDEO && params.GetCareVideo()){
            currentSecPTS = frame.GetSecPTS();
            frame.SetPTS(frame.GetSecPTS() * 1000);
            if(params.GetStartTime() != 0.0){
                if(ts->encoderVideoFrameIndex == 0){
                    frame.SetPTS(0);
                }else{
                    frame.SetPTS(frame.GetPTS() - (int64_t)(params.GetStartTime() * 1000));
                }
            }
            ts->encoderVideoFrameIndex++;

            EyerAVPixelFormat distPixelformat = params.GetVideoPixelFormat();
            int distWidth = params.GetWidth();
            int distHeight = params.GetHeight();

            EyerAVFrame distFrame;
            frame.Scale(distFrame, distPixelformat, distWidth, distHeight);

            // EyerLog("distPixelformat: %s\n", frame.GetPixelFormat().GetDescName().c_str());

            encoder->SendFrame(distFrame);
            while(1){
                EyerAVPacket packet;
                int ret = encoder->RecvPacket(packet);
                if(ret){
                    break;
                }
                // EyerLog("PTS: %lld, DTS: %lld\n", packet.GetPTS(), packet.GetDTS());
                packet.SetStreamIndex(ts->writeStreamId);
                packet.RescaleTs(encodeTimebase, write->GetTimebase(ts->writeStreamId));

                if(packet.GetDTS() > packet.GetPTS()){
                    packet.SetPTS(packet.GetDTS());
                }
                // EyerLog("PTS: %lld, DTS: %lld\n", packet.GetPTS(), packet.GetDTS());

                long long startTime = Eyer::EyerTime::GetTimeNano();
                write->WritePacket(packet);
                long long endTime = Eyer::EyerTime::GetTimeNano();
                ioWriteTime += (endTime - startTime);
            }
        }

        // EyerLog("p: %f\n", currentSecPTS);

        if(currentSecPTS - lastUpdateTime >= 0.5){
            if(listener != nullptr){
                double tempCurrentSecPTS = 0;
                tempCurrentSecPTS = currentSecPTS - params.GetStartTime();
                if(params.GetStartTime() != 0.0 || params.GetEndTime() != 0.0){
                    if(params.GetEndTime() != 0.0){
                        duration = params.GetEndTime() - params.GetStartTime();
                    } else{
                        duration = duration - params.GetStartTime();
                    }
                }

                float progress = tempCurrentSecPTS / duration;
                if(progress >= 1.0){
                    progress = 1.0;
                }
                if(duration <= 0){
                    progress = 0;
                }

                // EyerLog("p: %f\n", progress);
                listener->OnProgress(progress);
            }
            lastUpdateTime = currentSecPTS;
        }

        return 0;
    }

    int EyerAVTranscoder::ClearFrame(Eyer::EyerAVWriter * write, EyerAVTranscodeStream * ts)
    {
        EyerAVEncoder * encoder = ts->encoder;
        EyerAVResample * resample = ts->resample;

        if(encoder == nullptr){
            return -1;
        }

        EyerAVRational encodeTimebase = encoder->GetTimebase();

        encoder->SendFrameNull();
        while(1){
            EyerAVPacket packet;
            int ret = encoder->RecvPacket(packet);
            if(ret){
                break;
            }
            packet.SetStreamIndex(ts->writeStreamId);
            packet.RescaleTs(encodeTimebase, write->GetTimebase(ts->writeStreamId));

            long long startTime = Eyer::EyerTime::GetTimeNano();
            write->WritePacket(packet);
            long long endTime = Eyer::EyerTime::GetTimeNano();
            ioWriteTime += (endTime - startTime);
        }

        return 0;
    }

    int EyerAVTranscoder::SetListener(EyerAVTranscoderListener * _listener)
    {
        listener = _listener;
        return 0;
    }

    EyerAVTranscoderStatus EyerAVTranscoder::GetStatus()
    {
        return status;
    }

    int EyerAVTranscoder::SetStatus(const EyerAVTranscoderStatus & _status)
    {
        status = _status;
        return 0;
    }

    EyerString EyerAVTranscoder::GetErrorDesc()
    {
        return errorDesc;
    }

    int EyerAVTranscoder::SetErrorDesc(const EyerString & _errorDesc)
    {
        errorDesc = _errorDesc;
        return 0;
    }
}