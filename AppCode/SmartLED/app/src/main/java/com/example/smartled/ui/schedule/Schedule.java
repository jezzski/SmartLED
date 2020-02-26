package com.example.smartled.ui.schedule;

import android.content.Context;
import android.util.Xml;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringWriter;
import java.nio.ByteBuffer;
import java.util.ArrayList;

public class Schedule {
    public int ID;
    public String name;
    public int start;
    public int duration;
    public int repeatTime;
    public byte brightness;
    public byte Red;
    public byte Green;
    public byte Blue;
    public byte repeatMask;
    public Schedule(String schName){
        ID=0;
        name=schName;
        start=0;
        duration=0;
        repeatTime=0;
        brightness=(byte)0xff;
        Red= 0x00;
        Green= 0x00;
        Blue= 0x00;
        repeatMask=0x00;
    }
    public static byte[] createSchedulePacket(Schedule schedule, int channelNum){
        ByteBuffer b=ByteBuffer.allocate(4);
        ByteBuffer b2=ByteBuffer.allocate(4);
        ByteBuffer b3=ByteBuffer.allocate(4);
        byte[] packet=new byte[19];
        byte[] result=b.array();
        packet[0]=(byte)channelNum;
        packet[1]=(byte)schedule.ID;
        b.putInt(schedule.start);
        packet[2]=result[0];
        packet[3]=result[1];
        packet[4]=result[2];
        packet[5]=result[3];
        b2.putInt(schedule.duration);
        byte[] result2=b2.array();
        packet[6]=result2[0];
        packet[7]=result2[1];
        packet[8]=result2[2];
        packet[9]=result2[3];
        packet[10]=schedule.brightness;
        packet[11]=schedule.Red;
        packet[12]=schedule.Green;
        packet[13]=schedule.Blue;
        packet[14]=schedule.repeatMask;
        b3.putInt(schedule.repeatTime);
        byte[] result3=b3.array();
        packet[15]=result3[0];
        packet[16]=result3[1];
        packet[17]=result3[2];
        packet[18]=result3[3];
        return packet;
    }
    public static ArrayList<String> getScheduleNames(ArrayList<Schedule> scheduleList){
        ArrayList<String> schStringList=new ArrayList<String>();
        for(int i=0; i<scheduleList.size(); i++){
            schStringList.add(scheduleList.get(i).name);
        }
        return schStringList;
    }

    public static void writeToXML(Context context, ArrayList<Schedule> schedules){
        XmlSerializer serializer = Xml.newSerializer();
        StringWriter writer = new StringWriter();
        try{
            serializer.setOutput(writer);
            serializer.startDocument("UTF-8", true);
            serializer.startTag("", "schedules");
            //serializer.attribute("","number", String.valueOf(schedules.size()));
            for(Schedule schedule : schedules){
                serializer.startTag("", "schedule");

                serializer.startTag("", "name");
                serializer.text(schedule.name);
                serializer.endTag("", "name");

                serializer.startTag("", "ID");
                schedule.ID=schedules.indexOf(schedule);
                serializer.text(String.valueOf(schedule.ID));
                serializer.endTag("", "ID");

                serializer.startTag("", "start");
                serializer.text(String.valueOf(schedule.start));
                serializer.endTag("", "start");

                serializer.startTag("", "duration");
                serializer.text(String.valueOf(schedule.duration));
                serializer.endTag("", "duration");

                serializer.startTag("", "repeatTime");
                serializer.text(String.valueOf(schedule.repeatTime));
                serializer.endTag("", "repeatTime");

                serializer.startTag("", "brightness");
                serializer.text(String.valueOf(schedule.brightness));
                serializer.endTag("", "brightness");

                serializer.startTag("", "red");
                serializer.text(String.valueOf(schedule.Red));
                serializer.endTag("", "red");

                serializer.startTag("", "green");
                serializer.text(String.valueOf(schedule.Green));
                serializer.endTag("", "green");

                serializer.startTag("", "blue");
                serializer.text(String.valueOf(schedule.Blue));
                serializer.endTag("", "blue");

                serializer.startTag("", "repeat");
                serializer.text(String.valueOf(schedule.repeatMask));
                serializer.endTag("", "repeat");

                serializer.endTag("", "schedule");
            }
            serializer.endTag("", "schedules");
            serializer.endDocument();
            String result=writer.toString();
            writeToFile(context, "testSch.xml", result);
        } catch (Exception e){
            throw new RuntimeException(e);
        }

    }
    public static void writeToFile(Context context, String fileName, String str){
        try{
            FileOutputStream fos=context.openFileOutput(fileName, Context.MODE_PRIVATE);
            fos.write(str.getBytes(), 0, str.length());
            fos.close();
        } catch (IOException e){
            e.printStackTrace();
        }
    }

    public static ArrayList<Schedule> readXML(Context context){
        ArrayList<Schedule> schedules=new ArrayList<Schedule>();
        XmlPullParserFactory factory;
        FileInputStream fis= null;
        try{
           factory=XmlPullParserFactory.newInstance();
           factory.setNamespaceAware(true);
           XmlPullParser xpp=factory.newPullParser();
           fis=context.openFileInput("testSch.xml");
           String tag="";
           String text="";
           xpp.setInput(fis, null);
           int eventType=xpp.getEventType();
           int i=0;
           while(eventType !=XmlPullParser.END_DOCUMENT){
               tag=xpp.getName();
               if(eventType==XmlPullParser.START_DOCUMENT){

               }
               else if(eventType == XmlPullParser.START_TAG){

               }
               else if(eventType==XmlPullParser.END_TAG){
                   switch(tag){
                       case "name":
                           schedules.add(new Schedule(text));
                           break;
                       case "ID":
                           schedules.get(i).ID=Integer.valueOf(text);
                           break;
                       case "start":
                           schedules.get(i).start=Integer.valueOf(text);
                           break;
                       case "duration":
                           schedules.get(i).duration=Integer.valueOf(text);
                           break;
                       case "repeatTime":
                           schedules.get(i).repeatTime=Integer.valueOf(text);
                           break;
                       case "brightness":
                           schedules.get(i).brightness=Byte.valueOf(text);
                           break;
                       case "red":
                           schedules.get(i).Red=Byte.valueOf(text);
                           break;
                       case "green":
                           schedules.get(i).Green=Byte.valueOf(text);
                           break;
                       case "blue":
                           schedules.get(i).Blue=Byte.valueOf(text);
                           break;
                       case "repeat":
                           schedules.get(i).repeatMask=Byte.valueOf(text);
                           break;
                       case "schedule":
                           i++;
                           break;
                   }
               }
               else if(eventType==XmlPullParser.TEXT){
                   text=xpp.getText();
               }
               eventType=xpp.next();
           }
           return schedules;
        } catch (XmlPullParserException e){
            e.printStackTrace();
        } catch (IOException e){
            e.printStackTrace();
        }finally {
            if(fis!=null){
                try{
                    fis.close();
                } catch (IOException e){
                    e.printStackTrace();
                }
            }
        }
        return schedules;
    }
}
