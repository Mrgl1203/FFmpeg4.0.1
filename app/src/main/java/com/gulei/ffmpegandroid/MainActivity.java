package com.gulei.ffmpegandroid;

import android.Manifest;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

import com.chad.library.adapter.base.BaseQuickAdapter;
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
import butterknife.OnClick;

public class MainActivity extends AppCompatActivity {
    @BindView(R.id.but_delete)
    Button butDelete;
    @BindView(R.id.lineatTop)
    LinearLayout lineatTop;
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
                .permission(Manifest.permission.WRITE_EXTERNAL_STORAGE)
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
                    ffmpegCut();
                }else if (position==1){
                    ffmpegGif();
                }
            }
        });
    }

    private void initData() {
        rootDir = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg" + File.separator;
        input = rootDir + "Put1.mp4";
        output = rootDir + "Out1.mp4";

        data.add("剪切视频");
        data.add("Gif");
    }

    @OnClick(R.id.but_delete)
    public void onViewClicked() {
        deleteExitFile(output);
    }

    public interface ThreadActionListener {
        void doAction();
    }

    private void doOnThread(final ThreadActionListener listener) {
        pool.execute(new Runnable() {
            @Override
            public void run() {
                loadingDialog.showOnUiThread();
                deleteExitFile(output);
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

    private void ffmpegCut() {
        doOnThread(new ThreadActionListener() {
            @Override
            public void doAction() {
                //剪切视频从00：20-00：28的片段
                String cmd = "ffmpeg -d -ss 00:00:05 -t 00:00:10 -i %s -vcodec copy -acodec copy %s";
                cmd = String.format(cmd, input, output);
                FFmpegUtil.run(cmd.split(" "));
            }
        });
    }

    private void ffmpegGif() {
        doOnThread(new ThreadActionListener() {
            @Override
            public void doAction() {
                String cmd = "ffmpeg -i %s -vframes 30 -y -f gif %s";
                cmd=String.format(cmd,input,rootDir+"a.gif");
                FFmpegUtil.run(cmd.split(" "));
            }
        });
    }


}