



#ifndef _CANAERO_CAN_H
#define _CANAERO_CAN_H


#include <nuttx/config.h>


// can_devinit implemented in apps/drivers/boards/px4fmu/px4fmu_can.c
int can_devinit(void);

#define CANAERO_NODE_ID 255


struct canaero_NOD_msg 
{
	unsigned char nodeId;
	unsigned char dataType;
	unsigned char serviceCode;
	unsigned char messageCode;
	unsigned char msgData[4];
};

int canaero_NOD_msg_pack(struct canaero_NOD_msg *msg, unsigned char *data);

#define CANAERO_DLC 8

#define CANAERO_ID_BODY_X_ACC 300
#define CANAERO_ID_BODY_Y_ACC 301
#define CANAERO_ID_BODY_Z_ACC 302
#define CANAERO_ID_BODY_P_RATE 304
#define CANAERO_ID_BODY_Q_RATE 303
#define CANAERO_ID_BODY_R_RATE 305
#define CANAERO_ID_RUDDER_POS 306
#define CANAERO_ID_STAB_POS 307
#define CANAERO_ID_ELEV_POS 308
#define CANAERO_ID_LEFT_AIL_POS 309
#define CANAERO_ID_RIGHT_AIL_POS 310
#define CANAERO_ID_BODY_PITCH_ANGLE 311
#define CANAERO_ID_BODY_ROLL_ANGLE 312
#define CANAERO_ID_BODY_YAW_ANGLE 321
#define CANAERO_ID_BODY_SIDESLIP_ANGLE 313
#define CANAERO_ID_ALTITUDE_RATE 314



#endif


