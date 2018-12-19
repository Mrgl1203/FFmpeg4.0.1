package com.gulei.ffmpegandroid;

/**
 * Created by gl152 on 2018/12/19.
 */

public class FFmpegUtil {
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

        System.loadLibrary("FFmpegUtil");
    }

    public static native void run(int len, String[] cmd);

    public static void run(String[] cmd) {
        run(cmd.length, cmd);
    }
}
