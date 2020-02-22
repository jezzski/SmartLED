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
    schTimeUnix = setDate.getTime();
    console.log('Event Time: '+ schTime + ' / Unix: ' + schTimeUnix);
    
    // Get On Duration
    onDuration=document.getElementById("on_duration").value;
    console.log('On Duration: ' + onDuration);

    // Get Repeat Time
    repeatTime=document.getElementById("repeat_time").value;
    console.log('Repeat Time: ' + repeatTime);

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
    // Temp for simple schedule - Only supports singular event
    eventList.innerHTML = schTime + " | Bitmask: " + repeatBitMask +
        " | On Duration: " + onDuration + " | Repeat Time: " + 
        repeatTime;
    
    workingSchedule=[schTimeUnix, onDuration, repeatTime, repeatBitMask, ""]
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
    console.log('Adding schdule: ' + schName);

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
    selectedCell.innerHTML="<input type=\"checkbox\">";
    selectedCell.className="channel_entry";

    dictSchedules[schName]=workingSchedule;

}

function deleteSelected(){
    return;
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
            schProperties=dictSchedules[schName];
            console.log(schProperties);
            console.log(createScheduleJSON(1, schName, schProperties));
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