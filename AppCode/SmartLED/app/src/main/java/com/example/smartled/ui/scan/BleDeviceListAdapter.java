package com.example.smartled.ui.scan;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ListAdapter;
import android.widget.TextView;

import com.example.smartled.R;

import java.util.ArrayList;

import static com.example.smartled.ui.scan.BleDevice.readDeviceXML;
import static com.example.smartled.ui.scan.BleDevice.writeDeviceToXML;

public class BleDeviceListAdapter extends BaseAdapter implements ListAdapter {
    private ArrayList<String> list = new ArrayList<String>();
    private Context context;
    public static final String EXTRA_MESSAGE = "com.example.smartled.devicelist";


    public BleDeviceListAdapter(ArrayList<String> list, Context context) {
        this.list = list;
        this.context = context;
    }

    @Override
    public int getCount() {
        return list.size();
    }

    @Override
    public Object getItem(int pos) {
        return list.get(pos);
    }

    @Override
    public long getItemId(int pos) {
        return 0;
        //just return 0 if your list items do not have an Id variable.
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        View view = convertView;
        if (view == null) {
            LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            view = inflater.inflate(R.layout.activity_devicelistview, null);
        }

        //Handle TextView and display string from your list
        final TextView listItemText = (TextView)view.findViewById(R.id.list_item_string);
        listItemText.setText(list.get(position));

        //Handle buttons and add onClickListeners
        CheckBox activeCheck = (CheckBox) view.findViewById(R.id.active_check);
        ArrayList<BleDevice> tempDeviceList=readDeviceXML(context);
        if(tempDeviceList.get(position).isActive()){
            activeCheck.setChecked(true);
        }
        activeCheck.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,boolean isChecked) {
                ArrayList<BleDevice> tempDeviceList=readDeviceXML(context);
                if(isChecked){
                    tempDeviceList.get(position).setActive(true);
                }
                else{
                    tempDeviceList.get(position).setActive(false);
                }
                writeDeviceToXML(context,tempDeviceList);
            }
        });
        return view;
    }
}
