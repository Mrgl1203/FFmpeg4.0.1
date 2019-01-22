package com.gulei.ffmpegandroid.activity;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.Button;

import com.gulei.ffmpegandroid.Constant;
import com.gulei.ffmpegandroid.R;
import com.gulei.ffmpegandroid.ui.VideoPlayer;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class RenderActivity extends AppCompatActivity {

    @BindView(R.id.videoPlayer)
    VideoPlayer videoPlayer;
    private String path;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_render);
        ButterKnife.bind(this);
        path = getIntent().getStringExtra(Constant.VIDEOPATH);
    }

    @OnClick(R.id.but_Start)
    public void onViewClicked() {
        videoPlayer.setVideoPath(path);
        videoPlayer.start();
    }
}
