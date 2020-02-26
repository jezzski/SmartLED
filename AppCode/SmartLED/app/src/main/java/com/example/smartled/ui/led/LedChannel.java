package com.example.smartled.ui.led;

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
import java.util.ArrayList;

public class LedChannel {
    public int channelNum;
    public int[] scheduleIndexes;
    public int scheduleCount;

    public LedChannel(){
        channelNum=0;
        scheduleIndexes=null;
        scheduleCount=0;
    }
    public void setLedChannel(int chanNum, int[] indexes, int count){
        channelNum=chanNum;
        scheduleIndexes=indexes;
        scheduleCount=count;
    }

    public static void writeChannelToXML(Context context, ArrayList<LedChannel> channels){
        XmlSerializer serializer = Xml.newSerializer();
        StringWriter writer = new StringWriter();
        try{
            serializer.setOutput(writer);
            serializer.startDocument("UTF-8", true);
            serializer.startTag("", "channels");
            //serializer.attribute("","number", String.valueOf(schedules.size()));
            for(LedChannel channel : channels){
                serializer.startTag("", "channel");

                serializer.startTag("", "number");
                serializer.text(String.valueOf(channel.channelNum));
                serializer.endTag("", "number");

                serializer.startTag("", "count");
                serializer.text(String.valueOf(channel.scheduleCount));
                serializer.endTag("", "count");

                for(int i=0;i<channel.scheduleCount;i++){
                    serializer.startTag("", "scheduleID");
                    serializer.text(String.valueOf(channel.scheduleIndexes[i]));
                    serializer.endTag("", "scheduleID");
                }

                serializer.endTag("", "channel");
            }
            serializer.endTag("", "channels");
            serializer.endDocument();
            String result=writer.toString();
            writeToFile(context, "testChan.xml", result);
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
    public static ArrayList<LedChannel> readChannelXML(Context context){
        ArrayList<LedChannel> channels=new ArrayList<LedChannel>();
        XmlPullParserFactory factory;
        FileInputStream fis= null;
        try{
            factory=XmlPullParserFactory.newInstance();
            factory.setNamespaceAware(true);
            XmlPullParser xpp=factory.newPullParser();
            fis=context.openFileInput("testChan.xml");
            String tag="";
            String text="";
            xpp.setInput(fis, null);
            int eventType=xpp.getEventType();
            int i=0;
            int currentIDCount=0;
            while(eventType !=XmlPullParser.END_DOCUMENT){
                tag=xpp.getName();
                if(eventType==XmlPullParser.START_DOCUMENT){

                }
                else if(eventType == XmlPullParser.START_TAG){

                }
                else if(eventType==XmlPullParser.END_TAG){
                    switch(tag){
                        case "number":
                            channels.add(new LedChannel());
                            channels.get(i).channelNum=Integer.valueOf(text);
                            break;
                        case "count":
                            channels.get(i).scheduleCount=Integer.valueOf(text);
                            channels.get(i).scheduleIndexes=new int[channels.get(i).scheduleCount];
                            currentIDCount=0;
                            break;
                        case "scheduleID":
                            channels.get(i).scheduleIndexes[currentIDCount]=Integer.valueOf(text);
                            currentIDCount++;
                            break;
                        case "channel":
                            i++;
                            break;
                    }
                }
                else if(eventType==XmlPullParser.TEXT){
                    text=xpp.getText();
                }
                eventType=xpp.next();
            }
            return channels;
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
        return null;
    }
}
