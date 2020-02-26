package com.example.smartled.ui.scan;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import com.example.smartled.R;

import java.util.ArrayList;

import static com.example.smartled.ui.scan.BleDevice.getDeviceInfo;
import static com.example.smartled.ui.scan.BleDevice.readDeviceXML;
import static com.example.smartled.ui.scan.BleDevice.writeDeviceToXML;

public class ScanFragment extends Fragment {

    private ScanViewModel scanViewModel;
    public static final String EXTRA_MESSAGE = "com.example.smartled.schedulename";


    private String deviceName;
    private String deviceAddress;

    public TextView deviceNameView;
    public TextView deviceAddressView;

    public BleDeviceListAdapter deviceAdapter;
    public ListView deviceList;
    ArrayList<BleDevice> deviceArray=new ArrayList<BleDevice>();

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

        deviceArray=readDeviceXML(getContext());
        if(deviceArray==null){
            deviceArray=new ArrayList<BleDevice>();
            BleDevice tempDevice=new BleDevice();
            tempDevice.setDeviceInfo("D","L");
            deviceArray.add(tempDevice);
        }
        deviceAdapter=new BleDeviceListAdapter(getDeviceInfo(deviceArray),getContext());
        deviceList=(ListView) root.findViewById(R.id.list_device);
        deviceList.setAdapter(deviceAdapter);
        Button scanBtn = (Button) root.findViewById(R.id.scanButton);

        scanBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(v.getContext(), DeviceScanActivity.class);
                //intent.putExtra(EXTRA_MESSAGE, listItemText.getText());
                v.getContext().startActivity(intent);
            }
        });

        Bundle extras = getArguments();
        if (extras != null) {
            deviceName = extras.getString("DeviceName");
            if(deviceName ==null){
                deviceName ="Unknown";
            }
            deviceAddress = extras.getString("DeviceAddress");
            if(deviceAddress==null){
                deviceAddress="Unknown";
            }
            BleDevice tempDevice=new BleDevice();
            tempDevice.setDeviceInfo(deviceName,deviceAddress);
            deviceArray.add(tempDevice);
            writeDeviceToXML(getContext(),deviceArray);
        }

        return root;
    }


}