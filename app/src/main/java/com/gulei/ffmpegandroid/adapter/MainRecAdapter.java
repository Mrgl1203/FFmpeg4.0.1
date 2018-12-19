package com.gulei.ffmpegandroid.adapter;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.BaseViewHolder;
import com.gulei.ffmpegandroid.R;

/**
 * Created by gl152 on 2018/12/19.
 */

public class MainRecAdapter extends BaseQuickAdapter<String, BaseViewHolder> {
    public MainRecAdapter() {
        super(R.layout.main_rec_item, null);
    }

    @Override
    protected void convert(BaseViewHolder helper, String item) {
        helper.setText(R.id.tvContent, item);
    }
}
