#include "com_gulei_ffmpegandroid_ApiPlayer.h"
#include <jni.h>
#include <stdlib.h>
#include <android/log.h>
#include <libavutil/imgutils.h>
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include <unistd.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>


#include "libyuv.h"

#define TAG "ApiPlayer"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__);

JNIEXPORT jstring JNICALL Java_com_gulei_ffmpegandroid_ApiPlayer_jnitest
        (JNIEnv *env, jclass jclazz) {
    return (*env)->NewStringUTF(env, "Hello Jni");
};

JNIEXPORT void JNICALL Java_com_gulei_ffmpegandroid_ApiPlayer_decodeVideo
        (JNIEnv *env, jclass jclazz, jstring input_jstr, jstring output_jstr) {
    const char *input_cstr = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    const char *output_cstr = (*env)->GetStringUTFChars(env, output_jstr, NULL);
    LOGI("输入：%s\n,输出：%s\n", input_cstr, output_cstr);

    //1.注册组件
    //avcodec_register_all();
    //av_codec_iterate(NULL);
    //封装格式上下文
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    //2.打开输入视频文件
    //@return 0 on success, a negative AVERROR on failure.
    if (avformat_open_input(&pFormatCtx, input_cstr, NULL, NULL) != 0) {
        LOGI("打开视频文件失败");
        return;
    } else {
        LOGI("打开视频文件成功")
    };
    //3.获取视频信息
    //@return >=0 if OK, AVERROR_xxx on error
    if (avformat_find_stream_info(pFormatCtx, NULL) >= 0) {
        LOGI("获取视频信息成功");
    } else {
        LOGI("获取视频信息失败");
        return;
    }
    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int video_stream_idx = -1;
    int i = 0;
    for (; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    //4.获取视频解码器
    AVCodecParameters *pCodeParameter = pFormatCtx->streams[video_stream_idx]->codecpar;
    AVCodec *pCodec = avcodec_find_decoder(pCodeParameter->codec_id);
    //// 申请AVCodecContext空间。需要传递一个编码器，也可以不传，但不会包含编码器。
    AVCodecContext *avCodecContext = avcodec_alloc_context3(pCodec);
    //该函数用于将流里面的参数，也就是AVStream里面的参数直接复制到AVCodecContext的上下文当中
    avcodec_parameters_to_context(avCodecContext, pCodeParameter);
    //输出视频信息
    LOGI("视频格式：%s", pFormatCtx->iformat->name);
    LOGI("视频时长：%d", pFormatCtx->duration / 1000000);//（单位：微秒us，转换为秒需要除以1000000）
    LOGI("视频比特率：%d", pFormatCtx->bit_rate / 1000);//（单位bps，转换为kbps需要除以1000）
    LOGI("视频宽高：%d,%d", avCodecContext->width, avCodecContext->height);

    if (pCodec == NULL) {
        LOGI("解码器为空");
        return;
    }
    //5.打开解码器
    //@return zero on success, a negative value on error
    if (avcodec_open2(avCodecContext, pCodec, NULL) == 0) {
        LOGI("打开解码器成功");
    } else {
        LOGI("打开解码器失败");
        return;
    }
    LOGI("解码器名称：%s", pCodec->name);
    //编码数据容器
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));

    //解码后的像素数据容器,av_frame_alloc没有为data像素数据分配空间，后续还需要自行为data开辟内存空间
    AVFrame *avFrame = av_frame_alloc();
    AVFrame *yuvFrame = av_frame_alloc();

    //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
    //缓冲区分配内存
    //av_image_get_buffer_size（）函数的作用是通过指定像素格式、图像宽、图像高来计算所需的内存大小。
    // 重点说明一个参数align:此参数是设定内存对齐的对齐数，也就是按多大的字节进行内存对齐。
    // 比如设置为1，表示按1字节对齐，那么得到的结果就是与实际的内存大小一样。再比如设置为4，表示按4字节对齐。也就是内存的起始地址必须是4的整倍数。
    uint8_t *out_buffer = (uint8_t *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, avCodecContext->width,
                                     avCodecContext->height, 1));
    //对申请的内存进行格式化，将out_buffer指向data,并且像素存储类型格式化为YUV420p
    av_image_fill_arrays(yuvFrame->data, yuvFrame->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                         avCodecContext->width, avCodecContext->height, 1);

    //输出文件
    FILE *fp_yuv = fopen(output_cstr, "wb");

    //用于像素格式转换或者缩放
    struct SwsContext *sws_ctx = sws_getContext(avCodecContext->width, avCodecContext->height,
                                                avCodecContext->pix_fmt, avCodecContext->width,
                                                avCodecContext->height, AV_PIX_FMT_YUV420P,
                                                SWS_BILINEAR, NULL, NULL, NULL);
    int got_picture_ptr, ret, frame_count = 0;
    //6.一帧帧读取压缩数据
    while (av_read_frame(pFormatCtx, avPacket) >= 0) {
        if (avPacket->stream_index == video_stream_idx) {
            ret = avcodec_decode_video2(avCodecContext, avFrame, &got_picture_ptr, avPacket);
            if (ret < 0) {
                LOGI("解码错误xxxx");
            }
            //为0说明解码完成，非0正在解码
            if (got_picture_ptr) {
                //AVFrame转为像素格式YUV420，宽高
                //2 6输入、输出数据
                //3 7输入、输出画面一行的数据的大小 AVFrame 转换是一行一行转换的
                //4 输入数据第一列要转码的位置 从0开始
                //5 输入画面的高度
                sws_scale(sws_ctx, avFrame->data, avFrame->linesize, 0, avCodecContext->height,
                          yuvFrame->data, yuvFrame->linesize);
                //输出到YUV文件
                //AVFrame像素帧写入文件
                //data解码后的图像像素数据（音频采样数据）
                //Y 亮度 UV 色度（压缩了） 人对亮度更加敏感
                //U V 个数是Y的1/4
                //在这里又有一个问题，YUV420P格式需要写入data[0]，data[1]，data[2]；而RGB24，UYVY格式却仅仅是写入data[0]，
                // 他们的区别到底是什么呢？经过研究发现，在FFMPEG中，图像原始数据包括两种：planar和packed。planar就是将几个分量分开存，
                // 比如YUV420中，data[0]专门存Y，data[1]专门存U，data[2]专门存V。而packed则是打包存，所有数据都存在data[0]中。
                //具体哪个格式是planar，哪个格式是packed，可以查看pixfmt.h文件。注：有些格式名称后面是LE或BE，分别对应little-endian或big-endian。
                // 另外名字后面有P的是planar格式。
                int y_size = avCodecContext->width * avCodecContext->height;
                fwrite(avFrame->data[0], y_size, 1, fp_yuv);
                fwrite(avFrame->data[1], y_size / 4, 1, fp_yuv);
                fwrite(avFrame->data[2], y_size / 4, 1, fp_yuv);
                frame_count++;
                LOGI("解码第%d帧", frame_count);
            }
        }
        //释放资源
        av_packet_unref(avPacket);
    }
    fclose(fp_yuv);

    (*env)->ReleaseStringUTFChars(env, input_jstr, input_cstr);
    (*env)->ReleaseStringUTFChars(env, output_jstr, output_cstr);

    av_frame_free(&avFrame);
    av_frame_free(&yuvFrame);
    avcodec_free_context(&avCodecContext);
    avformat_free_context(pFormatCtx);
    sws_freeContext(sws_ctx);
};


JNIEXPORT void JNICALL Java_com_gulei_ffmpegandroid_ApiPlayer_decodeVideo2
        (JNIEnv *env, jclass jclazz, jstring input_jstr, jstring output_jstr) {
    const char *input_cstr = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    const char *output_cstr = (*env)->GetStringUTFChars(env, output_jstr, NULL);
    LOGI("输入：%s\n,输出：%s\n", input_cstr, output_cstr);

    //1.注册组件
    //avcodec_register_all();
    //av_codec_iterate(NULL);
    //封装格式上下文
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    //2.打开输入视频文件
    //@return 0 on success, a negative AVERROR on failure.
    if (avformat_open_input(&pFormatCtx, input_cstr, NULL, NULL) != 0) {
        LOGI("打开视频文件失败");
        return;
    } else {
        LOGI("打开视频文件成功")
    };
    //3.获取视频信息
    //@return >=0 if OK, AVERROR_xxx on error
    if (avformat_find_stream_info(pFormatCtx, NULL) >= 0) {
        LOGI("获取视频信息成功");
    } else {
        LOGI("获取视频信息失败");
        return;
    }
    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int video_stream_idx = -1;
    int i = 0;
    for (; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    //4.获取视频解码器
    AVCodecParameters *pCodeParameter = pFormatCtx->streams[video_stream_idx]->codecpar;
    AVCodec *pCodec = avcodec_find_decoder(pCodeParameter->codec_id);
    //// 申请AVCodecContext空间。需要传递一个编码器，也可以不传，但不会包含编码器。
    AVCodecContext *avCodecContext = avcodec_alloc_context3(pCodec);
    //该函数用于将流里面的参数，也就是AVStream里面的参数直接复制到AVCodecContext的上下文当中
    avcodec_parameters_to_context(avCodecContext, pCodeParameter);
    //输出视频信息
    LOGI("视频格式：%s", pFormatCtx->iformat->name);
    LOGI("视频时长：%d", pFormatCtx->duration / 1000000);//（单位：微秒us，转换为秒需要除以1000000）
    LOGI("视频比特率：%d", pFormatCtx->bit_rate / 1000);//（单位bps，转换为kbps需要除以1000）
    LOGI("视频宽高：%d,%d", avCodecContext->width, avCodecContext->height);

    if (pCodec == NULL) {
        LOGI("解码器为空");
        return;
    }
    //5.打开解码器
    //@return zero on success, a negative value on error
    if (avcodec_open2(avCodecContext, pCodec, NULL) == 0) {
        LOGI("打开解码器成功");
    } else {
        LOGI("打开解码器失败");
        return;
    }
    LOGI("解码器名称：%s", pCodec->name);
    //编码数据容器
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));

    //解码后的像素数据容器,av_frame_alloc没有为data像素数据分配空间，后续还需要自行为data开辟内存空间
    AVFrame *avFrame = av_frame_alloc();
    AVFrame *yuvFrame = av_frame_alloc();

    //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
    //缓冲区分配内存
    //av_image_get_buffer_size（）函数的作用是通过指定像素格式、图像宽、图像高来计算所需的内存大小。
    // 重点说明一个参数align:此参数是设定内存对齐的对齐数，也就是按多大的字节进行内存对齐。
    // 比如设置为1，表示按1字节对齐，那么得到的结果就是与实际的内存大小一样。再比如设置为4，表示按4字节对齐。也就是内存的起始地址必须是4的整倍数。
    uint8_t *out_buffer = (uint8_t *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, avCodecContext->width,
                                     avCodecContext->height, 1));

    //对申请的内存进行格式化，将out_buffer指向data,并且像素存储类型格式化为YUV420p
    av_image_fill_arrays(yuvFrame->data, yuvFrame->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                         avCodecContext->width, avCodecContext->height, 1);

    //输出文件
    FILE *fp_yuv = fopen(output_cstr, "wb");

    //用于像素格式转换或者缩放
    struct SwsContext *sws_ctx = sws_getContext(avCodecContext->width, avCodecContext->height,
                                                avCodecContext->pix_fmt, avCodecContext->width,
                                                avCodecContext->height, AV_PIX_FMT_YUV420P,
                                                SWS_BILINEAR, NULL, NULL, NULL);
    int ret, frame_count = 0;
    //6.一帧帧读取压缩数据
    while (av_read_frame(pFormatCtx, avPacket) >= 0) {
        if (avPacket->stream_index == video_stream_idx) {
            //@return 0 on success, otherwise negative error code:
            ret = avcodec_send_packet(avCodecContext, avPacket);
            if (ret < 0) {
                LOGI("avcodec_send_packet错误xxxx");
                return;
            }
            for (;;) {
                ret = avcodec_receive_frame(avCodecContext, avFrame);
                if (ret == 0) {
                    //AVFrame转为像素格式YUV420，宽高
                    //2 6输入、输出数据
                    //3 7输入、输出画面一行的数据的大小 AVFrame 转换是一行一行转换的
                    //4 输入数据第一列要转码的位置 从0开始
                    //5 输入画面的高度
                    sws_scale(sws_ctx, avFrame->data, avFrame->linesize, 0, avCodecContext->height,
                              yuvFrame->data, yuvFrame->linesize);
                    //输出到YUV文件
                    //AVFrame像素帧写入文件
                    //data解码后的图像像素数据（音频采样数据）
                    //Y 亮度 UV 色度（压缩了） 人对亮度更加敏感
                    //U V 个数是Y的1/4
                    int y_size = avCodecContext->width * avCodecContext->height;
                    //在这里又有一个问题，YUV420P格式需要写入data[0]，data[1]，data[2]；而RGB24，UYVY格式却仅仅是写入data[0]，
                    // 他们的区别到底是什么呢？经过研究发现，在FFMPEG中，图像原始数据包括两种：planar和packed。planar就是将几个分量分开存，
                    // 比如YUV420中，data[0]专门存Y，data[1]专门存U，data[2]专门存V。而packed则是打包存，所有数据都存在data[0]中。
                    //具体哪个格式是planar，哪个格式是packed，可以查看pixfmt.h文件。注：有些格式名称后面是LE或BE，分别对应little-endian或big-endian。
                    // 另外名字后面有P的是planar格式。
                    fwrite(avFrame->data[0], y_size, 1, fp_yuv);
                    fwrite(avFrame->data[1], y_size / 4, 1, fp_yuv);
                    fwrite(avFrame->data[2], y_size / 4, 1, fp_yuv);
                    frame_count++;
                    LOGI("解码第%d帧", frame_count);
                } else {
                    LOGI("avcodec_receive_packet：AVERROR%d", ret);
                    break;
                }
            }
        }
        //释放资源
        av_packet_unref(avPacket);
    }
    fclose(fp_yuv);

    (*env)->ReleaseStringUTFChars(env, input_jstr, input_cstr);
    (*env)->ReleaseStringUTFChars(env, output_jstr, output_cstr);

    av_frame_free(&avFrame);
    av_frame_free(&yuvFrame);
    avcodec_free_context(&avCodecContext);
    avformat_free_context(pFormatCtx);
    sws_freeContext(sws_ctx);
};


JNIEXPORT void JNICALL Java_com_gulei_ffmpegandroid_ApiPlayer_render
        (JNIEnv *env, jclass jclazz, jstring input_jstr, jobject surface) {
    const char *input_cstr = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    AVFormatContext *avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, input_cstr, NULL, NULL) == 0) {
        LOGI("打开视频文件成功");
    } else {
        LOGI("打开视频文件失败");
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGI("获取视频流信息失败");
        return;
    }
    int videoStream_index = -1;
    int i = 0;
    for (; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream_index = i;
            break;
        }
    }
    AVCodecParameters *avCodecParameters = avFormatContext->streams[videoStream_index]->codecpar;
    AVCodec *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
    if (avCodec == NULL) {
        LOGI("没有解码器");
        return;
    }
    AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(avCodecContext, avCodecParameters);
    //输出视频信息
    LOGI("视频格式：%s", avFormatContext->iformat->name);
    LOGI("视频时长：%d", avFormatContext->duration / 1000000);//（单位：微秒us，转换为秒需要除以1000000）
    LOGI("视频比特率：%d", avFormatContext->bit_rate / 1000);//（单位bps，转换为kbps需要除以1000）
    LOGI("视频宽高：%d,%d", avCodecContext->width, avCodecContext->height);

    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGI("打开解码器失败");
        return;
    }

    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    AVFrame *yuvFrame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();
    int gotPicture, ret;
    ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_Buffer aNativeWindow_buffer;
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        if (avPacket->stream_index == videoStream_index) {
            ret = avcodec_decode_video2(avCodecContext, yuvFrame, &gotPicture, avPacket);
            if (ret < 0) {
                LOGI("解码失败");
                return;
            }

            if (gotPicture) {
                //设置缓冲区的属性（宽、高、像素格式）
                ANativeWindow_setBuffersGeometry(aNativeWindow, yuvFrame->width, yuvFrame->height,
                                                 WINDOW_FORMAT_RGBA_8888);
                //lock
                ANativeWindow_lock(aNativeWindow, &aNativeWindow_buffer, NULL);
                av_image_fill_arrays(rgb_frame->data, yuvFrame->linesize, aNativeWindow_buffer.bits,
                                     AV_PIX_FMT_YUV420P,
                                     avCodecContext->width, avCodecContext->height, 1);

                //YUV->RGBA_8888
                I420ToARGB(yuvFrame->data[0], yuvFrame->linesize[0],
                           yuvFrame->data[2], yuvFrame->linesize[2],
                           yuvFrame->data[1], yuvFrame->linesize[1],
                           rgb_frame->data[0], rgb_frame->linesize[0],
                           avCodecContext->width, avCodecContext->height);
                //unlock
                ANativeWindow_unlockAndPost(aNativeWindow);

                usleep(1000 * 16);
            }
            av_packet_unref(avPacket);
        }
    }
    ANativeWindow_release(aNativeWindow);
    avformat_free_context(avFormatContext);
    avcodec_free_context(&avCodecContext);
    av_frame_free(&yuvFrame);
    av_frame_free(&rgb_frame);
};
