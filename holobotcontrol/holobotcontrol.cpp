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
	this->batterymonitor = new BatteryMonitor();

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
		
	// Open i2c device
	int file = open("/dev/i2c-1", O_RDWR);
	uint8_t buf[4]; 
	unsigned int dummy;
	stringstream s1,s2,s3;
	float cell1, cell2,cell3;
	// Check if open device was successful
	if (file < 0) {
		cout << "[ FAILED ] GroveMotorDriver: ERROR opening device: " << I2C_DEVICE << endl;
		exit(1);
	}
	else cout << "[ OK ] GroveMotorDriver: Opening I2C device: " << I2C_DEVICE << endl;
	
	// Set opened file to i2c slave I/O- device
	if (ioctl(file, I2C_SLAVE, 0x69) < 0) {
		cout << "[ FAILED ] GroveMotorDriver: Set to I2C device failed" << endl;
		exit(1);
	}
	else cout << "[ OK ] GroveMotorDriver: Set to I2C device successful" << endl;
		
	s1.precision(3);
	s2.precision(3);
	s3.precision(3);
		
		
	while(1){
		s1.str("");
		s2.str("");
		s3.str("");
		// wait 1s for incomming heartbeat 
		sleep(1);
// Just test
		i2c_smbus_write_byte (file, 0x98);
		usleep (70000);
		i2c_smbus_read_i2c_block_data(file,0x98,4,buf);
		usleep (70000);
		dummy = ((buf[0]<<8)|buf[1]);
	  	if (dummy>=32768) dummy=65536-dummy;
	  	cell1 = dummy * 0.000158;
		cell1 = cell1*0.985;
		cout << "Cell 1: " <<  cell1 << endl; 
		usleep (70000);
		i2c_smbus_write_byte (file, 0xb8);
		usleep (70000);
		i2c_smbus_read_i2c_block_data(file,0xb8,4,buf);
		usleep (700000);
		dummy = ((buf[0]<<8)|buf[1]);
	  	if (dummy>=32768) dummy=65536-dummy;
	  	cell2 = dummy * 0.000158;
	
		cell2 = (cell2*(1.814+1.047));
		cell2 = cell2 - cell1;
		cout << "Cell 2: " <<  cell2 << endl; 
		usleep (70000);
		i2c_smbus_write_byte (file, 0xd8);
		usleep (70000);
		i2c_smbus_read_i2c_block_data(file,0xd8,4,buf);
		usleep (70000);
		dummy = ((buf[0]<<8)|buf[1]);
	  	if (dummy>=32768) dummy=65536-dummy;
	  	cell3 = dummy * 0.000158;
		cell3 = (cell3*(2.7083+1.080));
		cell3 = cell3 - cell2 - cell1;
		cout << "Cell 3: " <<  cell3 << endl; 
		usleep (70000);
		cout << "Overall: " <<  cell3+cell2+cell1 << endl; 
	
		s1 << fixed << cell1;
		s2 << fixed << cell2;
		s3 << fixed << cell3;
	
		pthread_mutex_lock( &displayMutex );
		controlPt->display->printToRow(""+s1.str()+"V "+s2.str()+"V "+s3.str()+"V",20,3);
		pthread_mutex_unlock( &displayMutex );
// Test end
		
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