/** Class to control bots main functions controlled over HTTP.
 *  There are 3 main functions:
 *  - control omnidrive based on incomming commands (see ICD)
 *  - control liquid crystal display 
 *  - heartbeat mechanism to notice connection loss
 *
 *  License: MIT
 *   
 *  @author Lukas Dudichum 
 *  @date 10.10.2012
 * 
 */

#include "holobotcontrol.h"

/** Mutex to protect heartbeat flag */
pthread_mutex_t heartbeatMutex = PTHREAD_MUTEX_INITIALIZER;
/** Mutex to protect display */
pthread_mutex_t displayMutex = PTHREAD_MUTEX_INITIALIZER;
/** Flag that shows if connection bot <-> client is OK */
static bool heartbeatAlive = false;
/** Handle for HoloBotControl */
HoloBotControl* mongooseServer = NULL;

HoloBotControl::HoloBotControl(): MongooseServer(){

	// Init liquid crystal display
	this->display = new LiquidCrystal_I2C( 0x20,20,4);
	this->display->init();
	this->display->backlight();
	this->display->printToRow("## HoloBot online ##",20,0);

	// Start thread for heartbeat mechanism to check connection bot <-> client
	pthread_create(&this->heartbeatThread, NULL, &HoloBotControl::checkHeartbeat, this);
	// Start thread to display local ip addresses
	pthread_create(&this->displayIPAdressesThread, NULL, &HoloBotControl::displayLocalIpAddresses, this);
	
	this->omnidrive = new OmniDrive();

}

void* HoloBotControl::displayLocalIpAddresses(void *object){

	struct ifaddrs * ifaTmp=NULL;
	struct ifaddrs * ifAddrStruct=NULL;
	void * tmpAddrPtr=NULL;
	HoloBotControl* controlPt = (HoloBotControl*)object;
	
	while(1){
		
		// Get IP of all NICs
		getifaddrs(&ifAddrStruct);
		
		for (ifaTmp = ifAddrStruct; ifaTmp != NULL; ifaTmp = ifaTmp->ifa_next) {
			// Check if it is IP4 (IPv6 will be ignored)
			if (ifaTmp ->ifa_addr->sa_family==AF_INET) { 
				// Valid IP4 Address
				tmpAddrPtr=&((struct sockaddr_in *)ifaTmp->ifa_addr)->sin_addr;
				char addressBuffer[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			
				string ifName(ifaTmp->ifa_name);
				string ifIP(addressBuffer);
				ifName += ":";
				ifName += ifIP;

				pthread_mutex_lock( &displayMutex );
				controlPt->display->printToRow(ifName, 20, 2);
				pthread_mutex_unlock( &displayMutex );
				// Display NIC information for 2 seconds
				sleep(2);
			}
		} 
	}
}

HoloBotControl::~HoloBotControl(){
	// Turn display off
	display->noDisplay();
	display->noBacklight();
	// Stop running threads
	pthread_cancel(this->displayIPAdressesThread);
	pthread_cancel (this->heartbeatThread);
	// Free stack
	delete(this->omnidrive);
	delete(this->display);
	cout << "[ OK ] HoloBotControl stopped" << endl;
}

bool HoloBotControl::handleEvent(ServerHandlingEvent eventCode, MongooseConnection &connection, const MongooseRequest &request, MongooseResponse &response) {
	
	bool success = false;
	string command, angle;
	
	// Check if its a normal http request
	if (eventCode == MG_NEW_REQUEST) {
		
		// There are two types of messages -> "command messages": Control robots movement / "heartbeat messages": Check availability of bot
		if (request.getUri() == string(HoloBotControl::COMMAND_MESSAGE)) {
			request.getVar("command", command);
			if(command == "drive"){
				request.getVar("angle", angle);
				cout << angle << endl;
				this->omnidrive->driveInSpecificAngle( atoi(angle.c_str()), OmniDrive::NONE);
			}else if(command == "hold"){
				this->omnidrive->stop();
			}else if(command == "rotate_counterclockwise"){
				this->omnidrive->rotate(OmniDrive::COUNTERCLOCKWISE);
			}else if(command == "rotate_clockwise"){
				this->omnidrive->rotate(OmniDrive::CLOCKWISE);
			}else{
				cout << "[ ERROR ] Unknown command!" << endl;
			}
			
			handleInfo(request, response);
			success = true;
			
		}else if(request.getUri() == string(HoloBotControl::HEARTBEAT_MESSAGE)){
			pthread_mutex_lock( &heartbeatMutex );
			heartbeatAlive = true;
			pthread_mutex_unlock( &heartbeatMutex );
			handleInfo(request, response);
			success=true;
			
		}
	}

	return success;
}

void HoloBotControl::handleInfo(const MongooseRequest &request, MongooseResponse &response) {

	response.setStatus(200);
	response.setConnectionAlive(false);
	response.setCacheDisabled();
	response.setContentType("text/html");
	response.addContent(generateStatusContent());
	response.write();
}

void* HoloBotControl::checkHeartbeat(void *object){
	
	HoloBotControl* controlPt = (HoloBotControl*)object;
		
	while(1){
		// wait 1s for incomming heartbeat 
		sleep(1);
		
		if(!heartbeatAlive){
			
			// Stop all motors when connection is lost
			controlPt->omnidrive->stop();
			pthread_mutex_lock( &displayMutex );
			controlPt->display->printToRow("Connected: NONE",20,1);
			pthread_mutex_unlock( &displayMutex );
			cout << "[ STATUS ] No client connected" << endl;
			
		}else{
			
			pthread_mutex_lock( &displayMutex ); 
			controlPt->display->printToRow("Connected: YES",20,1);
			pthread_mutex_unlock( &displayMutex );
				
		}
		pthread_mutex_lock( &heartbeatMutex );
		heartbeatAlive = false;
		pthread_mutex_unlock( &heartbeatMutex );
		
	}
	return 0;
}

const string HoloBotControl::generateStatusContent() {
	string result;
	result = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
	result += "<statusmessage>";
	result += "<lipovoltage>12</lipovoltage>";
	result += "</statusmessage>";
	return result;
}

void signal_handler(int signum)
{
	mongooseServer->stop();
	delete(mongooseServer);
	exit(signum);
}

int main()
{
	mongooseServer = new HoloBotControl();
	mongooseServer->setOption("document_root", "./html");
	mongooseServer->setOption("listening_ports", "2000");
	mongooseServer->setOption("num_threads", "1");
	mongooseServer->start();

	// Register signal -> Program interrupt. (ctrl-c)
	signal(SIGINT, signal_handler);
	// Register signal -> Generated by "kill" command
	signal(SIGTERM, signal_handler);

	cout << "[ OK ] HoloBotControl started!" << endl;
	cout << "[ INFO ] Press button to quit!" << endl;
	cin.ignore();
	// Call signal handler to cancel HoloBotControl
	signal_handler(SIGUSR1);
	

}