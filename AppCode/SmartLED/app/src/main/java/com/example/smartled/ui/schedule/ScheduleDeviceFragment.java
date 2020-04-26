package com.example.smartled.ui.schedule;


import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.Intent;
import android.os.Bundle;

import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.Toast;

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
import static com.example.smartled.ui.schedule.Schedule.getScheduleNames;

/**
 * A simple {@link Fragment} subclass.
 */
public class ScheduleDeviceFragment extends Fragment {
    private ScheduleListAdapter scheduleDeviceAdapter;
    private ListView scheduleDeviceList;
    private ArrayList<String> scheduleDeviceString=new ArrayList<>();
    private ArrayList<Schedule> scheduleDeviceArray=new ArrayList<Schedule>();
    private BluetoothLeService bleService;

    private Switch connectSwitch;
    private ImageButton deleteBtn;

    Toast toast;

    private boolean bleConnectedState=false;
    private int channelPageNum;
    public ScheduleDeviceFragment(int channelNum) {
        channelPageNum=channelNum;
    }

    public static ScheduleDeviceFragment newInstance(int channelNum) {
        Bundle args = new Bundle();
        ScheduleDeviceFragment fragment = new ScheduleDeviceFragment(channelNum);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View root=inflater.inflate(R.layout.fragment_schedule_device, container, false);
        ScheduleViewModel scheduleViewModel = new ViewModelProvider(requireActivity()).get(ScheduleViewModel.class);
        connectSwitch=root.findViewById(R.id.connectSwitchDevice);
        scheduleViewModel.getBleService().observe(getViewLifecycleOwner(), new Observer<BluetoothLeService>() {
            @Override
            public void onChanged(BluetoothLeService bluetoothLeService) {
                bleService=bluetoothLeService;
            }
        });
        scheduleViewModel.getConnectedState().observe(getViewLifecycleOwner(), new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean aBoolean) {
                connectSwitch.setChecked(aBoolean);
                if(aBoolean){
                    bleConnectedState=true;
                    connectSwitch.setText("Connected");
                } else{
                    bleConnectedState=false;
                    connectSwitch.setText("Disconnected");
                }
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
                    toast=Toast.makeText(getContext(), "Lost connection", Toast.LENGTH_SHORT);
                    toast.show();
                }
                connectSwitch.setChecked(aBoolean);
            }
        });
        switch(channelPageNum){
            case 0:
                scheduleViewModel.getSchedulesChannel1().observe(getViewLifecycleOwner(), new Observer<ArrayList<Schedule>>() {
                    @Override
                    public void onChanged(ArrayList<Schedule> schedules) {
                        scheduleDeviceArray=schedules;
                        scheduleDeviceString=getScheduleNames(scheduleDeviceArray);
                        scheduleDeviceAdapter.updateAdapter(scheduleDeviceString);
                    }
                });
                break;
            case 1:
                scheduleViewModel.getSchedulesChannel2().observe(getViewLifecycleOwner(), new Observer<ArrayList<Schedule>>() {
                    @Override
                    public void onChanged(ArrayList<Schedule> schedules) {
                        scheduleDeviceArray=schedules;
                        scheduleDeviceString=getScheduleNames(scheduleDeviceArray);
                        scheduleDeviceAdapter.updateAdapter(scheduleDeviceString);
                    }
                });
                break;
            case 2:
                scheduleViewModel.getSchedulesChannel3().observe(getViewLifecycleOwner(), new Observer<ArrayList<Schedule>>() {
                    @Override
                    public void onChanged(ArrayList<Schedule> schedules) {
                        scheduleDeviceArray=schedules;
                        scheduleDeviceString=getScheduleNames(scheduleDeviceArray);
                        scheduleDeviceAdapter.updateAdapter(scheduleDeviceString);
                    }
                });
                break;
            case 3:
                scheduleViewModel.getSchedulesChannel4().observe(getViewLifecycleOwner(), new Observer<ArrayList<Schedule>>() {
                    @Override
                    public void onChanged(ArrayList<Schedule> schedules) {
                        scheduleDeviceArray=schedules;
                        scheduleDeviceString=getScheduleNames(scheduleDeviceArray);
                        scheduleDeviceAdapter.updateAdapter(scheduleDeviceString);
                    }
                });
                break;
            case 4:
                scheduleViewModel.getSchedulesChannel5().observe(getViewLifecycleOwner(), new Observer<ArrayList<Schedule>>() {
                    @Override
                    public void onChanged(ArrayList<Schedule> schedules) {
                        scheduleDeviceArray=schedules;
                        scheduleDeviceString=getScheduleNames(scheduleDeviceArray);
                        scheduleDeviceAdapter.updateAdapter(scheduleDeviceString);
                    }
                });
                break;
            case 5:
                scheduleViewModel.getSchedulesChannel6().observe(getViewLifecycleOwner(), new Observer<ArrayList<Schedule>>() {
                    @Override
                    public void onChanged(ArrayList<Schedule> schedules) {
                        scheduleDeviceArray=schedules;
                        scheduleDeviceString=getScheduleNames(scheduleDeviceArray);
                        scheduleDeviceAdapter.updateAdapter(scheduleDeviceString);
                    }
                });
                break;
            default:
                break;
        }
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
        scheduleDeviceArray.add(0,new Schedule("default"));
        scheduleDeviceString=getScheduleNames(scheduleDeviceArray);
        scheduleDeviceAdapter = new ScheduleListAdapter(scheduleDeviceString, getActivity());
        scheduleDeviceList = (ListView) root.findViewById(R.id.list_schedule_device);
        scheduleDeviceList.setAdapter(scheduleDeviceAdapter);
        deleteBtn=root.findViewById(R.id.deleteButton);
        deleteBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                List<Boolean> tempcheck=scheduleDeviceAdapter.getCheckedArray();
                for(int i=0;i<scheduleDeviceArray.size();i++){
                    //Log.e("Hunaid","Size: "+String.valueOf(scheduleDeviceArray.size())+"Index: "+i+"Val: "+String.valueOf(tempcheck.get(i)));
                }
                writeCharacteristic();
            }
        });

        return root;
    }

    public boolean writeCharacteristic(){
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

    @Override
    public void onStop() {
        super.onStop();
        if(toast!=null){
            toast.cancel();
        }
    }
}
