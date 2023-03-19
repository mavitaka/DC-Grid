

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#if DEBUG_SERIE
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Timestamp.h>
#endif 

#include "HardwareSetup.h"
#include "TacheUtilite.h"
#include "ConstanteCom.h"


/****************définition des constantes******************/

#define VOLTAGE_GAIN      	   11

//Mode de fonctionnement
#define IDLE					0
#define BATTERY_CHARGING		1
#define LED_DRIVING_DIMMING				2
#define BATTERY_VOLTAGE_FIRST_LEVEL	4
#define BATTERY_FULL	5
#define BATTERY_EMPTY 6
#define	POWER_MANAGEMENT	7 

#define DAY_FUNCTIONNEMENT		10
#define NIGHT_FUNCTIONNEMENT	11


#define BATTERY_VOLTAGE_FIRST_LEVEL_TRiGGER	1
#define BATTERY_VOLTAGE_SECOND_LEVEL_TRiGGER	2

#define BATTERY_VOLTAGE_MIN	DEFAULT
#define BATTERY_POWER_MIN	DEFAULT


#define Q_FIRST_INCREMENT   20
#define Q_ALPHA 0.005
#define Q_BETA  0.04

/***********************************************************/


/*********** Definition Variable programme *****************/
union DonnePhysique
{
	float ValeurFloat;
	uint8_t ValeurByte[4];
};
typedef union DonnePhysique DonnePhysique;

DonnePhysique V, Vpv_new, Vpv_old, I_pv, V_bat, I_bat, V_led, I_led, P, P_out, V_pv,P_led,temp;
uint16_t D_zeta;
float Q;
uint16_t Dzeta_new ;
uint16_t Dzeta_old ;
uint16_t D_sepic ;
uint16_t D_dimming ;

uint32_t ADC0Value[8];
uint8_t*  AddrVar[11];

 uint8_t Mode_Actuel; 
 uint8_t ModeJourNuit;
 uint8_t BatterieOtherGridState;
// non utiliser pour l'instant
//uint16_t lux;
//uint16_t PIR;

/***********************************************************/


/************Liste Semaphore*******************************/
//SemaphoreJourNuit 	Switch mode jour/nuit
//SemaphoreMesure		Sempahore Utiliser pour les mesures	
//SemaphoreBattAutreGrid	Sempahore utiliser pour échanger information entre TaskBatteryFull et ControlCplCom_SWI
/*********************************************************/

/*********Liste des mails box*****************************/
//MailBoxSupervisionAlarm	Mailbox utiliser pour mettres des changements d'états/informations pour la supervision	taille : 	NbMsg:
//MailBoxDataOtherGrid	Mailbox utiliser pour obtenir le retour d'information de data d'un autre grid	taille : 1 :Sizeof float ( 4 octets)
/*********************************************************/

/*******Liste Software Timer********************/

//TimerMesure nom du handle 
//Timer configuer pour ne pas démarrer automatiquement au début du  programme 

/**********************************************/


/**********Définition des tâches***************************/
void TaskMPPT(void); // taskHandle : TaskMPPT_Handle; 
void TaskCurrentRegulation(void); // taskHandle : TaskCurrentRegulation_Handle;
void TaskLedDriving(void);// taskHandle : TaskLedDriving_Handle;                                   
void TaskBatteryFull(void); // taskHandle : TaskBatteryFull_Handle
void TaskDayNight(void); // taskHandle : TaskDayNight_Handle
void TaskAlarm(void); // taskHandle : TaskAlarm_Handle; 
void TaskPowerManagement(void);// taskHandle : TaskPowerManagement_Handle
/******************************************************/

/*****Définition des HWI Hardware interrupt***************/
void ADC_COMP_ISR_HWI(void); // HWI Handle : ADC_COMP_Handle	Interrupt Priority >=32 !!!!!! 
void UART_CPL_ISR_HWI(void);// HWI Handle : UART_CPL_Handle		Interrupt Priority >=32 !!!!!!

/**********************************************************/



/*******Définition des SWI Software interrupt**************/
void ADC_COMP_SWI(void); // SWI Handle : ADC_COMP_SWI_Handle 
void ControlFlowSystem_SWI(void); // SWI Handle : ControlFlowSystem_Handle
void ControlCplCom_SWI(void); // SWI Handle : ControlCplCom_Handle
/**********************************************************/

/********Software Timer ISR********************************/
void PeriodicMesure_SWI(void); // associé au timer software TimerMesure
/**********************************************************/

/****************** MPPT **********************************/
uint16_t Voltage_based(uint16_t duty_cycle, float Q);
/**********************************************************/

/*******ATMEL <==> TIVA communication en UART**************/
void ComAtmelCPL(uint8_t commande, uint8_t Req_Rep,uint8_t* val);
/*********************************************************/

/************Mode Fonctionnement*************************/
int GetSigne(float value);
void mode_change(uint8_t mode);
void InitSoftware(void);
void InitFonctionnement(uint8_t mode_init);
void TransmitDataSupervision(uint8_t * data,uint8_t addr);
void TransmitAlarmSupervision(uint8_t type);
void PostAlarm(uint8_t TypeAlarme);
/********************************************************/



