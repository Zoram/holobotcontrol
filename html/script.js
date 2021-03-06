/**************************************************************

	Author= Lukas Dudichum
	Created= 19.10.2012
	License= MIT
	
***************************************************************/

// Port on which Bot control is listen
var CONTROL_PORT = 2000;

// Port on which the mjpg-streamer webcam server runs
var WEBCAM_PORT = 8080;

// Textfield for diplay status information
var connectionStatus;

// Textfield for battery status information
var batteryStatus;

function init(){

	connectionStatus = document.getElementById("connection_status");
	batteryStatus = document.getElementById("battery_status");
	
	// Set video stream source
	document.getElementById('stream').setAttribute('src', window.location.protocol+"//"+window.location.hostname+":"+WEBCAM_PORT+"/?action=stream");
	
	// call every 500ms "checkConnection()" to check if bot is in WLAN- range
	var heartBeat = window.setInterval("checkConnection()", 500);
	
	// register eventlistener functions for control panel
	var controlSection = document.getElementById("control");
	var childCount = controlSection.getElementsByTagName("button").length;
	var buttons = controlSection.getElementsByTagName("button");
	
	for(var index = 0; index < childCount; index++){
		// to send command to bot
		buttons[index].addEventListener("mousedown",sendControlCommand,false);
		// to stop last command
		buttons[index].addEventListener("mouseup",stopControlCommand,false);
	}
	
}

function checkConnection(){
	$.ajax({
		cache:false,
		type:"GET",
		timeout: 800,
		url:window.location.protocol+'//'+window.location.host + "/heartbeat",
		error:function(){ 
			connectionStatus.innerText = "Connection lost"
			connectionStatus.style.color = "red";
		},
		success:function(response){
			
			var voltage = $(response).find('lipovoltage').text();
			batteryStatus.innerText = "Voltage: "+ voltage;
			batteryStatus.style.color = "red";
			connectionStatus.innerText = "Connection OK"
			connectionStatus.style.color = "green";
		}
	});
}

function sendControlCommand(event){
	var button = event.target;	
	if(button.id == "rotate_counterclockwise" || button.id == "rotate_clockwise"){
		$.ajax({
			cache:false,
			type:"GET",
			url:window.location.protocol+'//'+window.location.host + "/command?command="+button.id,
		});
	}else{
		$.ajax({
			cache:false,
			type:"GET",
			url:window.location.protocol+'//'+window.location.host + "/command?command=drive&angle="+buttonIdToAngle(button.id),
		});
	}
	
}

function stopControlCommand(event){
	$.ajax({
		cache:false,
		type:"GET",
		url:window.location.protocol+'//'+window.location.host + "/command?command=hold",
	});
}

function buttonIdToAngle(buttonId){
	switch (buttonId) {
		case "north":
			return "0";
			break;
		case "north_west":
			return "45";
			break;
		case "west":
			return "90";
			break;
		case "south_west":
			return "135";
			break;
		case "south":
			return "180";
			break;
		case "south_east":
			return "225";
			break;
		case "east":
			return "270";
			break;
		case "north_east":
			return "315";
			break;
	  default:
	    alert("Sie bleiben leider dumm");
	    break;
	}
}