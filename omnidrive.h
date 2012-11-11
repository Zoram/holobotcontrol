/** Class to calculate motor- rpms for an 4 wheel omnidrive
 *  License: MIT
 *   
 *  @author Lukas Dudichum 
 *  @date 31.10.2012
 * 
 */

#include <iostream>
#include <cmath>
#include <math.h>
#include "grovemotordriver.h"

class OmniDrive{
		
	private:
		
		/** Handle for driver A */
		GroveMotorDriver *motorDriverA;
		/** Handle for driver B */
		GroveMotorDriver *motorDriverB;
	
	public:
		
		/** Direction of motor */
		enum RotateDirection {CLOCKWISE, COUNTERCLOCKWISE, NONE};
		
		/**
		* \brief  Constructor
		*/
		OmniDrive();
		
		/**
		* \brief  Function to drive bot in an specific angle
		*
		*			 Calculation Steps needed:
		*         1. Calculate motor ratios (trigonometrically)
		*         2. Substract some translation motor speed to rotate clockwise/counterclockwise
		*			 3. Algebra to get from motor speed ratio the motor speed values in a valid range from 0 to 100
		*
		* \param      angle	Angle related to bots front counterclockwise
		* \param      direction	Direction to rotate
		*/
		void driveInSpecificAngle(float angle, RotateDirection direction);
		
		/**
		* \brief  Function to rotate Bot
		*
		* \param      direction  Direction to rotate
		*
		*/
		void rotate(RotateDirection direction);
		
		/**
		* \brief  Set reserve for rotation when bot drives
		*
		* \param      rotationReserve  Reserve for rotation when bot drives in percentage
		*
		*/
		void setRotationReserve(double rotationReserve);
		
		/**
		* \brief  Function to stop Bot
		*/
		void stop();
		
		
	protected:
		
		/** Reserve for additional rotation to normal driving direction*/
		double rotationReserve;
		
	 /**
		* \brief  Function to get motor speeds in valid range
		* \param      wheelsInXDirection	Value for X direction
		* \param      wheelsInYDirection Value for Y direction
		* \param      rotateDirection Direction to rotate
		*/
		void ratioToValidRange(double wheelsInXDirection, double wheelsInYDirection, RotateDirection rotateDirection);
		
};