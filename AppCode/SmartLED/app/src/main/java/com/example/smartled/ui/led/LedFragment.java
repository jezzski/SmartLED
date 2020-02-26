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
import static com.example.smartled.ui.schedule.Schedule.createSchedulePacket;
import static com.example.smartled.ui.schedule.Schedule.getScheduleNames;
import static com.example.smartled.ui.schedule.Schedule.readXML;
import static com.example.smartled.ui.led.LedChannel.readChannelXML;
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
    //ble variables
    private TextView mConnectionState;
    private TextView mDataField;
    private String mDeviceName;
    private String mDeviceAddress;
    private BluetoothLeService mBluetoothLeService;
    private boolean mConnected = false;

    boolean isBound=false;

    private UUID testService=UUID.fromString("000000ff-0000-1000-8000-00805f9b34fb");
    private UUID testB= UUID.fromString("0000ff02-0000-1000-8000-00805f9b34fb");
    private UUID testC=UUID.fromString("0000ff03-0000-1000-8000-00805f9b34fb");

    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        ledViewModel = ViewModelProviders.of(this).get(LedViewModel.class);
        View root = inflater.inflate(R.layout.fragment_led, container, false);

        setHasOptionsMenu(true);

        ArrayList<BleDevice> tempDeviceList=readDeviceXML(getContext());
        BleDevice activeDevice=getActiveDevice(tempDeviceList);
        if(activeDevice!=null){
            mDeviceAddress=activeDevice.getDeviceAddress();
            Log.e("Hunaid", mDeviceAddress+"");

        }
        else{
            mDeviceAddress="4C:11:AE:B7:5C:D2";//"4C:11:AE:B7:5D:36"mine
            Toast.makeText(getActivity(), "No active device selected!", Toast.LENGTH_SHORT).show();
        }

        Intent gattServiceIntent = new Intent(root.getContext(), BluetoothLeService.class);
        isBound=getContext().bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

        LEDSpin1=root.findViewById(R.id.ledSpinner1);
        LEDSpin2=root.findViewById(R.id.ledSpinner2);
        LEDSpin3=root.findViewById(R.id.ledSpinner3);
        LEDSpin4=root.findViewById(R.id.ledSpinner4);
        LEDSpin5=root.findViewById(R.id.ledSpinner5);
        LEDSpin6=root.findViewById(R.id.ledSpinner6);
        RGBSpin1=root.findViewById(R.id.rgbSpinner1);
        RGBSpin2=root.findViewById(R.id.rgbSpinner2);

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
        uploadButton=root.findViewById(R.id.uploadButton);

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

        enRGB1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
           public void onCheckedChanged(CompoundButton buttonView,boolean isChecked) {
                isCheckedArray[0]=isChecked;
                if(isChecked){
                    rgbButton1.setVisibility(View.VISIBLE);
                    rgbBar1.setVisibility(View.VISIBLE);
                    RGBSpin1.setVisibility(View.VISIBLE);
                }
                else{
                    rgbButton1.setVisibility(View.INVISIBLE);
                    rgbBar1.setVisibility(View.INVISIBLE);
                    RGBSpin1.setVisibility(View.INVISIBLE);
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
                    RGBSpin2.setVisibility(View.VISIBLE);
                }
                else{
                    rgbButton2.setVisibility(View.INVISIBLE);
                    rgbBar2.setVisibility(View.INVISIBLE);
                    RGBSpin2.setVisibility(View.INVISIBLE);
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
                        writeCharacteristic(3,7);
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
                        writeCharacteristic(3,8);
                    }
                });
                cpd.setTitle( "Pick a color" );
                cpd.setNoColorButton( R.string.no_color );
                cpd.show();
            }
        });

        uploadButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                writeCharacteristic(1,9);//no channel
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
                channelList.get(6).setLedChannel(0,RGBSpin1.getSelectedIndexes(),RGBSpin1.getSelectedIndexes().length);
                channelList.get(7).setLedChannel(1,RGBSpin2.getSelectedIndexes(),RGBSpin2.getSelectedIndexes().length);
                writeChannelToXML(v.getContext(),channelList);
                writeCharacteristic(2,9);//no channel
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
                writeCharacteristic(3,chanNum);
            }
        });
    }





    //ble code
    public boolean writeCharacteristic(int packetType, int channelNum){
        if (mBluetoothLeService.mBluetoothGatt == null) {
            Log.e("Hunaid", "lost connection");
            return false;
        }
        BluetoothGattService Service = mBluetoothLeService.mBluetoothGatt.getService(testService);
        if (Service == null) {
            List<BluetoothGattService> gattServices=mBluetoothLeService.getSupportedGattServices();
            Log.e("Hunaid", "service not found!");
            return false;
        }
        switch(packetType){
            case 1://time
                BluetoothGattCharacteristic charac = Service.getCharacteristic(testB);
                if (charac == null) {
                    Log.e("Hunaid", "char not found!");
                    return false;
                }
                byte[] temp=new byte[5];
                temp[0]=(byte) 0xff;
                Calendar calendar=Calendar.getInstance();
                int time=(int)(calendar.getTimeInMillis()/1000);
                ByteBuffer bBuffer=ByteBuffer.allocate(4);
                byte[] result=bBuffer.array();
                bBuffer.putInt(time);
                temp[1]=result[0];
                temp[2]=result[1];
                temp[3]=result[2];
                temp[4]=result[3];
                charac.setValue(temp);
                mBluetoothLeService.mBluetoothGatt.writeCharacteristic(charac);
                break;
            case 2://schedule
                BluetoothGattCharacteristic charac2 = Service.getCharacteristic(testC);
                if (charac2 == null) {
                    Log.e("Hunaid", "char not found!");
                    return false;
                }
                byte[] val=null;
                ArrayList<LedChannel> channelList=readChannelXML(getContext());
                ArrayList<Schedule> scheduleList=readXML(getContext());
                for(int i=0;i<channelList.size();i++){
                    LedChannel tempChannel=channelList.get(i);
                    for(int j=0;j<tempChannel.scheduleCount;j++){
                        if(tempChannel.scheduleIndexes[j]==0){

                        }
                        else{
                            val=createSchedulePacket(scheduleList.get(tempChannel.scheduleIndexes[j]-1),tempChannel.channelNum);
                            charac2.setValue(val);
                            boolean status=mBluetoothLeService.mBluetoothGatt.writeCharacteristic(charac2);
                            while(!status){
                                status=mBluetoothLeService.mBluetoothGatt.writeCharacteristic(charac2);
                            }
                        }
                    }
                }
                break;
            case 3://direct control
                BluetoothGattCharacteristic charac3 = Service.getCharacteristic(testB);
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
                break;
            default:
                Log.e("Hunaid", "invalid packet type");
                break;
        }
        return true;
    }

    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e("Hunaid", "Unable to initialize Bluetooth");
                //finish();
            }
            else {
                // Automatically connects to the device upon successful start-up initialization.
                if(mBluetoothLeService.connect(mDeviceAddress)){
                    Log.e("Hunaid", "Initialize Bluetooth");
                }
                else{
                    Log.e("Hunaid", "Failed to Initialize Bluetooth");
                }
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    private void updateConnectionState(final int resourceId) {
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mConnectionState.setText(resourceId);
            }
        });
    }

    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                //mConnected = true;
                //updateConnectionState(R.string.connected);
                //getActivity().invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                //mConnected = false;
                //updateConnectionState(R.string.disconnected);
                //getActivity().invalidateOptionsMenu();
                //clearUI();
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.
                //displayGattServices(mBluetoothLeService.getSupportedGattServices());
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                //displayData(intent.getStringExtra(BluetoothLeService.EXTRA_DATA));
            }
        }
    };

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

    @Override
    public void onResume() {
        super.onResume();
        getContext().registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            Log.d("Hunaid", "Connect request result=" + result);
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        getContext().unregisterReceiver(mGattUpdateReceiver);
    }
    @Override
    public void onDestroy() {
        super.onDestroy();
        mBluetoothLeService.disconnect();
        if(isBound){
            getContext().unbindService(mServiceConnection);
        }
        mBluetoothLeService = null;
    }
}

