#include "./common/can_common_api.h"
#include "utils_userapi.h"

void canopen_access_obj_init(void)
{
  int ptz_horiz_motion_ctrl_init(void);
  ptz_horiz_motion_ctrl_init();
}

/**
  * @brief  Canopen_application_entry
  * @param  NONE
  * @retval NONE
  */
int canopen_application_entry(void)
{
  canopen_system_init();
  canopen_access_obj_init();
  
  return 0;
}

THREAD_ENTRY_EXPORT(canopen_application_entry,0);