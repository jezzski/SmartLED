package com.example.smartled.ui.led;

import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import com.example.smartled.BleControlFragment;
import com.example.smartled.BleServices;
import com.example.smartled.R;
import com.example.smartled.ui.scan.BleDevice;
import com.example.smartled.ui.schedule.Schedule;
import com.example.smartled.HSVColorPickerDialog;
import com.example.smartled.BluetoothLeService;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.UUID;

import static android.content.Context.BIND_AUTO_CREATE;
import static com.example.smartled.ui.scan.BleDevice.getActiveDevice;
import static com.example.smartled.ui.scan.BleDevice.readDeviceXML;

public class LedFragment extends BleControlFragment {

    private LedViewModel ledViewModel;

    Button uploadButton;
    Button rgbButton1;
    Button rgbButton2;

    SeekBar ledBar1;
    SeekBar ledBar2;
    SeekBar ledBar3;
    SeekBar ledBar4;
    SeekBar ledBar5;
    SeekBar ledBar6;
    SeekBar rgbBar1;
    SeekBar rgbBar2;

    CheckBox enRGB1;
    CheckBox enRGB2;
    Switch connectSwitch;

    byte[] brightnessArray;
    int[] colorArray;
    boolean[] isCheckedArray;

    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        ledViewModel = ViewModelProviders.of(this).get(LedViewModel.class);
        View root = inflater.inflate(R.layout.fragment_led, container, false);

        setHasOptionsMenu(true);
        //Ble connect
        ArrayList<BleDevice> tempDeviceList=readDeviceXML(getContext());
        BleDevice activeDevice=getActiveDevice(tempDeviceList);
        if(activeDevice!=null){
            mDeviceAddress=activeDevice.getDeviceAddress();

        }
        else{
            mDeviceAddress="4C:11:AE:B7:5C:D2";//"4C:11:AE:B7:5D:36"mine ***remember to null this
            Toast.makeText(getActivity(), "No active device selected!", Toast.LENGTH_SHORT).show();
        }
        Intent gattServiceIntent = new Intent(root.getContext(), BluetoothLeService.class);
        isBound=getContext().bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

        ledBar1=root.findViewById(R.id.seekBar);
        ledBar2=root.findViewById(R.id.seekBar2);
        ledBar3=root.findViewById(R.id.seekBar3);
        ledBar4=root.findViewById(R.id.seekBar4);
        ledBar5=root.findViewById(R.id.seekBar5);
        ledBar6=root.findViewById(R.id.seekBar6);
        rgbBar1=root.findViewById(R.id.rgbBar1);
        rgbBar2=root.findViewById(R.id.rgbBar2);

        enRGB1=root.findViewById(R.id.checkBox);
        enRGB2=root.findViewById(R.id.checkBox2);
        rgbButton1=root.findViewById(R.id.rgbbutton);
        rgbButton2=root.findViewById(R.id.rgbbutton2);

        connectSwitch=root.findViewById(R.id.connectSwitch);

        brightnessArray=new byte[8];
        colorArray=new int[2];
        isCheckedArray=new boolean[2];

        setSeekBarListener(ledBar1,1);
        setSeekBarListener(ledBar2,2);
        setSeekBarListener(ledBar3,3);
        setSeekBarListener(ledBar4,4);
        setSeekBarListener(ledBar5,5);
        setSeekBarListener(ledBar6,6);
        setSeekBarListener(rgbBar1,7);
        setSeekBarListener(rgbBar2,8);

        enRGB1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
           public void onCheckedChanged(CompoundButton buttonView,boolean isChecked) {
                isCheckedArray[0]=isChecked;
                if(isChecked){
                    rgbButton1.setVisibility(View.VISIBLE);
                    rgbBar1.setVisibility(View.VISIBLE);
                }
                else{
                    rgbButton1.setVisibility(View.INVISIBLE);
                    rgbBar1.setVisibility(View.INVISIBLE);
                }
           }
        });

        enRGB2.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,boolean isChecked) {
                isCheckedArray[1]=isChecked;
                if(isChecked){
                    rgbButton2.setVisibility(View.VISIBLE);
                    rgbBar2.setVisibility(View.VISIBLE);
                }
                else{
                    rgbButton2.setVisibility(View.INVISIBLE);
                    rgbBar2.setVisibility(View.INVISIBLE);
                }
            }
        });



        rgbButton1.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                HSVColorPickerDialog cpd = new HSVColorPickerDialog(v.getContext(), 0xFF4488CC, new HSVColorPickerDialog.OnColorSelectedListener() {
                    @Override
                    public void colorSelected(Integer color) {
                        rgbButton1.setBackgroundColor(color);
                        colorArray[0]=color;
                        writeCharacteristic(7);
                    }
                });
                cpd.setTitle( "Pick a color" );
                cpd.setNoColorButton( R.string.no_color );
                cpd.show();
            }
        });

        rgbButton2.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                HSVColorPickerDialog cpd = new HSVColorPickerDialog(v.getContext(), 0xFF4488CC, new HSVColorPickerDialog.OnColorSelectedListener() {
                    @Override
                    public void colorSelected(Integer color) {
                        rgbButton2.setBackgroundColor(color);
                        colorArray[1]=color;
                        writeCharacteristic(8);
                    }
                });
                cpd.setTitle( "Pick a color" );
                cpd.setNoColorButton( R.string.no_color );
                cpd.show();
            }
        });

        connectSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked){
                    mBluetoothLeService.connect(mDeviceAddress);
                    connectSwitch.setText("Connected");
                }
                else{
                    mBluetoothLeService.disconnect();
                    connectSwitch.setText("Disconnected");
                }
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

    public void setSeekBarListener(SeekBar sB, int channelNumber){
        final int chanNum=channelNumber;
        sB.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser) {
                // TODO Auto-generated method stub
                brightnessArray[chanNum-1]=(byte)progress;
                writeCharacteristic(chanNum);
            }
        });
    }

    public boolean writeCharacteristic(int channelNum){
        if(mBluetoothLeService==null){
            Log.e("Hunaid", "lost connection");
            return false;
        }
        if (mBluetoothLeService.mBluetoothGatt == null) {
            Log.e("Hunaid", "lost connection");
            return false;
        }
        BluetoothGattService Service = mBluetoothLeService.mBluetoothGatt.getService(BleServices.testService);
        if (Service == null) {
            Log.e("Hunaid", "service not found!");
            return false;
        }
        BluetoothGattCharacteristic charac3 = Service.getCharacteristic(BleServices.testB);
        if (charac3 == null) {
            Log.e("Hunaid", "char not found!");
            return false;
        }
        byte[] temp2=new byte[7];
        temp2[0]=(byte)((channelNum-1)%6);
        temp2[1]=(byte) 0xff;
        temp2[3]=(byte)brightnessArray[channelNum-1];
        if(channelNum>6){//possibly use isCheckedArray instead of ust 7 and 8 (channelNum) for rgbchannel
            temp2[2]=1;
            temp2[4] = (byte) ((colorArray[channelNum-7] & 0xFF0000) >> 16);
            temp2[5] = (byte) ((colorArray[channelNum-7] & 0xFF00) >> 8);
            temp2[6] = (byte) (colorArray[channelNum-7] & 0xFF);
        }
        else{
            temp2[2]=0;
        }
        charac3.setValue(temp2);
        mBluetoothLeService.mBluetoothGatt.writeCharacteristic(charac3);
        return true;
    }

    @Override
    public void ifConnected() {
        connectSwitch.setChecked(true);
        connectSwitch.setText("Connected");
    }

    @Override
    public void ifDisconnected() {
        connectSwitch.setChecked(false);
        connectSwitch.setText("Disconnected");
    }

    @Override
    public void ifReadData(Intent intent) {

    }
}

