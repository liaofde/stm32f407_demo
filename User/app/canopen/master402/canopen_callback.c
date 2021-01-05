/*
This file is part of CanFestival, a library implementing CanOpen Stack. 

Copyright (C): Edouard TISSERANT and Francis DUPIN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "canfestival.h"
#include "master402_od.h"
#include "canopen_callback.h"
#include "master402_canopen.h"

/*****************************************************************************/
void master402_heartbeatError(CO_Data* d, UNS8 heartbeatID)
{
	printf("heartbeatError %d\n", heartbeatID);
}

void master402_initialisation(CO_Data* d)
{
	printf("canfestival enter initialisation state\n");
}

void master402_preOperational(CO_Data* d)
{
	rt_thread_t tid;
	printf("canfestival enter preOperational state\n");
	tid = rt_thread_create("co_cfg", canopen_start_thread_entry, NULL, 1024, 12, 2);
	if(tid == NULL)
	{
		printf("canfestival config thread start failed!\n");
	}
}

void master402_operational(CO_Data* d)
{
	printf("canfestival enter operational state\n");
}

void master402_stopped(CO_Data* d)
{
	printf("canfestival enter stop state\n");
}

void master402_post_sync(CO_Data* d)
{
	
}

void master402_post_TPDO(CO_Data* d)
{

}

void master402_storeODSubIndex(CO_Data* d, UNS16 wIndex, UNS8 bSubindex)
{
	/*TODO : 
	 * - call getODEntry for index and subindex, 
	 * - save content to file, database, flash, nvram, ...
	 * 
	 * To ease flash organisation, index of variable to store
	 * can be established by scanning d->objdict[d->ObjdictSize]
	 * for variables to store.
	 * 
	 * */
	printf("storeODSubIndex : %4.4x %2.2x\n", wIndex,  bSubindex);
}

void master402_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg, const UNS8 errSpec[5])
{
	printf("received EMCY message. Node: %2.2x  ErrorCode: %4.4x  ErrorRegister: %2.2x\n", nodeID, errCode, errReg);
}
