// scripts.js
/*
    Javascript side of Website for Iot LED Controller
    Contains scripts that add functionality to website,
    post data to ESP32 device, and received data from 
    ESP 32 device

    Author: Andy Yang
    Written Spring 2020 for Senior Design
*/

workingSchedule = null;
dictSchedules = {};

function debug(){
    console.log(dictSchedules["Test"]);
}

function init(){
    /*
        Function to be called on schedule page load
        Initializes some elements of the new sch form
    */
    document.getElementById("sch_time").value="12:00";

    // below is temp setup for testing initialize a schedule
    document.getElementById("sch_name").value="Test";
    document.getElementById("on_duration").value=5;
    document.getElementById("repeat_time").value=10;
    document.getElementById("mon_checkbox").checked=true;
    document.getElementById("is_RGB").checked=true;
    addSchEvent();  // temporary call

    // init time on device - TBD remove when RTC used
    postTimeToESP();  // temporary call
}

function CreateNewSchedule(){  // Deprecated in development
    new_sch_container=document.getElementById("new_sch_container");
    debug_out=document.getElementById("debug");
    debug_out.innerHTML="wowow";
}

function channelSelect(channel_num){
    /*
        Interacts with HTML to create show active channel on navigation bar

        Arguments:
            channel_num - a numeric character argument to tell function which element to make active

        TODO:
            remove archaic channel_num argument and use JS's 'this' keyword instead - 
                see function SetOnOff() and it's caller HTML in index.html for an example
    */
    var i;
    for(i=1; i<7; i++){
        chSel = document.getElementById('ch_sel' + i);
        chSel.className = "";
    }
    activeCh = document.getElementById('ch_sel' + channel_num);
    activeCh.className = "active";
}

function addSchEvent(){
    /*
        Updates a schedule event in the HTML form

        Note:
            When I originally added this function, I envisioned having multiple schedule events
                in a schedule (not the simple one-event schedule that existed in the embedded code).
                - This is why I named this function this way - TBD Decide if renaming is worthwhile
    */
    
    // Get Time of Event
    schTime = document.getElementById("sch_time").value;
    console.log(schTime);
    if(schTime==""){
        window.alert("Not a valid time");
        return;
    }
    [hour, min] = schTime.split(":");
    currDate = new Date();
    setDate = new Date(currDate.getFullYear(), currDate.getMonth(),
        currDate.getDate(), hour, min);
    schTimeUnix = setDate.getTime()/1000;  // time since epoch in s
    console.log('Event Time: '+ schTime + ' / Unix: ' + schTimeUnix);
    
    // Get Brightness
    brightness=document.getElementById("brightness_slider").value;
    console.log('Brightness: ' + brightness);

    // Get On Duration
    onDuration=document.getElementById("on_duration").value;
    console.log('On Duration: ' + onDuration);

    // Get Repeat Time
    repeatTime=document.getElementById("repeat_time").value;
    console.log('Repeat Time: ' + repeatTime);

    // Get is_RGB
    isRGB=document.getElementById("is_RGB").checked;
    console.log('Is Channel RGB: ' + isRGB);

    // Get RGB Color Info
    colorValue=document.getElementById("sch_color").value;
    console.log('Color Value: ' + colorValue);

    // Generate Week Repeat Mask
    checkedArr = [
        document.getElementById("sun_checkbox").checked,
        document.getElementById("mon_checkbox").checked,
        document.getElementById("tues_checkbox").checked,
        document.getElementById("wed_checkbox").checked,
        document.getElementById("th_checkbox").checked,
        document.getElementById("fri_checkbox").checked,
        document.getElementById("sat_checkbox").checked
    ];
    repeatBitMask = 0;  // init to 0
    temp_mask = 64;
    for(check_box of checkedArr){
        if(check_box) repeatBitMask = repeatBitMask | temp_mask;
        temp_mask = temp_mask >> 1;
    }
    console.log('Repeat bitmask: ' + repeatBitMask);

    eventList = document.getElementById("sch_event_list");

    // Temp for simple schedule - Only supports singular event
    eventList.innerHTML = schTime + " | On Duration: " +
        onDuration + " | Repeat Time: " + repeatTime +
        " <br> Bitmask: " + repeatBitMask + " | isRGB: " +
        isRGB + " | Color: " + colorValue + " <br>" +
        "Brightness: " + brightness ;
    
    workingSchedule=[schTimeUnix, onDuration, repeatTime,
        repeatBitMask, brightness, isRGB, colorValue]
}

function addSchedule(){
    /*
        Adds/Updates schedule to table and internal schedules data structure
    */
    addSchEvent();  // updates sch form to most recent user input
    if(workingSchedule==null){
        window.alert("No events in schedule");
        return;
    }

    schName=document.getElementById("sch_name").value;
    if(schName==""){
        window.alert("Schedule is not named");
        return;
    }
    if(!scheduleInTable(schName)){  // if not in table, create entry
        console.log('Adding schedule: ' + schName);

        schTable=document.getElementById("sch_table");

        newSchEntry=schTable.insertRow(-1);
        nameCell=newSchEntry.insertCell(0);
        nameCell.innerHTML=schName;
        nameCell.className="channel_entry";

        activeCell=newSchEntry.insertCell(1);
        activeCell.innerHTML="<input type=\"checkbox\">";
        activeCell.className="channel_entry";

        editCell=newSchEntry.insertCell(2);
        editCell.innerHTML="<input type=\"button\">";
        editCell.className="channel_entry";

        selectedCell=newSchEntry.insertCell(3);
        selectedCell.innerHTML="<input type=\"button\" onclick=\"deleteSchedule(this)\">";
        selectedCell.className="channel_entry";
    }

    // add/update schedule properties
    dictSchedules[schName]=workingSchedule;

}

function scheduleInTable(schName){
    /*
        Utility to return if schedule is in table based on schedule name

        Arguments:
            schName - schedule name string
    */
    schTable=document.getElementById("sch_table");
    for(row of schTable.rows){
        if(row.cells[0].innerHTML==schName){
            return true;
        }
    }
    return false;  // returns false if no matches
}


function deleteSchedule(calledFrom){
    /*
        Deletes a schedule from UI table - does NOT delete from internal data structure

        Arguments:
            calledFrom - object in which invoked the function is passed in here
                In the use case this is the HTML input button at a delete column
                at the row of the particular schedule to be deleted
    */
    // TBD: Should I erase the record from dictSchedules here?
    row = calledFrom.parentElement.parentElement;
    row.parentElement.deleteRow(row.rowIndex);
}

function applySchedules(){
    /*
        Takes active schedules and sends them to the ESP32
    */
    schTable=document.getElementById("sch_table");
    for(row of schTable.rows){
        if(row.rowIndex==0) continue;  // skip header row
        schName = row.cells[0].innerHTML;
        activeCheck = row.cells[1].children[0].checked;
        if(activeCheck){
            console.log('Processing active schedule: '+ schName);
            if(! (schName in dictSchedules)){  // if not found
                window.alert('No data structure found for selected ' +
                    'schedule: ' + schName);
                return;
            }
            // Get Active Channel
            for(i=1; i<7; i++){
                chSel = document.getElementById('ch_sel' + i);
                if(chSel.className=="active"){
                    activeCh=i;
                }
            }
            console.log(`Applying schedule ${schName} to channel: ${activeCh}`);
            // Sent to ESP32
            httpPostSchToESP(schName, activeCh);
        }
    }
}

function createScheduleJSON(active_ch, schName,schProperties){  // deprecated in development
    sch_JSON =`{
        "${active_ch}" : {
            "id": 0,
            "name": "${schName}",
            "enabled": true,
            "start": "${schProperties[0]}"
        },
    }`;
    return sch_JSON;
}

function Test(schName, channel_num){
    delimiterStr=";';";
    schData = dictSchedules[schName];
    strToTransmit=schName + delimiterStr + channel_num;
    for(i=0; i<schData.length; i++){
        strToTransmit = strToTransmit + delimiterStr +
            schData[i];
    }

    console.log('The following string will be trasmitted: ' +
        strToTransmit);
}

function httpPostSchToESP(schName, channel_num){
    /*
        Post a schedule to the ESP32 as a delimited string

        Arguments:
            schName - schedule name string
            channel_num - numeric char of channel number
    */
    request = new XMLHttpRequest();
    // request.onload = function(){
    //     status = request.status;
    //     data=request.responseText;
    // }

    // get schedule data
    schData = dictSchedules[schName];  // error checking is performed by caller

    // prepare data to send
    delimiterStr=";';";
    strSchData = schName + delimiterStr + channel_num + 
        delimiterStr + "true";  // hard-code enabled TBD Revist
    for(i=0; i<schData.length; i++){
        strSchData = strSchData + delimiterStr +
            schData[i];
    }

    strSchData = strSchData + delimiterStr + "PADDING";

    request.open("POST", "post_sch", true);  // true indicates Async
    // request.setRequestHeader("Content-Type", "applicaiton/json;charset=UTF-8");

    // send data
    request.send(strSchData);
}

function postTimeToESP(){
    /*
        Post time to ESP32

        Note:
            Chrome uses minute precision for time so your uploaded time will have
                a maximum error of 59 s
    */
    request = new XMLHttpRequest();

    currDate = new Date();
    notExactDate = new Date(currDate.getFullYear(), currDate.getMonth(),
    currDate.getDate(), currDate.getHours(), currDate.getMinutes());
    schTimeUnix = notExactDate.getTime()/1000;  // time since epoch in s

    request.open("POST", "time", true);  // true indicates Async
    // request.setRequestHeader("Content-Type", "applicaiton/json;charset=UTF-8");
    console.log('Sending: ' + schTimeUnix);
    // send data
    request.send(schTimeUnix);
}

// -----------------------------------------
// Direct Control Scripts


function setOnOff(objCalledFrom, channelNum){
    /*
        Sets on off state of LED channel - HTTP Post to ESP32

        Arguments:
            objCalledFrom - 
            channelNum - 
    */
    // channelNum parameter is currently indexed by 1
    // i.e. channels 1-6 - do we want to index by 0?
    console.log(objCalledFrom.checked + ":" + channelNum);

    request = new XMLHttpRequest();
    request.open("POST", "direct_control", true);  // true indicates Async
    delimiterStr=";';";
    // sends cmd channel num state
    // console.log("OnOff" + delimiterStr + channelNum + delimiterStr +
    // Number(objCalledFrom.checked) + delimiterStr + "PADDING")
    request.send("OnOff" + delimiterStr + channelNum + delimiterStr +
        Number(objCalledFrom.checked) + delimiterStr + "PADDING");
}

function setColor(objCalledFrom, iWhichSet){
    /*
        Sets color of RGB channel - HTTP Post to ESP32

        Arguments:
            objCalledFrom - 
            iWhichSet - 
    */
    // iWhichSet 1 corresponds to channels 1-3
    // iWhichSet 4 corresponds to channels 4-6
    console.log("Reached setColor. Set: " + iWhichSet +
        " Color: " + objCalledFrom.value);
    request = new XMLHttpRequest();
    request.open("POST", "direct_control", true);  // true indicates Async
    delimiterStr=";';";
    request.send("Color" + delimiterStr + iWhichSet + delimiterStr +
        objCalledFrom.value + delimiterStr + "PADDING")
}

function setBrightness(objCalledFrom, channelNum){
    /*
        Sets brightness - HTTP Post to ESP32

        Arguments:
            objCalledFrom - 
            channelNum - 
    */
    console.log("Channel Num: " + channelNum + " - Brightness: " + objCalledFrom.value);

    request = new XMLHttpRequest();
    request.open("POST", "direct_control", true);  // true indicates Async
    delimiterStr=";';";
    // console.log("Brightness" + delimiterStr + channelNum + 
    //     delimiterStr + objCalledFrom.value + delimiterStr + "PADDING");
    request.send("Brightness" + delimiterStr + channelNum + 
        delimiterStr + objCalledFrom.value + delimiterStr + "PADDING");
}
