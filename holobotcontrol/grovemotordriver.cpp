/** Class to control Grove- motordriver
 *  License: MIT
 *   
 *  @author Lukas Dudichum 
 *  @date 10.10.2012
 * 
 */

#include "grovemotordriver.h"


GroveMotorDriver::GroveMotorDriver(uint8_t i2c_address){
	
	// Default motorspeed-> maximum
	this->max_motor_speed_a = 255;
	this->max_motor_speed_b = 255;
	
	// Open i2c device
	this->file_descriptor = open(I2C_DEVICE.c_str(), O_RDWR);
	
	// Check if open device was successful
	if (this->file_descriptor < 0) {
		cout << "[ FAILED ] GroveMotorDriver: ERROR opening device: " << I2C_DEVICE << endl;
		exit(1);
	}
	else cout << "[ OK ] GroveMotorDriver: Opening I2C device: " << I2C_DEVICE << endl;
	
	// Set opened file to i2c slave I/O- device
	if (ioctl(file_descriptor, I2C_SLAVE, i2c_address) < 0) {
		cout << "[ FAILED ] GroveMotorDriver: Set to I2C device failed" << endl;
		exit(1);
	}
	else cout << "[ OK ] GroveMotorDriver: Set to I2C device successful" << endl;
	
	// Calculate relative motor stepping
	this->motor_step_a = (float)this->max_motor_speed_a/100.0;
	this->motor_step_b = (float)this->max_motor_speed_b/100.0;
	
}
	
GroveMotorDriver::~GroveMotorDriver(){
	
}

bool GroveMotorDriver::setMotorSpeedAB(uint8_t motorspeed_a,uint8_t motorspeed_b){
	
	return sendI2C(this->MOTOR_SPEED_SET_AB, motorspeed_a*this->motor_step_a, motorspeed_b*this->motor_step_b);

}

bool GroveMotorDriver::setMotorA(uint8_t motorspeed_a, uint8_t motor_direction){
	
	return sendI2C(this->MOTOR_DIRECTION_AND_SPEED_A, motor_direction, motorspeed_a*this->motor_step_a);

}

bool GroveMotorDriver::setMotorB(uint8_t motorspeed_b, uint8_t motor_direction){
	
	return sendI2C(this->MOTOR_DIRECTION_AND_SPEED_B, motor_direction, motorspeed_b*this->motor_step_b);
		
}

void GroveMotorDriver::setMotorMaxSpeedAB(uint8_t max_motorspeed_a, uint8_t max_motorspeed_b){
	
	this->max_motor_speed_a = max_motorspeed_a;
	this->max_motor_speed_b = max_motorspeed_b;
	
	// Recalculate relative motor stepping
	this->motor_step_a = this->max_motor_speed_a/100;
	this->motor_step_b = this->max_motor_speed_b/100;
}

bool GroveMotorDriver::sendI2C(uint8_t command, uint8_t arg1, uint8_t arg2){
	
	char write_buffer[3];
	write_buffer[0] = command;
	write_buffer[1] = arg1;
	write_buffer[2] = arg2;
	
	if (write(this->file_descriptor, write_buffer, 3) != 3) {
		cout << "[ FAILED ] GroveMotorDriver: Write on i2c bus failed" << endl;
		return false;
	}
	return true;
}