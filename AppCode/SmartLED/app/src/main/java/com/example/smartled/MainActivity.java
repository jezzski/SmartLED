package com.example.smartled;

import android.content.Intent;
import android.os.Bundle;

import com.example.smartled.ui.scan.BleDevice;

import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import com.google.android.material.navigation.NavigationView;

import androidx.drawerlayout.widget.DrawerLayout;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.view.Menu;

import java.util.ArrayList;

import static com.example.smartled.ui.scan.BleDevice.readDeviceXML;
import static com.example.smartled.ui.scan.BleDevice.writeDeviceToXML;

public class MainActivity extends AppCompatActivity {

    private AppBarConfiguration mAppBarConfiguration;
    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        NavigationView navigationView = findViewById(R.id.nav_view);
        //        // Passing each menu ID as a set of Ids because each
        //        // menu should be considered as top level destinations.
        mAppBarConfiguration = new AppBarConfiguration.Builder(
                R.id.nav_home, R.id.nav_schedule, R.id.nav_settings, R.id.nav_scan, R.id.nav_led)
                .setDrawerLayout(drawer)
                .build();
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment);
        NavigationUI.setupActionBarWithNavController(this, navController, mAppBarConfiguration);
        NavigationUI.setupWithNavController(navigationView, navController);

        final Intent intent = getIntent();
        if(intent!=null){
            String deviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
            String deviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);
            if(deviceAddress!=null){
                if(deviceName==null){
                    deviceName="Unknown";
                }
                ArrayList<BleDevice> deviceArray=readDeviceXML(getApplicationContext());
                BleDevice tempDevice=new BleDevice();
                tempDevice.setDeviceInfo(deviceName,deviceAddress);
                deviceArray.add(tempDevice);
                writeDeviceToXML(getApplicationContext(),deviceArray);
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onSupportNavigateUp() {
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment);
        return NavigationUI.navigateUp(navController, mAppBarConfiguration)
                || super.onSupportNavigateUp();
    }
}
