/**
 * @file wastegate_crl.c
 * Fireblade Wategate Control Application
 *
 * @author Duncan Greer <duncan.greer@fiveringsaero.com.au>
 */

#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>

#include <uORB/uORB.h>
#include <uORB/topics/sensor_wastegate.h>
#include <uORB/topics/actuator_wastegate.h>

__EXPORT int wastegate_ctrl_main(int argc, char *argv[]);

int wastegate_ctrl_main(int argc, char *argv[])
{
	PX4_INFO("Starting Fireblade Wastegate Control Application");

	/* subscribe to sensor_combined topic */
	int sensor_sub_fd = orb_subscribe(ORB_ID(sensor_wastegate));
	orb_set_interval(sensor_sub_fd, 1000);

	/* advertise attitude topic */
	struct actuator_wastegate_s act;
	memset(&act, 0, sizeof(act));
	orb_advert_t act_pub = orb_advertise(ORB_ID(actuator_wastegate), &act);

	/* one could wait for multiple topics with this technique, just using one here */
	px4_pollfd_struct_t fds[] = {
		{ .fd = sensor_sub_fd,   .events = POLLIN },
		/* there could be more file descriptors here, in the form like:
		 * { .fd = other_sub_fd,   .events = POLLIN },
		 */
	};

	int error_counter = 0;

	for (int i = 0; i < 5; i++) {
		/* wait for sensor update of 1 file descriptor for 1000 ms (1 second) */
		int poll_ret = px4_poll(fds, 1, 1000);

		/* handle the poll result */
		if (poll_ret == 0) {
			/* this means none of our providers is giving us data */
			PX4_ERR("[wastegate_ctrl] Got no data within a second");

		} else if (poll_ret < 0) {
			/* this is seriously bad - should be an emergency */
			if (error_counter < 10 || error_counter % 50 == 0) {
				/* use a counter to prevent flooding (and slowing us down) */
				PX4_ERR("[wastegate_ctrl] ERROR return value from poll(): %d"
					, poll_ret);
			}

			error_counter++;

		} else {

			if (fds[0].revents & POLLIN) {
				/* obtained data for the first file descriptor */
				struct sensor_wastegate_s raw;
				/* copy sensors raw data into local buffer */
				orb_copy(ORB_ID(sensor_wastegate), sensor_sub_fd, &raw);

				PX4_WARN("[wastegate_ctrl] Position:\t%8.4f",
					 (double)raw.position);

				act.velocity_cmd = 0;

				orb_publish(ORB_ID(actuator_wastegate), act_pub, &act);
			}


		}
	}

	PX4_INFO("exiting");

	return 0;
}
