package com.gulei.ffmpegandroid.dialog;

import android.app.Dialog;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;

import com.gulei.ffmpegandroid.R;

/**
 * Created by gl152 on 2018/12/19.
 */

public class LoadingDialog extends Dialog {
    private Context context;
    Handler handler;

    public LoadingDialog(@NonNull Context context) {
        this(context, R.style.LoadingDialog);
    }

    public LoadingDialog(@NonNull Context context, int themeResId) {
        super(context, themeResId);
        this.context = context;
        handler = new Handler(Looper.getMainLooper());
        setContentView(R.layout.loading_dialog);
    }

    public void showOnUiThread() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                show();
            }
        });
    }

    public void dismissOnUiThread() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                dismiss();
            }
        });
    }
}
