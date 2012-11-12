/** Class to monitor and protect an 3S Lipo Battery
 *  License: MIT
 *   
 *  @author Lukas Dudichum 
 *  @date 11.11.2012
 * 
 */

#include <iostream>
#include <time.h>
#include <sstream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

using namespace std;

class BatteryMonitor{
		
	private:
		/** Voltage of cell 1*/
		float voltageCell1;
		/** Voltage of cell 2*/
		float voltageCell2;
		/** Voltage of cell 3*/
		float voltageCell3;
		/** Voltage of whole battery*/
		float voltageOverall;
		
		/** Handle for i2c stream */
		int file_descriptor;
	
	public:
		/**
		* \brief  Constructor
		*/
		BatteryMonitor();
		
		
		/**
		* \brief  Function to check voltage of all lipo cells
		*
		* \param      object  Object
		*/
		static void* checkVoltage(void *object);
		
		/**
		* \brief  Function to get voltage of specific cell
		*
		* \param      cell	Cellnumber, valid range (1-3)
		* \return                 Current voltage of cell
		*/
		float getCellVoltage(char cellnumber);
		
		/**
		* \brief  Function to get overall voltage
		*
		* \return                 Current overall voltage
		*/
		float getOverallVoltage();
		
		/**
		* \brief  Function to show user that battery is really low
		*/
		void lowBatteryAlarm();
		
	protected:

		
};