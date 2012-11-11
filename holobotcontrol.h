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

#include <iostream>
#include <time.h>
#include <sstream>
#include <pthread.h>
#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <signal.h>
#include <sys/param.h>

#include "mongoose/mongcpp.h"
#include "omnidrive.h"
#include "liquidcrystal_i2c.h"


using namespace mongoose;
using namespace std;

class HoloBotControl: public MongooseServer {
	
	public:

		/** Message to control bot functions */
		static const string COMMAND_MESSAGE;
		/** Message for heartbeat mechanism and status informations for client */
		static const string HEARTBEAT_MESSAGE;
		
		/**
		* \brief  Constructor
		*/
		HoloBotControl();
		
		/**
		* \brief  Destructor
		*/
		~HoloBotControl();
		
		
	protected:

		/** Member to control bots movement */
		OmniDrive* omnidrive;
		/** Member to control i2c display */
		LiquidCrystal_I2C* display;
		/** Thread handle for heartbeat mechanism */
		pthread_t heartbeatThread;
		/** Thread handle to display NICs IP addresses */
		pthread_t displayIPAdressesThread;
		
		/**
		* \brief  Callback function to handle incomming http requests.
		*
		*         By incomming request the function decides on URL what the robot should do:
		*			- /command = Control- command from Client to drive bot
		*			- /heartbeat = Heartbeat to check if Bot is in WLAN- range 
		*
		* \param      eventCode  Kind of event
		* \param      &connection  Connection
		* \param      &request  Object of incomming Request which all sended informations
		* \param      &response  Connection informations to response to client
		* \return                 Success of handling
		*
		*/
		virtual bool handleEvent(ServerHandlingEvent eventCode, MongooseConnection &connection, const MongooseRequest &request, MongooseResponse &response);
		
		/**
		* \brief  Hanle incomming HTTP request by respond with status codes
		*
		*         Send client Status- code: 200 -> OK
		*
		* \param      &request  Object of incomming Request which all sended informations
		* \param      &response  Connection informations to response to client
		*/
		void handleInfo(const MongooseRequest &request, MongooseResponse &response);
		
		/**
		* \brief  Function for heartbeat mechanism
		*
		*         Controls incomming heartbeats to notice when connection get lost
		*
		* \param      object  Object
		*/
		static void* checkHeartbeat(void* object);
		
		/**
		* \brief  Function to display IP addresses of all active NICs
		*
		* \param      object  Object
		*/
		static void* displayLocalIpAddresses(void *object);
		
		/**
		* \brief  Generates status message to tell client battery status
		*
		*/
		const string generateStatusContent();
	
		/**
		* \brief  Convert bool to string
		*
		* \param      value Bool to convert
		*/
		inline const string boolToString(bool value){return value ? "true" : "false";}
		
};
/** Message to control bot functions */
const string HoloBotControl::COMMAND_MESSAGE = "/command";
const string HoloBotControl::HEARTBEAT_MESSAGE = "/heartbeat";