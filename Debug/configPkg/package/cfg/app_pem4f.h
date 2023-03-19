/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-D05
 */

#include <xdc/std.h>

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle SemaphoreJourNuit;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle SemaphoreMesure;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle SemaphoreBattAutreGrid;

#include <ti/sysbios/knl/Mailbox.h>
extern const ti_sysbios_knl_Mailbox_Handle MailBoxSupervisionAlarm;

#include <ti/sysbios/knl/Mailbox.h>
extern const ti_sysbios_knl_Mailbox_Handle MailBoxDataOtherGrid;

#include <ti/sysbios/hal/Timer.h>
extern const ti_sysbios_hal_Timer_Handle TimerMesure;

#include <ti/sysbios/hal/Hwi.h>
extern const ti_sysbios_hal_Hwi_Handle ADC_COMP_Handle;

#include <ti/sysbios/hal/Hwi.h>
extern const ti_sysbios_hal_Hwi_Handle UART_CPL_Handle;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle ADC_COMP_SWI_Handle;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle ControlFlowSystem_Handle;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle ControlCplCom_SWI_Handle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TaskMPPT_Handle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TaskCurrentRegulation_Handle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TaskLedDriving_Handle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TaskBatteryFull_Handle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TaskDayNight_Handle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TaskAlarm_Handle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TaskPowerManagement_Handle;

#include <ti/sysbios/hal/Timer.h>
extern const ti_sysbios_hal_Timer_Handle TimerSupervision;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

