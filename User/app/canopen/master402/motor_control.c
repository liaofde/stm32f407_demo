#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "cmsis_os.h"
#include "canfestival.h"
#include "master402_od.h"
#include "master402_canopen.h"

#define SYNC_DELAY osDelay(20)
#define PROFILE_POSITION_MODE 1
#define ENCODER_RES (2500 * 4)

void servo_on(uint8_t nodeId)
{
	UNS32 speed;

	speed = ENCODER_RES * 30;
	modes_of_operation_6060 = PROFILE_POSITION_MODE;
	profile_velocity_6081 = speed;
	target_position_607a = 0;

	control_word_6040 = 0x06;
	SYNC_DELAY;
	control_word_6040 = 0x0f;
}


void relative_move(int32_t position, int32_t speed)
{
	target_position_607a = position;
	profile_velocity_6081 = speed;

	control_word_6040 = 0x6f;
	SYNC_DELAY;
	control_word_6040 = 0x7f;
}

void motorstate(void)
{
	printf("ControlWord 0x%0X\n", control_word_6040);
	printf("StatusWord 0x%0X\n", status_word_6041);
	printf("current position %d\n", position_actual_value_6063);
	printf("current speed %d\n", velocity_actual_value_606c);
}
