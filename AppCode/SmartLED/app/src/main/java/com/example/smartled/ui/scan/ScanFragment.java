package com.example.smartled.ui.scan;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import com.example.smartled.R;
import com.example.smartled.ui.schedule.ScheduleEditorActivity;

public class ScanFragment extends Fragment {

    private ScanViewModel scanViewModel;
    public static final String EXTRA_MESSAGE = "com.example.smartled.schedulename";

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        scanViewModel =
                ViewModelProviders.of(this).get(ScanViewModel.class);
        View root = inflater.inflate(R.layout.fragment_scan, container, false);
        final TextView textView = root.findViewById(R.id.text_scan);
        scanViewModel.getText().observe(this, new Observer<String>() {
            @Override
            public void onChanged(@Nullable String s) {
                textView.setText(s);
            }
        });

        Button scanBtn = (Button) root.findViewById(R.id.scanButton);

        scanBtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(v.getContext(), DeviceScanActivity.class);
                //intent.putExtra(EXTRA_MESSAGE, listItemText.getText());
                v.getContext().startActivity(intent);
            }
        });
        return root;
    }
}