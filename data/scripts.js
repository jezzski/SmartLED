// scripts.js

workingSchedule = null;
dictSchedules = {};

function init(){
    document.getElementById("sch_time").value="12:00";

    // below is temp setup for testing initialize a schedule
    document.getElementById("sch_name").value="Test";
    document.getElementById("on_duration").value=5;
    document.getElementById("repeat_time").value=10;
    document.getElementById("mon_checkbox").checked=true;
    addSchEvent();
}

function CreateNewSchedule(){
    new_sch_container=document.getElementById("new_sch_container");
    debug_out=document.getElementById("debug");
    debug_out.innerHTML="wowow";
}

function channelSelect(channel_num){
    var i;
    for(i=1; i<7; i++){
        chSel = document.getElementById('ch_sel' + i);
        chSel.className = "";
    }
    activeCh = document.getElementById('ch_sel' + channel_num);
    activeCh.className = "active";
}

function addSchEvent(){
    
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
    
    // Get On Duration
    onDuration=document.getElementById("on_duration").value;
    console.log('On Duration: ' + onDuration);

    // Get Repeat Time
    repeatTime=document.getElementById("repeat_time").value;
    console.log('Repeat Time: ' + repeatTime);

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
    temp_mask = 1;
    for(check_box of checkedArr){
        if(check_box) repeatBitMask = repeatBitMask | temp_mask;
        temp_mask = temp_mask << 1;
    }
    console.log('Repeat bitmask: ' + repeatBitMask);

    eventList = document.getElementById("sch_event_list");
    // Temp Placeholder for params not implmented.
    isRGB = true;
    brightness = 100;
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
    schTable=document.getElementById("sch_table");
    for(row of schTable.rows){
        if(row.cells[0].innerHTML==schName){
            return true;
        }
    }
    return false;  // returns false if no matches
}


function deleteSchedule(calledFrom){
    // TBD: Should I erase the record from dictSchedules here?
    row = calledFrom.parentElement.parentElement;
    row.parentElement.deleteRow(row.rowIndex);
}

function applySchedules(){
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

function createScheduleJSON(active_ch, schName,schProperties){
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