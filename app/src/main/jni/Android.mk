
LOCAL_PATH := $(call my-dir)

#ffmpeg lib
include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := libavcodec-58.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avdevice
LOCAL_SRC_FILES := libavdevice-58.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avfilter
LOCAL_SRC_FILES := libavfilter-7.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := libavformat-58.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := libavutil-56.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := postproc
LOCAL_SRC_FILES := libpostproc-55.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := libswresample-3.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swscale
LOCAL_SRC_FILES := libswscale-5.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := yuv
LOCAL_SRC_FILES := libyuv.so
include $(PREBUILT_SHARED_LIBRARY)

#myapp
include $(CLEAR_VARS)
LOCAL_MODULE := FFmpegUtil
LOCAL_SRC_FILES := com_gulei_ffmpegandroid_FFmpegUtil.cpp\
                 cmdutils.c \
                 ffmpeg_filter.c \
                 ffmpeg_opt.c \
                 ffmpeg_hw.c \
                 ffmpeg.c
LOCAL_C_INCLUDES := F:\FFmpeg\FFmpeg-n4.0.1
LOCAL_LDLIBS := -llog -landroid
LOCAL_SHARED_LIBRARIES := avcodec avdevice avfilter avformat avutil postproc swresample swscale yuv
include $(BUILD_SHARED_LIBRARY)

#test
include $(CLEAR_VARS)
LOCAL_MODULE := ApiPlayer
#需要用到的本地c头文件路径前缀
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ffmpeg
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/libyuv
LOCAL_SRC_FILES := com_gulei_ffmpegandroid_ApiPlayer.c
LOCAL_LDLIBS := -llog -landroid
LOCAL_SHARED_LIBRARIES := avcodec avdevice avfilter avformat avutil postproc swresample swscale yuv
include $(BUILD_SHARED_LIBRARY)
