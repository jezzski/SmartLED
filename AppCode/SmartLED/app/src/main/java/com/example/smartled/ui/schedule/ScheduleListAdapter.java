package com.example.smartled.ui.schedule;

import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.ListAdapter;
import android.widget.TextView;

import com.example.smartled.R;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class ScheduleListAdapter extends BaseAdapter implements ListAdapter {
    private ArrayList<String> list = new ArrayList<String>();
    private Context context;
    private List<Boolean> checkedList;
    private LayoutInflater mInflater;
    public static final String EXTRA_MESSAGE = "com.example.smartled.schedulename";


    public ScheduleListAdapter(ArrayList<String> list, Context context) {
        this.list = list;
        this.context = context;
        this.checkedList=new ArrayList<Boolean>(Arrays.asList(new Boolean[list.size()]));
        mInflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        Collections.fill(this.checkedList, Boolean.FALSE);
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

    public List<Boolean> getCheckedArray() {
        return this.checkedList;
    }

    public void updateAdapter(ArrayList<String> nameList){
        this.list=nameList;
        List<Boolean> tempList=new ArrayList<Boolean>(Arrays.asList(new Boolean[nameList.size()]));
        Collections.fill(tempList, Boolean.FALSE);
        /*
        int i=0;
        for(Boolean b: checkedList){
            if(i<tempList.size()){
                tempList.set(i,b);
            }
            i++;
        }
         */
        this.checkedList=tempList;
        notifyDataSetChanged();
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        View view = convertView;
        Holder holder=null;
        if (view == null) {
            //LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            view = mInflater.inflate(R.layout.activity_listview, null);
            holder = new Holder();
            holder.editBtn=(ImageButton)view.findViewById(R.id.edit_btn);
            holder.ckbox=(CheckBox)view.findViewById(R.id.schedule_check);
            view.setTag(holder);
        } else{
            holder = (Holder) convertView.getTag();
            holder.ckbox.setOnCheckedChangeListener(null);
        }
        holder.ckbox.setFocusable(false);
        holder.ckbox.setChecked(checkedList.get(position));
        //Handle TextView and display string from your list
        final TextView listItemText = (TextView)view.findViewById(R.id.list_item_string);
        listItemText.setText(list.get(position));

        //Handle buttons and add onClickListeners
        //ImageButton editBtn = (ImageButton)view.findViewById(R.id.edit_btn);

        holder.editBtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(v.getContext(), ScheduleEditorActivity.class);
                intent.putExtra(EXTRA_MESSAGE, position);
                v.getContext().startActivity(intent);
                notifyDataSetChanged();
            }
        });
        //need to keep track of which schedules are checked
        //CheckBox scheduleCheck = (CheckBox) view.findViewById(R.id.schedule_check);
        holder.ckbox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,boolean isChecked) {
                if(isChecked){
                    checkedList.set(position, Boolean.TRUE);
                }
                else{
                    checkedList.set(position, Boolean.FALSE);
                }
            }
        });
        return view;
    }

    static class Holder
    {
        ImageButton editBtn;
        CheckBox ckbox;

    }
}


