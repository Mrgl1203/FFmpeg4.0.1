package com.gulei.ffmpegandroid;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.gulei.ffmpegandroid.activity.RenderActivity;
import com.gulei.ffmpegandroid.adapter.MainRecAdapter;
import com.gulei.ffmpegandroid.dialog.LoadingDialog;
import com.yanzhenjie.permission.Action;
import com.yanzhenjie.permission.AndPermission;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import butterknife.BindView;
import butterknife.ButterKnife;

public class MainActivity extends AppCompatActivity {
    @BindView(R.id.recyclerView)
    RecyclerView recyclerView;
    MainRecAdapter adapter;

    List<String> data = new ArrayList<>();
    String input, output, rootDir;
    LoadingDialog loadingDialog;
    ExecutorService pool = Executors.newSingleThreadExecutor();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);
        AndPermission.with(this)
                .runtime()
                .permission(new String[]{
                        Manifest.permission.WRITE_EXTERNAL_STORAGE,
                        Manifest.permission.CAMERA,
                        Manifest.permission.RECORD_AUDIO})
                .onGranted(new Action<List<String>>() {
                    @Override
                    public void onAction(List<String> data) {

                    }
                })
                .onDenied(new Action<List<String>>() {
                    @Override
                    public void onAction(List<String> data) {
                        finish();
                    }
                }).start();

        initData();
        loadingDialog = new LoadingDialog(this);
        recyclerView.setLayoutManager(new LinearLayoutManager(this, LinearLayoutManager.VERTICAL, false));
        adapter = new MainRecAdapter();
        adapter.setNewData(data);
        recyclerView.setAdapter(adapter);
        adapter.setOnItemClickListener(new BaseQuickAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(BaseQuickAdapter adapter, View view, int position) {
                if (position == 0) {
                    ffmpegCut(input, output);
                } else if (position == 1) {
                    ffmpegGif(input, rootDir + "a.gif");
                } else if (position == 2) {
                    ApiPlayer.decodeVideo(input, rootDir + "decode.yuv");
                } else if (position == 3) {
                    ApiPlayer.decodeVideo2(input, rootDir + "decode2.yuv");
                } else if (position == 4) {
                    Intent intent = new Intent(MainActivity.this, RenderActivity.class);
                    intent.putExtra(Constant.VIDEOPATH, input);
                    startActivity(intent);
                }
            }
        });

        Toast.makeText(this, ApiPlayer.jnitest(), Toast.LENGTH_LONG).show();
    }


    private void initData() {
        rootDir = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg" + File.separator;
        input = rootDir + "Put1.mp4";
        output = rootDir + "Out1.mp4";
        data.add("剪切视频");
        data.add("Gif");
        data.add("视频解码");
        data.add("视频解码2:新api");
        data.add("视频Native原生绘制");
    }


    public interface ThreadActionListener {
        void doAction();
    }

    private void doOnThread(final ThreadActionListener listener) {
        pool.execute(new Runnable() {
            @Override
            public void run() {
                loadingDialog.showOnUiThread();
                if (listener != null) {
                    long startTime = System.currentTimeMillis();
                    listener.doAction();
                    Log.d("FFmpegTest", "run: 耗时：" + (System.currentTimeMillis() - startTime));
                }
                loadingDialog.dismissOnUiThread();
            }
        });
    }

    public void deleteExitFile(String path) {
        File file = new File(path);
        if (file.exists()) {
            file.delete();
//            Toast.makeText(this, "删除成功", Toast.LENGTH_SHORT).show();
        } else {
//            Toast.makeText(this, "文件不存在", Toast.LENGTH_SHORT).show();
        }
    }

    private void ffmpegCut(final String in, final String out) {
        deleteExitFile(out);
        doOnThread(new ThreadActionListener() {
            @Override
            public void doAction() {
                //剪切视频从00：20-00：28的片段
                String cmd = "ffmpeg -d -ss 00:00:05 -t 00:00:10 -i %s -vcodec copy -acodec copy %s";
                cmd = String.format(cmd, in, out);
                FFmpegUtil.run(cmd.split(" "));
            }
        });
    }

    private void ffmpegGif(final String in, final String out) {
        doOnThread(new ThreadActionListener() {
            @Override
            public void doAction() {
                String cmd = "ffmpeg -i %s -vframes 30 -y -f gif %s";
                cmd = String.format(cmd, in, out);
                FFmpegUtil.run(cmd.split(" "));
            }
        });
    }


}