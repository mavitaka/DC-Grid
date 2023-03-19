

/**** valeur par default ********/
#define DEFAULT	0
#define BLOCKED -1


/*******Priorité des taches*******/
#define TaskMPPT_PRIORITY	2
#define TaskCurrentRegulation_PRIORITY	2
#define TaskBatteryFull_PRIORITY	3
#define TaskDayNight_PRIORITY	5
#define TaskLedDriving_PRIORITY	2
#define	TaskAlarm_PRIORITY	1 
#define TaskPowerManagement_PRIORITY	2

/******Priorité des HWI***********/
#define ADC_COMP_ISR_HWI_PRIORITY	33	//Interrupt Priority >=32 !!!!!!
#define UART_CPL_ISR_HWI_PRIORITY	35	//Interrupt Priority >=32 !!!!!!


/******Priorité des SWI***********/
#define ADC_COMP_SWI_PRIORITY 	3
#define ControlFlowSystem_PRIORITY	4	
#define ControlCplCom_SWI_PRIORITY	2



/*******HWI numéro interruption*******/
#define ADC_COMP_INTERRUPT_NUMBER	31
#define UART_CPL_INTERRUPT_NUMBER	77





