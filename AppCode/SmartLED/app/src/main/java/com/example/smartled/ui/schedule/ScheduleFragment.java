package com.example.smartled.ui.schedule;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import com.example.smartled.R;
import com.example.smartled.Schedule;

import org.xml.sax.SAXException;

import java.io.IOException;
import java.util.ArrayList;

import javax.xml.parsers.ParserConfigurationException;

import static com.example.smartled.Schedule.getScheduleNames;
import static com.example.smartled.Schedule.readXML;
import static com.example.smartled.Schedule.writeToXML;

public class ScheduleFragment extends Fragment {

    private ScheduleViewModel scheduleViewModel;
    public ScheduleListAdapter scheduleAdapter;
    public ListView scheduleList;
    ArrayList<String> scheduleString=new ArrayList<>();
    ArrayList<Schedule> scheduleArray=new ArrayList<Schedule>();
    Button addButton;
    EditText scheduleName;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        scheduleViewModel = ViewModelProviders.of(this).get(ScheduleViewModel.class);
        View root = inflater.inflate(R.layout.fragment_schedule, container, false);
        scheduleArray=readXML(root.getContext());
        //scheduleArray.clear();
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
                scheduleAdapter.notifyDataSetChanged();
            }
        });
        /*
        final TextView textView = root.findViewById(R.id.text_schedule);
        scheduleViewModel.getText().observe(this, new Observer<String>() {
            @Override
            public void onChanged(@Nullable String s) {
                textView.setText(s);
            }
        });
         */
        return root;
    }

    public void onResume(){
        super.onResume();

    }
}

 /* Sharded Preference Code
    private void saveStringToPreferences(String str){
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(getActivity());
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString("label", str);
        editor.apply();
    }
    scheduleList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            if (scheduleList.getItemAtPosition(position).toString() == "1") {
                temp=temp+1;
                scheduleString.add(temp);
                scheduleAdapter.notifyDataSetChanged();
                saveStringToPreferences(temp);
            }
        }
    });
    SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(getActivity());
    String previousText = preferences.getString("label", "");
        if(! TextUtils.isEmpty(previousText)){
        scheduleString.add(previousText);
        scheduleAdapter.notifyDataSetChanged();
    }
     */