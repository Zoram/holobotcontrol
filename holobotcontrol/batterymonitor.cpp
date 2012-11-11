/** Class to monitor and protect an 3S Lipo Battery
 *  License: MIT
 *   
 *  @author Lukas Dudichum 
 *  @date 11.11.2012
 * 
 */

#include "batterymonitor.h"

/** Mutex to protect display */
extern pthread_mutex_t displayMutex;
/** Device to communicate with A/D converter */
static const string BATTERY_I2C_DEVICE = "/dev/i2c-1";
/** Address for first A/D converter input a1 - a4 */
static const uint8_t ADC_1_I2C_ADDRESS = 0x68;
/** Address for second A/D converter input a5 - a8 */
static const uint8_t ADC_2_I2C_ADDRESS = 0x69;

BatteryMonitor::BatteryMonitor(){
	
	// Open i2c device
	this->file_descriptor = open("/dev/i2c-1", O_RDWR);

	// Check if open device was successful
	if (this->file_descriptor < 0) {
		cout << "[ FAILED ] BatteryMonitor: ERROR opening device: " << BATTERY_I2C_DEVICE << endl;
		exit(1);
	}
	else cout << "[ OK ] BatteryMonitor: Opening I2C device: " << BATTERY_I2C_DEVICE << endl;
	
	// Set opened file to i2c slave I/O- device
	if (ioctl(this->file_descriptor, I2C_SLAVE, ADC_2_I2C_ADDRESS) < 0) {
		cout << "[ FAILED ] BatteryMonitor: Set to I2C device failed" << endl;
		exit(1);
	}
	else cout << "[ OK ] BatteryMonitor: Set to I2C device successful" << endl;
}


void* BatteryMonitor::checkVoltage(void *object){
	
}

float BatteryMonitor::getCellVoltage(char cellnumber){
	float ret;
	switch(cellnumber){
		case 1: ret = this->voltageCell1; break;
		case 2: ret = this->voltageCell2; break;
		case 3: ret = this->voltageCell1; break;
		default: ret = -1; break;
		break;
	}
	return ret;
}

float BatteryMonitor::getOverallVoltage(){
	
}
		
void BatteryMonitor::lowBatteryAlarm(){
	
}
