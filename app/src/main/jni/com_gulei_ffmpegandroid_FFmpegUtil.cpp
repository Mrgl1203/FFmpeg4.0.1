//
// Created by gl152 on 2018/12/19.
//

#include <jni.h>
#include <string.h>
#include "android/log.h"

extern "C" {
#include "ffmpeg.h"
}
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , "ffmpeg", __VA_ARGS__)

extern "C"
JNIEXPORT void JNICALL Java_com_gulei_ffmpegandroid_FFmpegUtil_run
        (JNIEnv *env, jclass type, jint cmdLen, jobjectArray cmd) {
    char *argCmd[cmdLen];
    jstring buf[cmdLen];
    LOGD("length=%d", cmdLen);
    for (int i = 0; i < cmdLen; ++i) {
        buf[i] = static_cast<jstring>(env->GetObjectArrayElement(cmd, i));
        char *string = const_cast<char *>(env->GetStringUTFChars(buf[i], JNI_FALSE));
        argCmd[i] = string;
        LOGD("argCmd=%s", argCmd[i]);
    }
    ffmpeg_exec(cmdLen, argCmd);
};