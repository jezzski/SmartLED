package com.example.smartled.ui.led;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import com.example.smartled.R;
import java.util.ArrayList;

import static com.example.smartled.Schedule.getScheduleNames;
import static com.example.smartled.Schedule.readXML;
import static com.example.smartled.ui.led.LedChannel.writeChannelToXML;

public class LedFragment extends Fragment {

    private LedViewModel ledViewModel;
    ArrayList<String> scheduleNameList;
    ArrayAdapter<String> spinAdapter;
    ArrayList<LedChannel> channelList;
    MultiSpinner LEDSpin1;
    MultiSpinner LEDSpin2;
    MultiSpinner LEDSpin3;
    MultiSpinner LEDSpin4;
    MultiSpinner LEDSpin5;
    MultiSpinner LEDSpin6;
    MultiSpinner RGBSpin1;
    MultiSpinner RGBSpin2;
    Button uploadButton;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        ledViewModel =
                ViewModelProviders.of(this).get(LedViewModel.class);
        View root = inflater.inflate(R.layout.fragment_led, container, false);

        LEDSpin1=root.findViewById(R.id.ledSpinner1);
        LEDSpin2=root.findViewById(R.id.ledSpinner2);
        LEDSpin3=root.findViewById(R.id.ledSpinner3);
        LEDSpin4=root.findViewById(R.id.ledSpinner4);
        LEDSpin5=root.findViewById(R.id.ledSpinner5);
        LEDSpin6=root.findViewById(R.id.ledSpinner6);
        RGBSpin1=root.findViewById(R.id.rgbSpinner1);
        RGBSpin2=root.findViewById(R.id.rgbSpinner2);
        scheduleNameList=getScheduleNames(readXML(root.getContext()));
        scheduleNameList.add(0,"");

        MultiSpinner.MultiSpinnerListener select=new MultiSpinner.MultiSpinnerListener() {
            @Override
            public void onItemsSelected(boolean[] selected) {

            }
        };
        LEDSpin1.setItems(scheduleNameList, "", select);
        LEDSpin2.setItems(scheduleNameList, "", select);
        LEDSpin3.setItems(scheduleNameList, "", select);
        LEDSpin4.setItems(scheduleNameList, "", select);
        LEDSpin5.setItems(scheduleNameList, "", select);
        LEDSpin6.setItems(scheduleNameList, "", select);
        RGBSpin1.setItems(scheduleNameList, "", select);
        RGBSpin2.setItems(scheduleNameList, "", select);

        uploadButton=root.findViewById(R.id.uploadButton);
        uploadButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                channelList=new ArrayList<LedChannel>();
                for(int i=0;i<8;i++){
                    channelList.add(new LedChannel());
                }
                channelList.get(0).setLedChannel(0,LEDSpin1.getSelectedIndexes(),LEDSpin1.getSelectedIndexes().length);
                channelList.get(1).setLedChannel(1,LEDSpin2.getSelectedIndexes(),LEDSpin2.getSelectedIndexes().length);
                channelList.get(2).setLedChannel(2,LEDSpin3.getSelectedIndexes(),LEDSpin3.getSelectedIndexes().length);
                channelList.get(3).setLedChannel(3,LEDSpin4.getSelectedIndexes(),LEDSpin4.getSelectedIndexes().length);
                channelList.get(4).setLedChannel(4,LEDSpin5.getSelectedIndexes(),LEDSpin5.getSelectedIndexes().length);
                channelList.get(5).setLedChannel(5,LEDSpin6.getSelectedIndexes(),LEDSpin6.getSelectedIndexes().length);
                channelList.get(6).setLedChannel(1,RGBSpin1.getSelectedIndexes(),RGBSpin1.getSelectedIndexes().length);
                channelList.get(7).setLedChannel(2,RGBSpin2.getSelectedIndexes(),RGBSpin2.getSelectedIndexes().length);
                writeChannelToXML(v.getContext(),channelList);
            }
        });
        final TextView textView = root.findViewById(R.id.text_led);
        ledViewModel.getText().observe(this, new Observer<String>() {
            @Override
            public void onChanged(@Nullable String s) {
                textView.setText(s);
            }
        });
        return root;
    }
}
