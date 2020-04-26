package com.example.smartled.ui.schedule;

import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.Intent;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.viewpager2.adapter.FragmentStateAdapter;
import androidx.viewpager2.widget.ViewPager2;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.example.smartled.BleControlFragment;
import com.example.smartled.BleServices;
import com.example.smartled.BluetoothLeService;
import com.example.smartled.R;
import com.example.smartled.ui.scan.BleDevice;
import com.google.android.material.tabs.TabLayout;
import com.google.android.material.tabs.TabLayoutMediator;

import java.util.ArrayList;

import static android.content.Context.BIND_AUTO_CREATE;
import static com.example.smartled.ui.scan.BleDevice.getActiveDevice;
import static com.example.smartled.ui.scan.BleDevice.readDeviceXML;

public class ScheduleTabbed extends BleControlFragment {
    private ViewPagerAdapter adapter;
    private ViewPager2 viewPager;
    private ScheduleViewModel scheduleViewModel;
    private MutableLiveData<byte[]> bytePacket=new MutableLiveData<byte[]>();
    android.os.Handler customHandler;
    private boolean foundName=false;
    private int channelCounter=0;
    private String savedName;
    private ArrayList<Schedule>[] schedulesOnDevice;

    public ScheduleTabbed() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRetainInstance(true);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View root = inflater.inflate(R.layout.fragment_schedule_tabbed, container, false);
        scheduleViewModel = new ViewModelProvider(requireActivity()).get(ScheduleViewModel.class);
        //Ble connect
        ArrayList<BleDevice> tempDeviceList=readDeviceXML(getContext());
        BleDevice activeDevice=getActiveDevice(tempDeviceList);
        if(activeDevice!=null){
            mDeviceAddress=activeDevice.getDeviceAddress();
        }
        else{
            mDeviceAddress="4C:11:AE:B7:5C:D2";//"4C:11:AE:B7:5D:36"mine
            Toast.makeText(getActivity(), "No active device selected!", Toast.LENGTH_SHORT).show();
        }
        Intent gattServiceIntent = new Intent(root.getContext(), BluetoothLeService.class);
        isBound=getContext().bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

        schedulesOnDevice=(ArrayList<Schedule>[])new ArrayList[6];
        for(int i=0;i<schedulesOnDevice.length;i++){
            schedulesOnDevice[i]=new ArrayList<Schedule>();
        }
        bytePacket.observe(getViewLifecycleOwner(), new Observer<byte[]>() {
            @Override
            public void onChanged(byte[] bytes) {
                if(bytes!=null){
                    if(new String(bytes).contains("Done Read")){
                        Log.d("Hunaid", "onChanged: All schedules found "+ channelCounter);
                        switch(channelCounter){
                            case 0:
                                scheduleViewModel.setSchedulesChannel1(schedulesOnDevice[0]);
                                break;
                            case 1:
                                scheduleViewModel.setSchedulesChannel2(schedulesOnDevice[1]);
                                break;
                            case 2:
                                scheduleViewModel.setSchedulesChannel3(schedulesOnDevice[2]);
                                break;
                            case 3:
                                scheduleViewModel.setSchedulesChannel4(schedulesOnDevice[3]);
                                break;
                            case 4:
                                scheduleViewModel.setSchedulesChannel5(schedulesOnDevice[4]);
                                break;
                            case 5:
                                scheduleViewModel.setSchedulesChannel6(schedulesOnDevice[5]);
                                break;
                        }
                        channelCounter++;
                        if(channelCounter<6){
                            startSchedulesRead(channelCounter);
                        }
                        else{
                            channelCounter=0;
                        }
                    }
                    else if(!foundName){
                        Log.d("Hunaid", "onChanged: "+new String(bytes));
                        savedName=new String(bytes);
                        BluetoothGattService Service = mBluetoothLeService.mBluetoothGatt.getService(BleServices.testService);
                        BluetoothGattCharacteristic readCharac = Service.getCharacteristic(BleServices.testA);
                        mBluetoothLeService.mBluetoothGatt.readCharacteristic(readCharac);
                        foundName=true;
                    }
                    else{
                        Log.d("Hunaid", "onChanged: "+new String(bytes));
                        schedulesOnDevice[channelCounter].add(Schedule.decodeSchedulePacket(bytes, savedName));
                        BluetoothGattService Service = mBluetoothLeService.mBluetoothGatt.getService(BleServices.testService);
                        BluetoothGattCharacteristic readCharac = Service.getCharacteristic(BleServices.testA);
                        mBluetoothLeService.mBluetoothGatt.readCharacteristic(readCharac);
                        foundName=false;
                    }
                }
            }
        });
        return inflater.inflate(R.layout.fragment_schedule_tabbed, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        adapter=new ViewPagerAdapter(this);
        viewPager = view.findViewById(R.id.pager);
        viewPager.setAdapter(adapter);

        TabLayout tabs = (TabLayout) view.findViewById(R.id.tab_layout);
        new TabLayoutMediator(tabs, viewPager,
                new TabLayoutMediator.TabConfigurationStrategy() {
                    @Override
                    public void onConfigureTab(@NonNull TabLayout.Tab tab, int position) {
                        if(position==0){
                            tab.setText("My Schedules");
                        }
                        else if(position==1){
                            tab.setText("RGB Channel 1");
                        }
                        else if(position==2) {
                            tab.setText("RGB Channel 2");
                        }
                        else if(position>2&&position<9){
                            tab.setText("Channel " + (position-2));
                        }
                    }
                }).attach();
        customHandler = new android.os.Handler();
        customHandler.postDelayed(updateTimerThread, 3000);
    }
    private Runnable updateTimerThread = new Runnable()
    {
        public void run()
        {
            customHandler.postDelayed(updateTimerThread,60000);
            startSchedulesRead(channelCounter);
        }
    };

    @Override
    public void ifConnected() {
        scheduleViewModel.setConnectedState(true);
        scheduleViewModel.setBleService(mBluetoothLeService);
    }

    @Override
    public void ifDisconnected() {
        scheduleViewModel.setConnectedState(false);
        scheduleViewModel.setBleService(mBluetoothLeService);
    }

    @Override
    public void ifReadData(Intent intent) {
        bytePacket.setValue(intent.getByteArrayExtra(BluetoothLeService.EXTRA_DATA));
        //Toast.makeText(getActivity(), intent.getStringExtra(BluetoothLeService.EXTRA_DATA), Toast.LENGTH_SHORT).show();
    }

    public class ViewPagerAdapter extends FragmentStateAdapter {
        private static final int PAGE_COUNT = 9;

        public ViewPagerAdapter(@NonNull Fragment fragment) {
            super(fragment);
        }

        @NonNull
        @Override
        public Fragment createFragment(int position) {
            Fragment fragment;
            if(position==0){
                fragment=ScheduleFragment.newInstance();
            }
            else if(position==1){
                fragment=ScheduleDeviceFragment.newInstance(10);
            }
            else if(position==2) {
                fragment=ScheduleDeviceFragment.newInstance(10);
            }
            else if(position>2&&position<9){
                fragment=ScheduleDeviceFragment.newInstance(position-3);
            }
            else{
                fragment=ScheduleDeviceFragment.newInstance(10);
            }
            return fragment;
        }

        @Override
        public int getItemCount() {
            return PAGE_COUNT;
        }

    }

    public boolean startSchedulesRead(int channelNum){
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
        BluetoothGattCharacteristic readCharac = Service.getCharacteristic(BleServices.testA);
        if (readCharac == null) {
            Log.e("Hunaid", "char not found!");
            return false;
        }
        BluetoothGattCharacteristic writeCharac = Service.getCharacteristic(BleServices.testC);
        if (readCharac == null) {
            Log.e("Hunaid", "char not found!");
            return false;
        }
        schedulesOnDevice[channelNum].clear();
        String startString=channelNum+"Begin Read";
        writeCharac.setValue(startString.getBytes());
        boolean status=mBluetoothLeService.mBluetoothGatt.writeCharacteristic(writeCharac);
        while(!status){
            status=mBluetoothLeService.mBluetoothGatt.writeCharacteristic(writeCharac);
        }
        status=mBluetoothLeService.mBluetoothGatt.readCharacteristic(readCharac);
        while(!status){
            status=mBluetoothLeService.mBluetoothGatt.readCharacteristic(readCharac);
        }
        return true;
    }
}
