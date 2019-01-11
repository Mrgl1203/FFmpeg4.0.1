package com.gulei.ffmpegandroid;

import android.view.Surface;

/**
 * Created by gl152 on 2019/1/3.
 */

public class ApiPlayer {

    static {
        //虽然mk文件的name没有版本号  但是还是以复制过来的so文件文字为准
        System.loadLibrary("avutil-56");
        System.loadLibrary("swresample-3");
        System.loadLibrary("avcodec-58");
        System.loadLibrary("avformat-58");
        System.loadLibrary("swscale-5");
        System.loadLibrary("postproc-55");
        System.loadLibrary("avfilter-7");
        System.loadLibrary("avdevice-58");

        System.loadLibrary("ApiPlayer");
    }

    public static native String jnitest();

    public static native void decodeVideo(String inputPath, String outputPath);

    public static native void decodeVideo2(String inputPath, String outputPath);

    public  native void render(String input, Surface surface);
}
