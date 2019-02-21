package com.gulei.ffmpegandroid.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.gulei.ffmpegandroid.R;
import com.gulei.ffmpegandroid.camera.CameraHelper;

public class RecoderVideoActivity extends AppCompatActivity {

    private CameraHelper cameraHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_recoder_video);
        cameraHelper = new CameraHelper(this);

    }
}
