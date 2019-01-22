package com.gulei.ffmpegandroid.ui;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.gulei.ffmpegandroid.ApiPlayer;

/**
 * Created by gl152 on 2019/1/7.
 */

public class VideoPlayer extends SurfaceView implements SurfaceHolder.Callback{
    private ApiPlayer playerControll;
    private String path;
    private SurfaceHolder holder;
    private Context context;

    public VideoPlayer(Context context) {
        this(context, null);
    }

    public VideoPlayer(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public VideoPlayer(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    private void init(Context context) {
        this.context = context;
        //初始化，SufaceView绘制的像素格式
        holder = getHolder();
        holder.addCallback(this);
        holder.setFormat(PixelFormat.RGBA_8888);
        playerControll = new ApiPlayer();
    }

    public void setVideoPath(String path) {
        this.path = path;
    }

    public void start() {
        playerControll.render(path, holder.getSurface());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
