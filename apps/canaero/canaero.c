


#include <nuttx/config.h>

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/can.h>

#include "can.h"


#include <poll.h>

/* orb topics */
#include <uORB/topics/sensor_combined.h>
 
__EXPORT int canaero_main(int argc, char *argv[]);


/* 

// defined in nuttx/include/nuttx/can.h

struct can_hdr_s
{
  uint16_t      ch_dlc   : 4;  //4-bit DLC
  uint16_t      ch_rtr   : 1;  // RTR indication
  uint16_t      ch_id    : 11; // 11-bit standard ID
} packed_struct;


struct can_msg_s
{
  struct can_hdr_s cm_hdr;                  // The CAN header 
  uint8_t          cm_data[CAN_MAXDATALEN]; // CAN message data (0-8 byte) 
} packed_struct;

*/

int canaero_main(int argc, char *argv[])
{

	struct can_msg_s txmsg;
	struct can_msg_s rxmsg;
	int ret;
	int fd;
	int msgsize, nbytes;
	int i;
	int errval;

	printf("Starting CANAero\n");

 	printf("[canaero] Initializing external CAN device\n");
  	ret = can_devinit();
	if (ret != OK)
    {
    	printf("[canaero] ERROR: can_devinit failed: %d\n", ret);
    	errval = 1;

    	return -errval;
    }

  /* Open the CAN device for reading */

  printf("[canaero] Hardware initialized. Opening the CAN device\n");
  fd = open("/dev/can0", O_RDWR);
  if (fd < 0)
	{
		printf("[canaero] ERROR: open %s failed: %d\n",
              "/dev/can0", errno);
      errval = 2;
      return -errval;
    }


	// subscribe to sensor_combined topic
	int sensor_sub_fd = orb_subscribe(ORB_ID(sensor_combined));
	
	// set interval that we are interested in getting the data at
	orb_set_interval(sensor_sub_fd, 100); // 500ms

	struct pollfd fds[] = {
		{ .fd = sensor_sub_fd, .events = POLLIN },

	};

	int error_counter = 0;

	while(true)
	{
		
		int poll_ret = poll(fds,1,1000);  // wait for sensor update of 1 file descriptor for 1000ms

		if (poll_ret == 0)
		{
			printf("[canaero] Got no data within 1 sec\n");
		} else if (poll_ret < 0)
		{
			if (error_counter < 10 || error_counter % 50 == 0)
			{
				printf("[canaero] ERROR: return value from poll(): %d\n", poll_ret);
			}
			++error_counter;
		} else
		{
			// retrieve the data

			
			if (fds[0].revents & POLLIN)
			{
				struct sensor_combined_s raw;
				orb_copy(ORB_ID(sensor_combined), sensor_sub_fd, &raw);
				
				// send on CANBus
				txmsg.cm_hdr.ch_id    = CANAERO_ID_BODY_X_ACC;
				txmsg.cm_hdr.ch_rtr   = false;
				txmsg.cm_hdr.ch_dlc   = CANAERO_DLC;

				// fill in the message data
				txmsg.cm_data[0];
				


				msgsize = CAN_MSGLEN(txmsg.cm_hdr.ch_dlc);
				nbytes = write(fd, &txmsg, msgsize);
				if (nbytes != msgsize)
				{
					printf("[canaero] ERROR: write(%d) returned %d\n", msgsize, nbytes);
					errval = 3;
					return -errval; // TODO: exit gracefully?
				}

			}
		}
	}
	return OK;


}

int canaero_NOD_msg_pack(struct canaero_NOD_msg *msg, unsigned char *data)
{

	// check for null pointers
	if(data == NULL) return -1;
	if(msg == NULL) return -1;

	data[0] = msg->nodeId; // node ID
	data[1] = msg->dataType; // Data Type
	data[2] = msg->serviceCode; // Service Code
	data[3] = msg->messageCode; // Message Code
	data[4] = msg->msgData[0];
	data[5] = msg->msgData[1];
	data[6] = msg->msgData[2];
	data[7] = msg->msgData[3];

	return OK;

}

