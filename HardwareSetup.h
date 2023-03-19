#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"





/******définition constante ************/
#define DEFAULT 0 // valeur utiliser par défault à remplacer 

#define SEQUENCEUR_0	0
#define SEQUENCEUR_1	1

#define FIRST_LEVEL_COMPARATOR 1523	//1591 // valeur numérique premier niveau de tension
#define SECOND_LEVEL_COMPARATOR 1556	//1658  // valeur numérique deuxieme  niveau de tension
#define LOW_LEVEL_BATTERY 0

#define ALPHA_CURRENT	0.38
#define BETA_CURRENT	0.2

#define COMPARATEUR0	0
#define COMPARATEUR1	1

/***************************************/

/******Hardware Configuration***********/
void UartConfig(void);
void GpioClockPeriphConfig(void);
void AdcConfig(void);
void PWMConfig(void);
void InitHardware(void);
/*************************************/

