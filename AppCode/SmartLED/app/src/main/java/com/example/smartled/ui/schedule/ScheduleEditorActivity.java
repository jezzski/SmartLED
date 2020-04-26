package com.example.smartled.ui.schedule;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

import com.example.smartled.R;
import com.example.smartled.HSVColorPickerDialog;

import java.util.ArrayList;
import java.util.Calendar;

import static com.example.smartled.ui.schedule.Schedule.readXML;
import static com.example.smartled.ui.schedule.Schedule.writeToXML;

public class ScheduleEditorActivity extends AppCompatActivity {
    ArrayList<Schedule> scheduleList;
    int scheduleNumber;
    Button colorButton;
    Button colorImage;
    CheckBox mCheck;
    CheckBox tuCheck;
    CheckBox wCheck;
    CheckBox thCheck;
    CheckBox fCheck;
    CheckBox saCheck;
    CheckBox suCheck;
    SeekBar brightBar;
    Button saveButton;
    EditText hourText;
    EditText minText;
    EditText durationText;
    EditText repeatText;
    int colorVal;
    Calendar cal;
    boolean colorButtonPressed;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_schedule_editor);
        Intent intent = getIntent();
        scheduleNumber= intent.getIntExtra(ScheduleListAdapter.EXTRA_MESSAGE, -1);
        scheduleList=readXML(getApplicationContext());
        Schedule curSchedule=scheduleList.get(scheduleNumber);

        TextView textView = findViewById(R.id.editorText);
        String scheduleTitle=curSchedule.name;
        textView.setText(scheduleTitle);

        mCheck=findViewById(R.id.rbM);
        tuCheck=findViewById(R.id.rbT);
        wCheck=findViewById(R.id.rbW);
        thCheck=findViewById(R.id.rbTH);
        fCheck=findViewById(R.id.rbF);
        saCheck=findViewById(R.id.rbSA);
        suCheck=findViewById(R.id.rbSU);
        suCheck.setChecked(checkRepeatMask(curSchedule.repeatMask, 7));
        mCheck.setChecked(checkRepeatMask(curSchedule.repeatMask, 6));
        tuCheck.setChecked(checkRepeatMask(curSchedule.repeatMask, 5));
        wCheck.setChecked(checkRepeatMask(curSchedule.repeatMask, 4));
        thCheck.setChecked(checkRepeatMask(curSchedule.repeatMask, 3));
        fCheck.setChecked(checkRepeatMask(curSchedule.repeatMask, 2));
        saCheck.setChecked(checkRepeatMask(curSchedule.repeatMask, 1));


        brightBar=findViewById(R.id.brightnessBar);
        int b=curSchedule.brightness & 0xFF;
        brightBar.setProgress(b);

        int r=curSchedule.Red & 0xFF;
        r=r<<16;
        int g=curSchedule.Green & 0xFF;
        g=g<<8;
        int b1=curSchedule.Blue & 0xFF;
        int savedColor=0xFF000000+r+g+b1;
        colorButtonPressed=false;
        colorImage=findViewById(R.id.colorImage);
        colorImage.setBackgroundColor(savedColor);
        colorImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HSVColorPickerDialog cpd = new HSVColorPickerDialog(v.getContext(), 0xFF4488CC, new HSVColorPickerDialog.OnColorSelectedListener() {
                    @Override
                    public void colorSelected(Integer color) {
                        colorImage.setBackgroundColor(color);
                        colorVal=color;
                    }
                });
                cpd.setTitle( "Pick a color" );
                cpd.setNoColorButton( R.string.no_color );
                cpd.show();
                colorButtonPressed=true;
            }
        });
        colorButton=findViewById(R.id.colorButton);
        colorButton.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                HSVColorPickerDialog cpd = new HSVColorPickerDialog(v.getContext(), 0xFF4488CC, new HSVColorPickerDialog.OnColorSelectedListener() {
                    @Override
                    public void colorSelected(Integer color) {
                        colorImage.setBackgroundColor(color);
                        colorVal=color;
                    }
                });
                cpd.setTitle( "Pick a color" );
                cpd.setNoColorButton( R.string.no_color );
                cpd.show();
                colorButtonPressed=true;
            }
        });

        long timestamp=curSchedule.start;

        Calendar tempCal = Calendar.getInstance();
        //tempCal.setTimeInMillis(timestamp * 1000);
        int hour=tempCal.get(Calendar.HOUR_OF_DAY);
        int min=tempCal.get(Calendar.MINUTE);
        hourText=findViewById(R.id.hourTimeText);
        hourText.setText(String.valueOf(hour));
        minText =findViewById(R.id.minuteTimeText);
        minText.setText(String.valueOf(min));
        int dur=curSchedule.duration;
        durationText=findViewById(R.id.durationText);
        durationText.setText(String.valueOf(dur));
        int rptTime=curSchedule.repeatTime;
        repeatText=findViewById(R.id.repeatText);
        repeatText.setText(String.valueOf(rptTime));

        saveButton=findViewById(R.id.saveButton);
        saveButton.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                Schedule curSaveSchedule=scheduleList.get(scheduleNumber);
                boolean[] daysRepeat= new boolean[7];
                for(int i=0;i<7;i++){
                    daysRepeat[i]=false;
                }
                //repeat
                byte tempRepeatMask=0;
                if(suCheck.isChecked()){
                    tempRepeatMask+=128;
                    daysRepeat[0]=true;
                }
                if(mCheck.isChecked()){
                    tempRepeatMask+=64;
                    daysRepeat[1]=true;
                }
                if(tuCheck.isChecked()){
                    tempRepeatMask+=32;
                    daysRepeat[2]=true;
                }
                if(wCheck.isChecked()){
                    tempRepeatMask+=16;
                    daysRepeat[3]=true;
                }
                if(thCheck.isChecked()){
                    tempRepeatMask+=8;
                    daysRepeat[4]=true;
                }
                if(fCheck.isChecked()){
                    tempRepeatMask+=4;
                    daysRepeat[5]=true;
                }
                if(saCheck.isChecked()){
                    tempRepeatMask+=2;
                    daysRepeat[6]=true;
                }
                curSaveSchedule.repeatMask=tempRepeatMask;

                //brightness
                byte brightness=(byte)brightBar.getProgress();
                curSaveSchedule.brightness=brightness;

                //color
                if(colorButtonPressed) {
                    curSaveSchedule.Red = (byte) ((colorVal & 0xFF0000) >> 16);
                    curSaveSchedule.Green = (byte) ((colorVal & 0xFF00) >> 8);
                    curSaveSchedule.Blue = (byte) (colorVal & 0xFF);
                }

                //time
                cal=Calendar.getInstance();
                int currentWeek=cal.get(Calendar.WEEK_OF_YEAR);
                int firstStartDay=cal.get(Calendar.DAY_OF_WEEK);

                boolean lessHour=cal.get(Calendar.HOUR_OF_DAY)>Integer.valueOf(hourText.getText().toString());
                boolean sameHour=cal.get(Calendar.HOUR_OF_DAY)==Integer.valueOf(hourText.getText().toString());
                boolean lessMin=cal.get(Calendar.MINUTE)>Integer.valueOf(minText.getText().toString());
                int finalStartDay;
                if(lessHour){
                    firstStartDay++;
                    finalStartDay=loopThroughDays(firstStartDay,daysRepeat);
                }
                else if(sameHour){
                    if(lessMin){
                        firstStartDay++;
                        finalStartDay=loopThroughDays(firstStartDay,daysRepeat);
                    }
                    else{
                        finalStartDay=loopThroughDays(firstStartDay,daysRepeat);
                    }
                }
                else{
                    finalStartDay=loopThroughDays(firstStartDay,daysRepeat);
                }
                if(loopedOver){
                    currentWeek++;
                }
                loopedOver=false;
                cal.set(Calendar.DAY_OF_WEEK, finalStartDay);
                cal.set(Calendar.HOUR_OF_DAY, Integer.valueOf(hourText.getText().toString()));
                cal.set(Calendar.MINUTE,Integer.valueOf(minText.getText().toString()));
                cal.set(Calendar.SECOND,0);
                cal.set(Calendar.WEEK_OF_YEAR, currentWeek);
                curSaveSchedule.start=(int)(cal.getTimeInMillis()/1000);
                if(Integer.valueOf(durationText.getText().toString())!=0){
                    curSaveSchedule.duration=Integer.valueOf(durationText.getText().toString());
                }
                else{
                    curSaveSchedule.duration=0xffffffff;
                }
                curSaveSchedule.repeatTime=Integer.valueOf(repeatText.getText().toString());
                Log.d("Hunaid", String.valueOf(curSaveSchedule.duration));
                writeToXML(getApplicationContext(), scheduleList);
                finish();
            }
        });
    }
    static boolean loopedOver=false;
    public static int loopThroughDays(int firstStartDay,boolean[] daysRepeat){
        int finalStartDay=0;
        int dayOffset = 0;
        for(int i=0;i<7;i++){
            if((firstStartDay-1+i-dayOffset)<7){
                if(daysRepeat[firstStartDay-1+i-dayOffset]){
                    finalStartDay=firstStartDay-1+i-dayOffset+1;
                    return finalStartDay;
                }
            }
            else{
                loopedOver=true;
                i--;
                dayOffset=7;
            }
        }
        loopedOver=false;
        return finalStartDay=firstStartDay;//need to return next available time if nothing is checked
    }

    public boolean checkRepeatMask(Byte repeatMask, int day){
        boolean checked=false;
        switch(day){
            case 0:
                if((repeatMask&1)==1){}
                break;
            case 1:
                if(((repeatMask&2)>>day)==1){checked=true;}
                else{checked=false;}
                break;
            case 2:
                if(((repeatMask&4)>>day)==1){checked=true;}
                else{checked=false;}
                break;
            case 3:
                if(((repeatMask&8)>>day)==1){checked=true;}
                else{checked=false;}
                break;
            case 4:
                if(((repeatMask&16)>>day)==1){checked=true;}
                else{checked=false;}
                break;
            case 5:
                if(((repeatMask&0x32)>>day)==1){checked=true;}
                else{checked=false;}
                break;
            case 6:
                if(((repeatMask&64)>>day)==1){checked=true;}
                else{checked=false;}
                break;
            case 7:
                if(((repeatMask&128)>>day)==1){checked=true;}
                else{checked=false;}
                break;
        }
        return checked;
    }
}