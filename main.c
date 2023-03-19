
#include "Task_RTOS.h"

Int main()
{ 
    InitHardware();
    InitSoftware();
    System_printf("hello world\n");
    mode_change(IDLE);
    BIOS_start();    /* does not return */
    return(0);
}
