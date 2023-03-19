#include "HardwareSetup.h"





/**
 * @brief      Initialistion de tout les périphériques
 */
void InitHardware(void)
{
	GpioClockPeriphConfig();
	UartConfig();
	AdcConfig();
	PWMConfig();


}




/**
 * @brief    Configuration de Liaision UART/Serie Communication CPL + DEBUG + IHM
 * 			
 * 			 @CPL Communication 9600 Bauds/1 bits Stop/Pas de parité/Pas Flow Control (Hardware)
 * 			 @DEBUG Communication Via UART STDIO 9600 Bauds 16 MHz
 *			 @Supervision Communication  9600 Bauds/1 bits Stop/Pas de parité/Pas Flow Control (Hardware)	
 *			CPL Communication UART5 Port E PIN 4 et PIN 5 (PE4 RX & PE5 TX)
 			Supervision UART4	Port C	PIN 4 et PIN 5 (PC4 RX & PC5 TX)
 *			
 *			
 *			DEBUG Port USB standart UART0 Port A PIN 0 et PIN 1
 *			PA0 RX & PA1 TX 
 *			Note : Pour  DEBUG bien enregister comme symbole Prédéfini DEBUG_SERIE 				 
 *
 */

void UartConfig(void)
{

	UARTConfigSetExpClk(UART5_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_PAR_NONE | UART_CONFIG_WLEN_8 |UART_CONFIG_STOP_ONE));
    UARTFlowControlSet(UART5_BASE, UART_FLOWCONTROL_NONE) ; // pas de contrôle matériel
    UARTIntEnable(UART5_BASE, UART_INT_RX);
    UARTEnable(UART5_BASE);

    UARTConfigSetExpClk(UART4_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_PAR_NONE | UART_CONFIG_WLEN_8 |UART_CONFIG_STOP_ONE));
    UARTFlowControlSet(UART4_BASE, UART_FLOWCONTROL_NONE) ; // pas de contrôle matériel
    UARTEnable(UART4_BASE);


}


/**
 * @brief      Configuration des horloges & GPIO sur les Périphériques utilisés
 * PWM
 * ADC
 * GPIO
 * UART ATMEL-Supervision
 * Relais 
 */
void GpioClockPeriphConfig(void)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // ATMEL Uart Communication
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);// Relais
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);// PWM
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);// Supervision
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);// Enable/Disable RS485 Transmission


	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

	/*******ATMEL UART**********/
	GPIOPinConfigure(GPIO_PE4_U5RX);
	GPIOPinConfigure(GPIO_PE5_U5TX);
	GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	/*******Supervision**********/
	GPIOPinConfigure(GPIO_PC4_U4RX);
	GPIOPinConfigure(GPIO_PC5_U4TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1 |GPIO_PIN_0);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIO_PORTF_PCTL_R &= ~GPIO_PCTL_PF1_M & ~GPIO_PCTL_PF2_M & ~GPIO_PCTL_PF3_M; 
	GPIO_PORTF_PCTL_R |= GPIO_PCTL_PF1_M1PWM5 | GPIO_PCTL_PF2_M1PWM6 | GPIO_PCTL_PF3_M1PWM7;

	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2); // contrôle du relais
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0); // contrôle du Transceiver RS485 
	                                                       
	//GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5); Wifi Module Non utilisé
	//GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4); Capteur présence


}




/**
 * @brief      Configuration ADC0
 * 				
 * 				Mesure
 * @element 	AIN0, AIN1, AIN2, AIN3 correspondant à  PE3, PE2, PE1, PE0
 * @Sequence	Utilisation du séquenceur 8 case FIFO
 * @oversamplig 64 précision des mesures
 * @trigger software 
 * 				
 * 				Comparateur
 * 				
 * @Comparateur COMPARATEUR0 utilisé pour detecter  le Premier niveau Tension Batterie  FIRST_LEVEL_COMPARATOR ==> Fin Tache MPPT Debut Tache Regul Courant
 * 				COMPARATEUR1 utilisé pour detecter  le deuxieme niveau Tension de Batterie SECOND_LEVEL_COMPARATOR ==> fin Tache Regul Courant
 * 				
 *@configuration HWI	Interrupt Number =31 cf doc tm4c123gh6pm.pdf p104			
 */
void AdcConfig(void)
{
	ADCIntDisable(ADC0_BASE,SEQUENCEUR_0);
	ADCIntDisable(ADC0_BASE,SEQUENCEUR_1);
	
	ADCComparatorIntDisable(ADC0_BASE,SEQUENCEUR_1);

	ADCSequenceDisable(ADC0_BASE,SEQUENCEUR_0);
	ADCSequenceDisable(ADC0_BASE,SEQUENCEUR_1);


	ADCComparatorConfigure(ADC0_BASE, COMPARATEUR0, ADC_COMP_INT_HIGH_ONCE);
	ADCComparatorConfigure(ADC0_BASE, COMPARATEUR1, ADC_COMP_INT_HIGH_ONCE);
	ADCComparatorRegionSet( ADC0_BASE, COMPARATEUR0, LOW_LEVEL_BATTERY, FIRST_LEVEL_COMPARATOR );
	ADCComparatorRegionSet( ADC0_BASE, COMPARATEUR1, LOW_LEVEL_BATTERY, SECOND_LEVEL_COMPARATOR );


	ADCHardwareOversampleConfigure(ADC0_BASE, 64);
	ADCSequenceConfigure(ADC0_BASE, SEQUENCEUR_0, ADC_TRIGGER_PROCESSOR, 0); //utiliser pour effectuer des mesures utiliser avec une priorité de 0
	ADCSequenceConfigure(ADC0_BASE, SEQUENCEUR_1, ADC_TRIGGER_PROCESSOR, 1); // utiliser pour les comparateur avec une priorité de 1


	// TODO ajouter ADC_CTL_CMP0 &  ADC_CTL_CMP1 sur la PIN PV Voltage 
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_1, 0, ADC_CTL_CH2 | ADC_CTL_CMP0 );
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_1, 1, ADC_CTL_CH2 | ADC_CTL_CMP1 );


	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_0, 0, ADC_CTL_CH0);  // AIN0
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_0, 1, ADC_CTL_CH0);  // AIN0
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_0, 2, ADC_CTL_CH1);  // AIN1
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_0, 3, ADC_CTL_CH1);  // AIN1
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_0, 4, ADC_CTL_CH1);  // AIN1
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_0, 5, ADC_CTL_CH1);  // AIN1
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_0, 6, ADC_CTL_CH3); //AIN3
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCEUR_0, 7, ADC_CTL_CH2|ADC_CTL_IE|ADC_CTL_END );  // AIN2 Batt Voltage
	                                                                                             
	ADCIntEnable(ADC0_BASE,SEQUENCEUR_1);

	ADCComparatorReset(ADC0_BASE, COMPARATEUR0, true, true);
	ADCComparatorReset(ADC0_BASE, COMPARATEUR1, true, true);

	//ADCComparatorIntEnable( ADC0_BASE, SEQUENCEUR_1);

	ADCSequenceEnable(ADC0_BASE, SEQUENCEUR_0);
	ADCSequenceEnable(ADC0_BASE, SEQUENCEUR_1);
}


/**
 * @brief      Configuration PWM
 */
void PWMConfig(void)
{
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 2000);

	// PWM for Zeta and SEPIC
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 1280);

	// enable PWM
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

}

