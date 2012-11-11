/** Class to calculate motor- rpms for an 4 wheel omnidrive
 *  License: MIT
 *   
 *  @author Lukas Dudichum 
 *  @date 31.10.2012
 * 
 */

#include "omnidrive.h"

void OmniDrive::ratioToValidRange(double wheelsInXDirection, double wheelsInYDirection, RotateDirection direction ){
	
	double xWheels = 0;
	double yWheels = 0;
	int directionMotorA = 0;
	int directionMotorB = 0;
	
	// 2. Calculate valid motor ratio (-100 to 100)
	if(abs(wheelsInXDirection)>abs(wheelsInYDirection)){
		xWheels = abs((wheelsInXDirection/wheelsInXDirection))*100.0;
		yWheels = abs((wheelsInYDirection/wheelsInXDirection))*100.0;
	}else {
		xWheels = abs((wheelsInXDirection/wheelsInYDirection))*100.0;
		yWheels = abs((wheelsInYDirection/wheelsInYDirection))*100.0;
	}
	
	// Get algebraic sign back
	if(wheelsInXDirection<0)xWheels = xWheels*-1.0;
	if(wheelsInYDirection<0)yWheels = yWheels*-1.0;
	
	// 3. Substract some motor velocity to rotate where the bot drives
	// later...
	
	// 4. Send commands to motor driver
	// Motorspeed/ direction driver A
	if(xWheels < 0){
		directionMotorA = GroveMotorDriver::ROTATE_FORWARD;
		directionMotorB = GroveMotorDriver::ROTATE_BACKWARD;
	}else{
		directionMotorA = GroveMotorDriver::ROTATE_BACKWARD;
		directionMotorB = GroveMotorDriver::ROTATE_FORWARD;
	}
	this->motorDriverA->setMotorA((int)abs(xWheels),directionMotorA);
	this->motorDriverA->setMotorB((int)abs(xWheels),directionMotorB);
	
	// Motorspeed/ direction driver B
	if(yWheels < 0){
		directionMotorA = GroveMotorDriver::ROTATE_FORWARD;
		directionMotorB = GroveMotorDriver::ROTATE_BACKWARD;
	}else{
		directionMotorA = GroveMotorDriver::ROTATE_BACKWARD;
		directionMotorB = GroveMotorDriver::ROTATE_FORWARD;
	}
	this->motorDriverB->setMotorA((int)abs(yWheels),directionMotorA);
	this->motorDriverB->setMotorB((int)abs(yWheels),directionMotorB);
	
}
		
OmniDrive::OmniDrive(){
	
	this->rotationReserve = 15.0;
	this->motorDriverA = new GroveMotorDriver(I2C_ADDR_DRIVER_A);
	this->motorDriverB = new GroveMotorDriver(I2C_ADDR_DRIVER_B);
}
		
void OmniDrive::driveInSpecificAngle(float angle, RotateDirection direction){

	// Add 45 degrees because all calculations are made whith polar coordinates, so bots front equals 0 degree
	angle += 45;
	
	// 1. Calculate motor ratio in polar coordinates for given angle
	double divisorX = cos((angle*M_PI)/180.0);
	double divisorY = sin((angle*M_PI)/180.0);
	
	this->ratioToValidRange(divisorX, divisorY, direction);	

}
		
void OmniDrive::rotate(RotateDirection direction){
	
}

void OmniDrive::stop(){
	this->motorDriverA->setMotorSpeedAB(0,0);
}

void OmniDrive::setRotationReserve(double rotationReserve){
	this->rotationReserve = rotationReserve;
}