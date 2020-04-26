package com.example.smartled;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.util.Log;
import android.widget.TextView;

import androidx.fragment.app.Fragment;

import java.util.UUID;

abstract public class BleControlFragment extends Fragment {

    //ble variables
    protected TextView mConnectionState;
    protected TextView mDataField;
    protected String mDeviceName;
    protected String mDeviceAddress;
    protected BluetoothLeService mBluetoothLeService;
    protected boolean isBound=false;

    abstract public void ifConnected();
    abstract public void ifDisconnected();
    abstract public void ifReadData(Intent intent);

    public final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            if(mBluetoothLeService!=null){
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
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
                ifConnected();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                ifDisconnected();
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                ifReadData(intent);
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
        if(mBluetoothLeService!=null){
            mBluetoothLeService.disconnect();
        }
        if(isBound){
            getContext().unbindService(mServiceConnection);
        }
        mBluetoothLeService = null;
    }

    @Override
    public void onStop() {
        super.onStop();
    }
}
