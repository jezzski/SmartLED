package com.example.smartled.ui.schedule;

import android.util.Log;
import android.widget.ListAdapter;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.example.smartled.BluetoothLeService;

import java.util.ArrayList;
import java.util.List;

public class ScheduleViewModel extends ViewModel {
    private MutableLiveData<Boolean> connected;
    private MutableLiveData<BluetoothLeService> sharedBleService;
    private MutableLiveData<ArrayList<Schedule>> schedulesChannel1;
    private MutableLiveData<ArrayList<Schedule>> schedulesChannel2;
    private MutableLiveData<ArrayList<Schedule>> schedulesChannel3;
    private MutableLiveData<ArrayList<Schedule>> schedulesChannel4;
    private MutableLiveData<ArrayList<Schedule>> schedulesChannel5;
    private MutableLiveData<ArrayList<Schedule>> schedulesChannel6;

    public ScheduleViewModel() {
        connected = new MutableLiveData<>();
        sharedBleService = new MutableLiveData<>();
        schedulesChannel1 = new MutableLiveData<>();
        schedulesChannel2 = new MutableLiveData<>();
        schedulesChannel3 = new MutableLiveData<>();
        schedulesChannel4 = new MutableLiveData<>();
        schedulesChannel5 = new MutableLiveData<>();
        schedulesChannel6 = new MutableLiveData<>();
    }

    public void setConnectedState(boolean connectedState) {
        connected.setValue(connectedState);
    }

    public LiveData<Boolean> getConnectedState() {
        return connected;
    }

    public void setBleService(BluetoothLeService bluetoothService){
        sharedBleService.setValue(bluetoothService);
    }

    public LiveData<BluetoothLeService> getBleService(){
        return sharedBleService;
    }

    public void setSchedulesChannel1(ArrayList<Schedule> schedules){
        schedulesChannel1.setValue(schedules);
    }

    public LiveData<ArrayList<Schedule>> getSchedulesChannel1(){
        return schedulesChannel1;
    }

    public void setSchedulesChannel2(ArrayList<Schedule> schedules){
        schedulesChannel2.setValue(schedules);
    }

    public LiveData<ArrayList<Schedule>> getSchedulesChannel2(){
        return schedulesChannel2;
    }

    public void setSchedulesChannel3(ArrayList<Schedule> schedules){
        schedulesChannel3.setValue(schedules);
    }

    public LiveData<ArrayList<Schedule>> getSchedulesChannel3(){
        return schedulesChannel3;
    }

    public void setSchedulesChannel4(ArrayList<Schedule> schedules){
        schedulesChannel4.setValue(schedules);
    }

    public LiveData<ArrayList<Schedule>> getSchedulesChannel4(){
        return schedulesChannel4;
    }

    public void setSchedulesChannel5(ArrayList<Schedule> schedules){
        schedulesChannel5.setValue(schedules);
    }

    public LiveData<ArrayList<Schedule>> getSchedulesChannel5(){
        return schedulesChannel5;
    }

    public void setSchedulesChannel6(ArrayList<Schedule> schedules){
        schedulesChannel6.setValue(schedules);
    }

    public LiveData<ArrayList<Schedule>> getSchedulesChannel6(){
        return schedulesChannel6;
    }
}
