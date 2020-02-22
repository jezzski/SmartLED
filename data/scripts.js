// scripts.js

workingSchedule = null;

function init(){
    document.getElementById("sch_time").value="12:00";
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
    console.log('On Duration: ' + repeatTime);

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
