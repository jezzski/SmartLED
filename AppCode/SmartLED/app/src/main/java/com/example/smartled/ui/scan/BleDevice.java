package com.example.smartled.ui.scan;

import android.content.Context;
import android.util.Xml;

import com.example.smartled.ui.led.LedChannel;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringWriter;
import java.lang.reflect.Array;
import java.util.ArrayList;

public class BleDevice {
    public String deviceName;
    public String deviceAddress;
    public boolean active;

    BleDevice(){
        deviceName=null;
        deviceAddress=null;
        active=false;
    }

    public static BleDevice getActiveDevice(ArrayList<BleDevice> devices){
        BleDevice activeDevice=new BleDevice();
        for(BleDevice device:devices){
            if(device.isActive()){
                activeDevice=device;
                return activeDevice;
            }
        }
        return null;
    }

    public void setDeviceInfo(String deviceName, String deviceAddress){
        setDeviceName(deviceName);
        setDeviceAddress(deviceAddress);
    }


    public static ArrayList<String> getDeviceInfo(ArrayList<BleDevice> deviceList){
        ArrayList<String> deviceInfoList=new ArrayList<String>();
        for(int i=0; i<deviceList.size(); i++){
            deviceInfoList.add(deviceList.get(i).getDeviceName()+"\n"+deviceList.get(i).getDeviceAddress());
        }
        return deviceInfoList;
    }

    public boolean isActive() {
        return active;
    }

    public void setActive(boolean active) {
        this.active = active;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public String getDeviceAddress() {
        return deviceAddress;
    }

    public void setDeviceAddress(String deviceAddress) {
        this.deviceAddress = deviceAddress;
    }

    public static void writeDeviceToXML(Context context, ArrayList<BleDevice> bleDevices){
        XmlSerializer serializer = Xml.newSerializer();
        StringWriter writer = new StringWriter();
        try{
            serializer.setOutput(writer);
            serializer.startDocument("UTF-8", true);
            serializer.startTag("", "devices");
            //serializer.attribute("","number", String.valueOf(schedules.size()));
            for(BleDevice device : bleDevices){
                serializer.startTag("", "device");

                serializer.startTag("", "name");
                serializer.text(device.getDeviceName());
                serializer.endTag("", "name");

                serializer.startTag("", "address");
                serializer.text(device.getDeviceAddress());
                serializer.endTag("", "address");

                serializer.startTag("", "active");
                serializer.text(String.valueOf(device.isActive()));
                serializer.endTag("", "active");

                serializer.endTag("", "device");
            }
            serializer.endTag("", "devices");
            serializer.endDocument();
            String result=writer.toString();
            writeToFile(context, "testDevice.xml", result);
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
    public static ArrayList<BleDevice> readDeviceXML(Context context){
        ArrayList<BleDevice> devices=new ArrayList<BleDevice>();
        XmlPullParserFactory factory;
        FileInputStream fis= null;
        try{
            factory=XmlPullParserFactory.newInstance();
            factory.setNamespaceAware(true);
            XmlPullParser xpp=factory.newPullParser();
            fis=context.openFileInput("testDevice.xml");
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
                        case "name":
                            devices.add(new BleDevice());
                            devices.get(i).setDeviceName(text);
                            break;
                        case "address":
                            devices.get(i).setDeviceAddress(text);
                            break;
                        case "active":
                            devices.get(i).setActive(Boolean.valueOf(text));
                            break;
                        case "device":
                            i++;
                            break;
                    }
                }
                else if(eventType==XmlPullParser.TEXT){
                    text=xpp.getText();
                }
                eventType=xpp.next();
            }
            return devices;
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
