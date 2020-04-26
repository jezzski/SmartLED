package com.example.smartled.ui.schedule;

import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.lifecycle.ViewModelProviders;

import com.example.smartled.BleControlFragment;
import com.example.smartled.BleServices;
import com.example.smartled.BluetoothLeService;
import com.example.smartled.R;
import com.example.smartled.ui.scan.BleDevice;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import static android.content.Context.BIND_AUTO_CREATE;
import static com.example.smartled.ui.scan.BleDevice.getActiveDevice;
import static com.example.smartled.ui.scan.BleDevice.readDeviceXML;
import static com.example.smartled.ui.schedule.Schedule.createSchedulePacket;
import static com.example.smartled.ui.schedule.Schedule.getScheduleNames;
import static com.example.smartled.ui.schedule.Schedule.readXML;
import static com.example.smartled.ui.schedule.Schedule.writeToXML;

public class ScheduleFragment extends Fragment {

    private ScheduleListAdapter scheduleAdapter;
    private ListView scheduleList;
    private ArrayList<String> scheduleString=new ArrayList<>();
    private ArrayList<Schedule> scheduleArray=new ArrayList<Schedule>();
    private BluetoothLeService bleService;

    ImageButton addButton;
    ImageButton deleteButton;
    ImageButton uploadButton;
    EditText scheduleName;
    Switch connectSwitch;
    Spinner ledChannelSel;

    private boolean bleConnectedState=false;

    public static ScheduleFragment newInstance() {

        Bundle args = new Bundle();

        ScheduleFragment fragment = new ScheduleFragment();
        fragment.setArguments(args);
        return fragment;
    }
    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_schedule, container, false);
        ScheduleViewModel scheduleViewModel = new ViewModelProvider(requireActivity()).get(ScheduleViewModel.class);
        connectSwitch=root.findViewById(R.id.connectSwitchMain);
        scheduleViewModel.getBleService().observe(getViewLifecycleOwner(), new Observer<BluetoothLeService>() {
            @Override
            public void onChanged(BluetoothLeService bluetoothLeService) {
                bleService=bluetoothLeService;
            }
        });
        scheduleViewModel.getConnectedState().observe(getViewLifecycleOwner(), new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean aBoolean) {
                if(aBoolean){
                    bleConnectedState=true;
                    connectSwitch.setText("Connected");
                } else{
                    bleConnectedState=false;
                    connectSwitch.setText("Disconnected");
                    Toast.makeText(getContext(), "Lost connection", Toast.LENGTH_SHORT).show();
                }
                connectSwitch.setChecked(aBoolean);
            }
        });
        connectSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                ArrayList<BleDevice> tempDeviceList=readDeviceXML(getContext());
                BleDevice activeDevice=getActiveDevice(tempDeviceList);
                if(bleService!=null){
                    if(isChecked){
                        if(activeDevice!=null){
                            bleService.connect(activeDevice.getDeviceAddress());
                        }
                        else{
                            Toast.makeText(getContext(), "Select an active device", Toast.LENGTH_SHORT).show();
                        }
                    }else{
                        bleService.disconnect();
                    }
                }
                if(!bleConnectedState){
                    connectSwitch.setChecked(false);
                }
            }
        });
        scheduleArray=readXML(root.getContext());
        scheduleString=getScheduleNames(scheduleArray);

        scheduleAdapter = new ScheduleListAdapter(scheduleString, getActivity());
        scheduleList = (ListView) root.findViewById(R.id.list_schedule);
        scheduleList.setAdapter(scheduleAdapter);

        scheduleName= (EditText) root.findViewById(R.id.scheduleNameText);
        addButton= root.findViewById(R.id.addScheculeButton);
        addButton.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                scheduleArray.add(new Schedule(scheduleName.getText().toString()));
                writeToXML(v.getContext(), scheduleArray);
                scheduleString.add(scheduleName.getText().toString());
                scheduleAdapter.updateAdapter(scheduleString);
            }
        });
        ledChannelSel = (Spinner) root.findViewById(R.id.ledChannelSpinner);
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(root.getContext(), R.array.channel_Array, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        ledChannelSel.setAdapter(adapter);

        uploadButton=root.findViewById(R.id.uploadButtonMain);
        uploadButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            List<Boolean> tempCheck=scheduleAdapter.getCheckedArray();
            ArrayList<Schedule> schedules=readXML(getContext());
            boolean isDeleted=false;
            switch (ledChannelSel.getSelectedItem().toString()) {
                case "RGB Channel 1":
                    isDeleted=writeCharacteristicSchedules(schedules,tempCheck,0);
                    break;
                case "RGB Channel 2":
                    isDeleted=writeCharacteristicSchedules(schedules,tempCheck,1);
                    break;
                case "Channel 1":
                    isDeleted=writeCharacteristicSchedules(schedules,tempCheck,0);
                    break;
                case "Channel 2":
                    isDeleted=writeCharacteristicSchedules(schedules,tempCheck,1);
                    break;
                case "Channel 3":
                    isDeleted=writeCharacteristicSchedules(schedules,tempCheck,2);
                    break;
                case "Channel 4":
                    isDeleted=writeCharacteristicSchedules(schedules,tempCheck,3);
                    break;
                case "Channel 5":
                    isDeleted=writeCharacteristicSchedules(schedules,tempCheck,4);
                    break;
                case "Channel 6":
                    isDeleted=writeCharacteristicSchedules(schedules,tempCheck,5);
                    break;
                default:
                    Toast.makeText(getContext(), "Did not select a channel to upload", Toast.LENGTH_SHORT).show();
            }
            if(isDeleted){
                Toast.makeText(getContext(), "Schedules uploaded", Toast.LENGTH_SHORT).show();
            }
            else{
                Toast.makeText(getContext(), "Schedule upload fail", Toast.LENGTH_SHORT).show();
            }
            }
        });

        deleteButton=root.findViewById(R.id.deleteButtonMain);
        deleteButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                List<Boolean> tempCheck=scheduleAdapter.getCheckedArray();
                int scheduleCounter=0;
                for(int i=0;i<tempCheck.size();i++){
                    if(tempCheck.get(i)){
                        scheduleArray.remove(scheduleCounter);
                        scheduleCounter--;
                    }
                    scheduleCounter++;
                }
                writeToXML(v.getContext(), scheduleArray);
                scheduleString=getScheduleNames(scheduleArray);
                scheduleAdapter.updateAdapter(scheduleString);
            }
        });
        return root;
    }

    public void onResume(){
        super.onResume();

    }

    public boolean writeCharacteristicTime() {
        if (bleService == null) {
            Log.e("Hunaid", "lost connection");
            return false;
        }
        if (bleService.mBluetoothGatt == null) {
            Log.e("Hunaid", "lost connection");
            return false;
        }
        BluetoothGattService Service = bleService.mBluetoothGatt.getService(BleServices.testService);
        if (Service == null) {
            Log.e("Hunaid", "service not found!");
            return false;
        }
        BluetoothGattCharacteristic charac = Service.getCharacteristic(BleServices.testB);
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
        bleService.mBluetoothGatt.writeCharacteristic(charac);
        return true;
    }
    public boolean writeCharacteristicSchedules(ArrayList<Schedule> scheduleList, List<Boolean> schedulesChecked, int channelNum){
        if (bleService == null) {
            Log.e("Hunaid", "lost connection");
            return false;
        }
        if (bleService.mBluetoothGatt == null) {
            Log.e("Hunaid", "lost connection");
            return false;
        }
        BluetoothGattService Service = bleService.mBluetoothGatt.getService(BleServices.testService);
        if (Service == null) {
            Log.e("Hunaid", "service not found!");
            return false;
        }
        BluetoothGattCharacteristic charac1 = Service.getCharacteristic(BleServices.testC);
        BluetoothGattCharacteristic charac2 = Service.getCharacteristic(BleServices.testB);
        if (charac2 == null) {
            Log.e("Hunaid", "char not found!");
            return false;
        }
        byte[] val=null;
        for(int i=0;i<scheduleList.size();i++){
            if(schedulesChecked.get(i)){
                val=scheduleList.get(i).name.getBytes();;
                charac1.setValue(val);
                boolean status=bleService.mBluetoothGatt.writeCharacteristic(charac1);
                while(!status){
                    status=bleService.mBluetoothGatt.writeCharacteristic(charac1);
                }
                val=createSchedulePacket(scheduleList.get(i),channelNum);
                charac2.setValue(val);
                status=bleService.mBluetoothGatt.writeCharacteristic(charac2);
                while(!status){
                    status=bleService.mBluetoothGatt.writeCharacteristic(charac2);
                }
            }
        }
    return true;
    }
}