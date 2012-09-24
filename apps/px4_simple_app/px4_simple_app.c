/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *   Author: @author Example User <mail@example.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
 
/**
 * @file px4_simple_app.c
 * Minimal application example for PX4 autopilot.
 * Implements example from https://pixhawk.ethz.ch/px4/dev/hello_sky#step_4upload_and_test_the_app
 */
 
#include <nuttx/config.h>
#include <stdio.h>
#include <errno.h>

// use poll() for blocking wait on the orb fd. note poll() vs select(): http://www.unixguide.net/network/socketfaq/2.14.shtml
#include <poll.h>

/* orb topics */
#include <uORB/topics/sensor_combined.h>
 
__EXPORT int px4_simple_app_main(int argc, char *argv[]);





int px4_simple_app_main(int argc, char *argv[])
{
	printf("Starting Simple PX4 App!\n");

	// subscribe to sensor_combined topic
	int sensor_sub_fd = orb_subscribe(ORB_ID(sensor_combined));
	
	// set interval that we are interested in getting the data at
	orb_set_interval(sensor_sub_fd, 500); // 500ms

	struct pollfd fds[] = {
		{ .fd = sensor_sub_fd, .events = POLLIN },

	};

	int error_counter = 0;

	while(true)
	{
		
		int poll_ret = poll(fds,1,1000);  // wait for sensor update of 1 file descriptor for 1000ms

		if (poll_ret == 0)
		{
			printf("[px4_simple_app] Got no data within 1 sec\n");
		} else if (poll_ret < 0)
		{
			if (error_counter < 10 || error_counter % 50 == 0)
			{
				printf("[px4_simple_app] ERROR: return value from poll(): %d\n", poll_ret);
			}
			++error_counter;
		} else
		{
			// retrieve the data

			
			if (fds[0].revents & POLLIN)
			{
				struct sensor_combined_s raw;
				orb_copy(ORB_ID(sensor_combined), sensor_sub_fd, &raw);
				printf("Accelerometer:\t%8.4lf\t%8.4lf\t%8.4lf\n",  
					(double)raw.accelerometer_m_s2[0],
					(double)raw.accelerometer_m_s2[1],
					(double)raw.accelerometer_m_s2[2]);
			}
		}
	}
	return OK;
}


