/** Class to control Grove- motordriver
 *  License: MIT
 *   
 *  @author Lukas Dudichum 
 *  @date 10.10.2012
 * 
 */

#include <inttypes.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <iostream>

using namespace std;

static const uint8_t I2C_ADDR_DRIVER_A = 0x0F;
static const uint8_t I2C_ADDR_DRIVER_B = 0x0E;

static const string I2C_DEVICE = "/dev/i2c-1";

class GroveMotorDriver{
	
	private:
		/** I2C address of motor driver */
		uint8_t i2c_addr;
		
		/** Direction of motor a */
		uint8_t motor_direction_a;
		/** Direction of motor b */
		uint8_t motor_direction_b;
		
		/** Maximum motor speed of a (0-255) */
		uint8_t max_motor_speed_a;
		/** Maximum motor speed of b (0-255) */
		uint8_t max_motor_speed_b;
		
		/** One motor step of a (1%) */
		float motor_step_a;
		/** One motor step of b (1%) */
		float motor_step_b;
		
		/** Handle for i2c stream */
		int file_descriptor;
		
		// Register Addresses
		static const uint8_t MOTOR_SPEED_SET_AB = 0x82;
		static const uint8_t PWM_FREQ_SET = 0x84;
		static const uint8_t Direction_SET_HBRIDGE= 0xAA;
		static const uint8_t MOTOR_DIRECTION_AND_SPEED_A = 0xA1;
		static const uint8_t MOTOR_DIRECTION_AND_SPEED_B = 0xA5;
		
		/**
		* \brief  Sends bytes on I2C bus
		*
		*         Sends: driver address -> registerfunction address -> argument 1 -> argument 2
		*
		* \param      command  Type of message
		* \param      arg1  Argument 1 of message
		* \return	  arg2  Argument 2 of message
		*/
		bool sendI2C(uint8_t command, uint8_t arg1, uint8_t arg2);
		
	public:
		
		/**
		* \brief  Constructor
		*/
		GroveMotorDriver(uint8_t i2c_address);
		
		/**
		* \brief  Destructor
		*/
		~GroveMotorDriver();
		
		// Rotation control values
		static const uint8_t  ROTATE_FORWARD= 0x01;
		static const uint8_t  ROTATE_BACKWARD= 0x02;
		
		/**
		* \brief  Sets motorspeed of motor A + B
		*
		*         Sends: driver address -> registerfunction address -> speed a -> speed b
		*
		* \param      motorspeed_a  Motorspeed A relative to max_motor_speed (percent)
		* \param      motorspeed_b  Motorspeed B relative to max_motor_speed (percent)
		* \return	  Command successful sended
		*/
		bool setMotorSpeedAB(uint8_t motorspeed_a,uint8_t motorspeed_b);
		
		/**
		* \brief  Sets motorspeed of motor A
		*
		*         Sends: driver address -> registerfunction address -> direction -> speed a
		*
		* \param      motorspeed_a  Motorspeed A relative to max_motor_speed (percent)
		* \param      motor_direction  Direction of motor a
		* \return	  Command successful sended
		*/
		bool setMotorA(uint8_t motorspeed_a, uint8_t motor_direction);
		
		/**
		* \brief  Sets motorspeed of motor B
		*
		*         Sends: driver address -> registerfunction address -> direction -> speed b
		*
		* \param      motorspeed_a  Motorspeed A relative to max_motor_speed (percent)
		* \param      motor_direction  Direction of motor b
		* \return	  Command successful sended
		*/
		bool setMotorB(uint8_t motorspeed_b, uint8_t motor_direction);
		
		/**
		* \brief  Sets max motorspeed of motor A/B
		*
		* \param      max_motorspeed_a  Max motorspeed of a (0-255)
		* \param      max_motorspeed_b  Max motorspeed of b (0-255)
		* \return	  Command successful sended
		*/
		void setMotorMaxSpeedAB(uint8_t max_motorspeed_a, uint8_t max_motorspeed_b);
		
	protected:
		
};