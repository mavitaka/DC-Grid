#include "Task_RTOS.h"

uint32_t InterruptStatus;


int GetSigne(float value)
{
    if(value>0)
        return 1;
    else
        return -1;
}

/**
 * @brief      ISR déclancher lors de la réception de 8 octets de la carte ATMEL, ces données viennet d'un autre noeud CPL
 * 			   on appelle le SWI UART_CPL qui récupère les données, donne le sémaphore à la tâche TaskCplCom 
 */
void UART_CPL_ISR_HWI(void)
{
	UARTIntClear(UART5_BASE, UARTIntStatus(UART5_BASE, true));
	Swi_post(ControlCplCom_SWI_Handle);

}

/**
 * @brief      ISR déclencher lorsque Vbattery == 1 er  niveau Tension Battery chargé  ou Vbattery== 2 eme niveau Tension Battery chargé
 * 				Clear flag, lancement de la SWI pour le controlFlow en mettant à jour la variable Mode_Actuel; 
 */
void ADC_COMP_ISR_HWI(void)
{
   InterruptStatus=ADCComparatorIntStatus( ADC0_BASE );
   ADCComparatorIntClear(ADC0_BASE, InterruptStatus);
   ADCIntClear(ADC0_BASE, SEQUENCEUR_1);
   Swi_post(ADC_COMP_SWI_Handle);	
}



/**
 * @brief      SWI Software interrupt permettant de gerer le type de requete recu par la carte CPL
 */
void ControlCplCom_SWI(void)
{
	uint8_t i=0;
	uint8_t trameISR[8];
	uint8_t TabTemp[4];
	float temp;


	while(UARTCharsAvail(UART5_BASE))
        {
            trameISR[i]=UARTCharGet(UART5_BASE);
            i++;
        }

    #if DEBUG_SERIE
        for(i=0;i<8;i++)
               System_printf("%x \n",trameISR[i]);
    #endif 


   	if(trameISR[3]==RESPONSE && trameISR[2] != CMD_STATE_GIVE && trameISR[2] !=CMD_STATE_DAY_NIGHT )
	{
		// REPONSE ??
		#if DEBUG_SERIE
			System_printf("reponse \n");
		#endif
		temp=*(float *)(trameISR+4);
		Mailbox_post(MailBoxDataOtherGrid, &temp, BIOS_NO_WAIT);
	}
	else
	{

		switch(trameISR[2])
	{
		case CMD_PV_VOLTAGE :
			
				#if DEBUG_SERIE
				System_printf("PV voltage envoi \n");
				#endif
				ComAtmelCPL(CMD_PV_VOLTAGE,RESPONSE,Vpv_new.ValeurByte);

		break;

		case CMD_PV_CURRENT:
				#if DEBUG_SERIE
				System_printf("PV courant envoi \n");
				#endif
				ComAtmelCPL(CMD_PV_VOLTAGE,RESPONSE,I_pv.ValeurByte);
		break;

		case CMD_BATTERY_VOLTAGE:
				#if DEBUG_SERIE
				System_printf("Batterie voltage  envoi \n");
				#endif
				ComAtmelCPL(CMD_PV_VOLTAGE,RESPONSE,V_bat.ValeurByte);

		break;

		case CMD_BATTERY_CURRENT:
				#if DEBUG_SERIE
				System_printf("Batterie courant envoi \n");
				#endif
				ComAtmelCPL(CMD_PV_VOLTAGE,RESPONSE,I_bat.ValeurByte);
		break;

		case CMD_LED_CURRENT:
				#if DEBUG_SERIE
				System_printf("LED courant  envoi \n");
				#endif
				ComAtmelCPL(CMD_PV_VOLTAGE,RESPONSE,I_led.ValeurByte);
		break;

		case CMD_LED_VOLTAGE:
				#if DEBUG_SERIE
				System_printf("LED voltage envoi \n");
				#endif
				ComAtmelCPL(CMD_PV_VOLTAGE,RESPONSE,V_led.ValeurByte);
		break;

		case CMD_STATE_GIVE:
		if(trameISR[3]==RESPONSE)
			{
				
				#if DEBUG_SERIE
				System_printf("Etat autre grid recu\n");
				#endif
				BatterieOtherGridState=trameISR[4];
				Semaphore_post(SemaphoreBattAutreGrid);


			}
			else
			{
				#if DEBUG_SERIE
				System_printf("envoi etat grid \n");
				#endif

				// un autre noeud cherche à savoir si notre batterie est pleine ou non
				// si elle est pleine on ne reçoit pas d'énergie 
				// si elle n'est pas pleine on recoit de l'énergie 
				TabTemp[0]=Mode_Actuel;
				ComAtmelCPL(CMD_PV_VOLTAGE,RESPONSE,TabTemp);

			}
		break;

		case CMD_STATE_DAY_NIGHT:
			if(trameISR[3]==RESPONSE)
			{
				#if DEBUG_SERIE
				System_printf("changement d'état recu \n");
				#endif
				//
				//On recoit ici l'ordre de changer de mode de passer en nuit==>jour ou jour==>nuit
				//on averti la tache switch mode jour nuit pour lui dire de proceder au changement
				//on verifie que l 'etat actuel est different de celui recu pour switcher
				//if(ModeJourNuit!=trameISR[4])
				//{
					ModeJourNuit=trameISR[4];
			    	Semaphore_post(SemaphoreJourNuit);	
				//}
				//else
				//{
					// on ne fait rien si l'état actuel est le même 
				//}

			}
			else
			{
				#if DEBUG_SERIE
				System_printf("on envoi le mode de fonctionnement \n");
				#endif
				// on envoit l'état actuel de la variable contenant jour/nuit
				TabTemp[0]=ModeJourNuit;
				ComAtmelCPL(CMD_STATE_DAY_NIGHT,RESPONSE,TabTemp);

			}
		break; 

		default:
		#if DEBUG_SERIE
			System_printf("Commande incomprise \n");
		#endif
		break;
	}

	}
	
}

/**
 * @brief      Envoi une trame d'information en UART à la carte ATMEL pour une communication CPL  
 * 	 			Format Trame : [COMMANDE][REQUETE/REPONSE][VALEUR 4 Octets]
 *
 * @param[in]  commande  Type de demande effectué CMD_xxxxx cf ConstanteCom.h 
 * @param[in]  Req_Rep   Requete = demande REPONSE = réponse cf ConstanteCom.h
 * @param      val       pointeur de donnée sur les valeurs à transmettre ou vide dans le cas d'une requête 
 */
void ComAtmelCPL(uint8_t commande, uint8_t Req_Rep,uint8_t* val)
{
    uint8_t trameAtmel[6];
    int i;

    trameAtmel[0]=commande;
    trameAtmel[1]=Req_Rep;
    trameAtmel[2]=val[0];
    trameAtmel[3]=val[1];
    trameAtmel[4]=val[2];
    trameAtmel[5]=val[3];

    for(i=0;i<6;i++)
    	{
            UARTCharPut(UART5_BASE,  trameAtmel[i]);
        }
}


/**
 * @brief      Calcul le nouveau rapport Cyclique
 *
 * @param[in]  duty_cycle  The duty cycle
 * @param[in]  Q           
 *
 * @return     retourne le nouveau rapport Cyclique 
 */
uint16_t Voltage_based(uint16_t duty_cycle, float Q)
{
  float increment;
  float new_duty_cycle;
  int signe=GetSigne(Q);

  // calculate increment
  //TODO
  //changer les valeurs de Q
  if (-Q_ALPHA<Q<Q_ALPHA)
	  increment = 0;
  else
	  if (-Q_BETA<Q<Q_BETA)
		  increment =signe*0.5;
	  else
		  increment = signe*1;
  //increment = 0;
  new_duty_cycle = duty_cycle + increment;

  // saturation condition
  if(new_duty_cycle >= 800)
      return 800;
  else if (new_duty_cycle <= 300)
      return 300;
  else
      return (uint16_t)new_duty_cycle;
}


/**
 * @brief      Gerer le processus de déroulement du systeme switch Tache en foncions des niveaux de tensions atteins 
 * 			     
 * 
 */
void ControlFlowSystem_SWI(void)
{
	  
	                                                      
	switch(Mode_Actuel)
	{
    	case BATTERY_CHARGING :

    		#if DEBUG_SERIE
					System_printf("BATTERY_CHARGING\n");
			#endif

			PostAlarm(ALARME_BATTERY_CHARGING);
			PostAlarm(ALARME_RELAY_CLOSE);

    		mode_change(BATTERY_CHARGING); 
    		Hwi_enableInterrupt(ADC_COMP_INTERRUPT_NUMBER);
    		Task_setPri(TaskMPPT_Handle,TaskMPPT_PRIORITY);
    		Task_setPri(TaskLedDriving_Handle,BLOCKED);
    		Task_setPri(TaskCurrentRegulation_Handle,BLOCKED);
    		Timer_start(TimerMesure);
    		GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_PIN_2); // on ferme le relais
    	break; 

		case BATTERY_VOLTAGE_FIRST_LEVEL:
			#if DEBUG_SERIE
					System_printf("BATTERY_VOLTAGE_FIRST_LEVEL\n");
			#endif
			// Stop Tâche MPPT
			// Run Tache Regulation courant 
			Task_setPri(TaskMPPT_Handle,BLOCKED);
			Task_setPri(TaskCurrentRegulation_Handle,TaskCurrentRegulation_PRIORITY);  
    	break; 

		case BATTERY_FULL :
			// Stop Tâche Regulation courant
			// Run tache BatteryFull
			
			#if DEBUG_SERIE
				System_printf("BATTERY_FULL\n");
			#endif
			PostAlarm(ALARME_BATTERY_FULL);
			Hwi_disableInterrupt(ADC_COMP_INTERRUPT_NUMBER);
			Task_setPri(TaskCurrentRegulation_Handle,BLOCKED);
			Task_setPri(TaskBatteryFull_Handle,TaskBatteryFull_PRIORITY);
    		mode_change(IDLE);
		break;

		case POWER_MANAGEMENT:
			
			#if DEBUG_SERIE
					System_printf("Power management\n");
			#endif
			Task_setPri(TaskLedDriving_Handle,BLOCKED);
			Timer_start(TimerMesure);
			Task_setPri(TaskPowerManagement_Handle,TaskPowerManagement_PRIORITY);
		break; 

		case LED_DRIVING_DIMMING:

			#if DEBUG_SERIE
					System_printf("LED_DRIVING\n");
			#endif
		            Timer_start(TimerMesure);

			PostAlarm(ALARME_LED_DRIVING);
			GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,0x00);// on ouvre le relais
			mode_change(LED_DRIVING_DIMMING);
			Task_setPri(TaskLedDriving_Handle,TaskLedDriving_PRIORITY);
		break; 

	}

}





/**
 * @brief      ADC SWI permettant de mettre à jour la variable Mode_Actuel  en fonction des niveaux de tensions atteins
 * 			   après mis à jour de la variable Mode_Actuel on appelle ControlFlowSystem qui en fonction du trigger
 * 			   va bloqué ou mettre en run certaine taches
 */
void ADC_COMP_SWI(void)
{

	if(InterruptStatus==BATTERY_VOLTAGE_FIRST_LEVEL_TRiGGER)
        Mode_Actuel=BATTERY_VOLTAGE_FIRST_LEVEL;
    else
        if(InterruptStatus==BATTERY_VOLTAGE_SECOND_LEVEL_TRiGGER)
        Mode_Actuel=BATTERY_FULL;
	
	Swi_post(ControlFlowSystem_Handle);

}


/**
 * @brief      SWI appellé toutes les X ms lorsque le Software Timer arrive à son Timeout 
 * 			   on va ici réaliser les mesures et rendres les différents sémpahore
 */
	void PeriodicMesure_SWI(void)
{

	ADCProcessorTrigger(ADC0_BASE, SEQUENCEUR_0);
	ADCProcessorTrigger(ADC0_BASE, SEQUENCEUR_1);
	while(!ADCIntStatus(ADC0_BASE, SEQUENCEUR_0, false));
	ADCIntClear(ADC0_BASE, SEQUENCEUR_0);
	ADCSequenceDataGet(ADC0_BASE, SEQUENCEUR_0, ADC0Value);
	
	switch(ModeJourNuit)
	{
		case DAY_FUNCTIONNEMENT:
			//V_pv.ValeurFloat = ((float)ADC0Value[0]+(float)ADC0Value[1])*1.65*VOLTAGE_GAIN/4095;  // pv volage
			Vpv_new.ValeurFloat = ((float)ADC0Value[0]+(float)ADC0Value[1])*1.65*VOLTAGE_GAIN/4095;  // pv volage
			I_pv.ValeurFloat = ((float)ADC0Value[2] + (float)ADC0Value[3] + (float)ADC0Value[4] + (float)ADC0Value[5])*8.25/4095 - 25.4;    // pv current
		break;

		case NIGHT_FUNCTIONNEMENT:
			V_led.ValeurFloat = ((float)ADC0Value[0]+(float)ADC0Value[1])*1.65*VOLTAGE_GAIN/4095;
			I_led.ValeurFloat = 25.3 - ((float)ADC0Value[2] + (float)ADC0Value[3] + (float)ADC0Value[4] + (float)ADC0Value[5])*8.25/4095;
			P_led.ValeurFloat=V_led.ValeurFloat * I_led.ValeurFloat;
		break;

		default:
		break; 

	}

	V_bat.ValeurFloat = ((float)ADC0Value[7])*3.3*VOLTAGE_GAIN/4095;  // battery voltage
	I_bat.ValeurFloat = ((float)ADC0Value[6])*33/4095 - 25.5;    // battery current
	Semaphore_post(SemaphoreMesure);  

}

/**
 * @brief      configure PWM en fonction du mode de fonctionnement
 *
 * @param[in]  mode  mode de fonctionnement
 */
void mode_change(uint8_t mode)
{
  switch(mode)
  {
    case IDLE:
      PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
      PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
      PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, false);
      break;
    case BATTERY_CHARGING:
      PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
      PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
      PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, false);
      break;
    case LED_DRIVING_DIMMING:
      PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
      PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
      PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
      break;
  }
}


void SupervisionISR(void)
{
    int i=100,j;
    temp.ValeurFloat=12.3;

    for(j=0;j<11;j++)
    {
        TransmitDataSupervision((uint8_t*)AddrVar[j], i);
        i+=10;
    }

}
void InitSoftware(void)
{


    AddrVar[0]=V_bat.ValeurByte;
    AddrVar[1]=I_bat.ValeurByte;
    AddrVar[2]=P_out.ValeurByte;
    AddrVar[3]=Vpv_old.ValeurByte;
    AddrVar[4]=I_pv.ValeurByte;
    AddrVar[5]=P.ValeurByte;

    AddrVar[7]=V_led.ValeurByte;
    AddrVar[8]=I_led.ValeurByte;
    AddrVar[9]=P_led.ValeurByte;
   // AddrVar[10]=&D_sepic;
    //AddrVar[6]=&Dzeta_old;


}
/**
 * @brief      initialise l'ensemble des variables nécessaire au fonctionnement des Process jour/nuit
 *
 * @param[in]  Day_Night  The day night
 */
void InitFonctionnement(uint8_t mode_init)
{
	if(mode_init==DAY_FUNCTIONNEMENT)
	{
		Q=0;
		Dzeta_new=640;
		Dzeta_old=640;
	}
	else
		if(mode_init==NIGHT_FUNCTIONNEMENT)
		{
			D_sepic=640;
			D_dimming=1000;
		}


}


/**
 * @brief      Envoi les nbData octets de data, à l'adresse addr de la
 *             supervision
 *          	[]=1 octet
 *             formart de  la trame [0x1b][0x57][ADDR High][ADDR Low][Nb Octet High][Nb Octet Low][Octet Data ][Octet Data ][Octet Data][Octet Data]
 *
 * @param[in]  nbData  le numbre d'octet à envoyer
 * @param      data    données à envoyer
 * @param[in]  addr    addresse des octets à envoyer cf ConstanteCom.h  ADDR_SUPERVISION_
 */
void  TransmitDataSupervision(uint8_t * data,uint8_t addr)
{
	uint8_t TrameSupervision[10];
	uint8_t SizeFrame,i;

	TrameSupervision[0]=ENTETE_SUPERVISION;
	TrameSupervision[1]=WRITE_SUPERVISION;
	TrameSupervision[2]=0x00;
	TrameSupervision[3]=addr;
	TrameSupervision[4]=0x00;
	
	if((addr != ADDR_SUPERVISION_DC_PV) && (addr != ADDR_SUPERVISION_DC_LED) && (addr != ADDR_SUPERVISION_JOUR_NUIT))
	{
		SizeFrame=10;
		TrameSupervision[5]=4;
		TrameSupervision[6]=data[3];
		TrameSupervision[7]=data[2];
		TrameSupervision[8]=data[1];
		TrameSupervision[9]=data[0];				
	}
	else
	{
		SizeFrame=8;
		TrameSupervision[5]=1;
		TrameSupervision[6]=0x00;
		TrameSupervision[7]=*data;
		
	}

	#if DEBUG_SERIE
	#endif
	
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_0);

	for(i=0;i<SizeFrame;i++)
		 UARTCharPut(UART4_BASE,  TrameSupervision[i]);

}

/**
 * @brief      Permet de poster une alarme dans la mailbox qui va être ensuite par IdleTaskAlarm
 *
 * @param[in]  TypeAlarme  le type d'alarme à envoyer défini dans ConstanteCom.h
 */
void PostAlarm(uint8_t TypeAlarme)
{
	//TODO vérifier le bon fonctionnement  ??
	//
	Mailbox_post(MailBoxSupervisionAlarm, &TypeAlarme, BIOS_NO_WAIT);

}

/**
 * @brief      Envoi d'alarme défini dans TaskRTOS.h (BATTERY_CHARGING, etc..)
 *
 * @param[in]  type  type d'alarme à déclenchée 
 */
void TransmitAlarmSupervision(uint8_t type)
{
	uint8_t TrameAlarme[8];
	uint8_t i;
	
	TrameAlarme[0]=ENTETE_SUPERVISION;
	TrameAlarme[1]=WRITE_SUPERVISION;
	TrameAlarme[2]=0x00;
	TrameAlarme[3]=type;
	TrameAlarme[4]=0x00;
	TrameAlarme[5]=0x01;
	TrameAlarme[6]=0x00;
	TrameAlarme[7]=0x01;
	

	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_0);

	for(i=0;i<FRAME_DATA_LENGTH;i++)
		UARTCharPut(UART4_BASE,  TrameAlarme[i]);
}

/**
 * @brief      Tâche permettant de switcher le mode jour/nuit
 */
void TaskDayNight(void)
{

	while(1)
	{

		Semaphore_pend(SemaphoreJourNuit, BIOS_WAIT_FOREVER); // attente d'une notification de

		 // notification au gestionnaire d'alarme

		switch(ModeJourNuit)
		{
			case DAY_FUNCTIONNEMENT:
			
				#if DEBUG_SERIE
					System_printf("DAY_FUNCTIONNEMENT\n");
				#endif

				mode_change(IDLE);
				Mode_Actuel=BATTERY_CHARGING;
				InitFonctionnement(DAY_FUNCTIONNEMENT);
				Swi_post(ControlFlowSystem_Handle);
			break;

			case NIGHT_FUNCTIONNEMENT:

				#if DEBUG_SERIE
					System_printf("NIGHT_FUNCTIONNEMENT\n");
				#endif
			
				mode_change(IDLE);
				Mode_Actuel=LED_DRIVING_DIMMING;
				InitFonctionnement(NIGHT_FUNCTIONNEMENT);
				Swi_post(ControlFlowSystem_Handle);
			break;

			default:
			//TODO
			// ni mode jour ni mode nuit  en défaut envoi d'une alarme à l IHM
				#if DEBUG_SERIE
				System_printf("Pb Jour/nuit\n");
				System_printf("reboot\n");
				#endif

				PostAlarm(ALARM_ERROR_CRITICAL_SYSTEM);	

				//Reboot le systeme
				//HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
			break;
		}
	}
}

/**
 * @brief      Tâche executer lorsque la batterie est pleine 
 *			   demande l'etat de la Batterie autre µGrid 
 *			   Si  Batterie autre grid ==  BATTERY_FULL Alimente la LED (ouvre le relais)
 *			   Si Batterie Autre grid ==   BATTERY_CHARGING (on ne fait rien on attend que l autre batterie soit pleine pour ouvrir le relais)
 */
void TaskBatteryFull(void)
{
	uint8_t TrameReq[4];
	
	#if DEBUG_SERIE
				System_printf("BATTERY_FULL\n");
	#endif

	while(1)
	{
		ComAtmelCPL(CMD_STATE_GIVE, REQUEST, TrameReq);
		
		Semaphore_pend(SemaphoreBattAutreGrid, BIOS_WAIT_FOREVER);
		// on stocke la valeur dans Mode_Batt
	 	if(BatterieOtherGridState==BATTERY_FULL)
	 	{
	 		#if DEBUG_SERIE
				System_printf(" 2 BATTERY_FULL\n");
			#endif
	 		// on ouvre le relais et on quitte la tache
	 		// on lance la tâche qui controle la LED
	 		PostAlarm(ALARME_RELAY_OPEN);
	 		GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_PIN_2);
	 		Task_setPri(TaskBatteryFull_Handle,BLOCKED);
	 	}
	 	else
	 	{
	 		if(BatterieOtherGridState==BATTERY_CHARGING)
	 		{
	 			#if DEBUG_SERIE
				System_printf("Charging other grid\n");
				#endif
				PostAlarm(ALARME_CHARGING_OTHER_GRID);
	 			// on endort la tâche pendant xx S puis on effectue la requete 
	 			Task_sleep(1000000); // Task_sleep(xx) xx à définir  
	 		}
	 	}
	}

}


/**
 * @brief      Task Gère le chargement de la batterie par l'algorithme MPPT
 */
void TaskMPPT(void)
{
	static uint16_t count=1;
System_printf("MPPT \n");
	while(1)
	{

	Semaphore_pend(SemaphoreMesure, BIOS_WAIT_FOREVER); // attente des mesures (sémaphore donné par PeriodicMesure_SWI)
	                                                    
	//V_pv.ValeurFloat = ((float)ADC0Value[0]+(float)ADC0Value[1])*1.65*VOLTAGE_GAIN/4095;  // pv volage
	//Vpv_new.ValeurFloat = ((float)ADC0Value[0]+(float)ADC0Value[1])*1.65*VOLTAGE_GAIN/4095;  // pv volage
	//I_pv.ValeurFloat = ((float)ADC0Value[2] + (float)ADC0Value[3] + (float)ADC0Value[4] + (float)ADC0Value[5])*8.25/4095 - 25.4;    // pv current
	//V_bat.ValeurFloat = ((float)ADC0Value[7])*3.3*VOLTAGE_GAIN/4095;  // battery voltage
	//I_bat.ValeurFloat = ((float)ADC0Value[6])*33/4095 - 25.4;    // battery current
		                                                  
	if(count==1)
	{
		Dzeta_new = 655; // duty cycle 0.45
		Dzeta_old = 640;
		Vpv_old.ValeurFloat = Vpv_new.ValeurFloat;

	}
	else
	{
		P_out.ValeurFloat = V_bat.ValeurFloat*I_bat.ValeurFloat;
		Q = ((float)Dzeta_new/1280)*(1-((float)Dzeta_new/1280))*(Vpv_new.ValeurFloat-Vpv_old.ValeurFloat)+Vpv_new.ValeurFloat*(((float)Dzeta_new/1280)-((float)Dzeta_old/1280));
		Vpv_old.ValeurFloat = Vpv_new.ValeurFloat;
		Dzeta_old = Dzeta_new;
		Dzeta_new = Voltage_based(Dzeta_new,Q);
		P.ValeurFloat=V_pv.ValeurFloat*I_pv.ValeurFloat;

	}

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, Dzeta_old);
	count++;
	
	if(count==200)
	{
		// à partir de la valeur n de count on peut décider d'effectuer des opérations tel que l'envoi de valeurs sur l'IHM
		count=2;	
	}
	
	}
}

/**
 * @brief      Tâche permettant de controler la LED en mode nuit
 */
void TaskLedDriving(void)
{
	static uint16_t count=1;

	while(1)
	{
		Semaphore_pend(SemaphoreMesure, BIOS_WAIT_FOREVER);

		//V_led.ValeurFloat = ((float)ADC0Value[0]+(float)ADC0Value[1])*1.65*VOLTAGE_GAIN/4095;
		//I_led.ValeurFloat = 25.3 - ((float)ADC0Value[2] + (float)ADC0Value[3] + (float)ADC0Value[4] + (float)ADC0Value[5])*8.25/4095;
		//V_bat.ValeurFloat = ((float)ADC0Value[7])*3.3*VOLTAGE_GAIN/4095;  // battery voltage
		//I_bat.ValeurFloat = 25.3 - ((float)ADC0Value[6])*33/4095;    // battery current
		                                                 
		if(count == 1)
		{
			D_sepic = 640;
			D_dimming = 50;
			
		}
		else
		{

			if(V_led.ValeurFloat < 16)
				D_sepic = D_sepic + 10;
			else if (V_led.ValeurFloat < 18)
				D_sepic = D_sepic + 5;
			else if(V_led.ValeurFloat < 20)
				D_sepic = D_sepic + 2;
			else if(V_led.ValeurFloat > 22)
				D_sepic = D_sepic - 2;
			else if(V_led.ValeurFloat > 24)
				D_sepic = D_sepic - 5;

			if(D_sepic > 800)
				D_sepic=800;
			else if(D_sepic < 200)
				D_sepic=200;


		}

		//TODO utiliser un capteur pour savoir si il y a quelqun ou non 
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, D_sepic);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, D_dimming);

		count++;
		if(count==200)
	{
		// à partir de la valeur n de count on peut décider d'effectuer des opérations tel que l'envoi de valeurs sur l'IHM
		count=2;	
	}
	

	}


}


/**
 * @brief      Tâche permettant de reguler le courant lorsque la tension de la batterie est situé entre le 1er Niveau et le 2eme niveau de Tension
 */
void TaskCurrentRegulation(void)
{
	uint8_t IncDecDzeta=20;


	while(1)
	{
		Semaphore_pend(SemaphoreMesure, BIOS_WAIT_FOREVER);

		//I_bat.ValeurFloat = 25.3 - ((float)ADC0Value[6])*33/4095;

		if(I_bat.ValeurFloat>ALPHA_CURRENT)
			//decrease duty cycle Dzeta_old
			Dzeta_old-=IncDecDzeta;
		else
			if(I_bat.ValeurFloat<BETA_CURRENT)
				//increase duty cycle Dzeta_old
				Dzeta_old+=IncDecDzeta;

		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, Dzeta_old);
	}

}



/**
 * @brief      Tâche Idle permettant d'envoyer des alarme à la supervision
 * 				attente des données présentes dans la  MailBoxSupervisionAlarm et envoi les données directement  
 */
void TaskAlarm(void)
{
	uint8_t Alarme;

	while(1)
	{
		Mailbox_pend(MailBoxSupervisionAlarm, &Alarme, BIOS_WAIT_FOREVER);
		TransmitAlarmSupervision(Alarme);	
	}

}


/**
 * @brief      Tâche permettant de mettre en oeuvre le fonctionnement mode nuit 
 * 				en fonction des caractéristique de la batterie 
 *
 */
void TaskPowerManagement(void)
{
	uint8_t TrameData[4];
	float BatteryVoltageOtherGrid,IbattOtherGrid,PbattOtherGrid;

	Semaphore_pend(SemaphoreMesure, BIOS_WAIT_FOREVER);

	V_bat.ValeurFloat = ((float)ADC0Value[7])*3.3*VOLTAGE_GAIN/4095;

	if(V_bat.ValeurFloat>=BATTERY_VOLTAGE_MIN)
	{
		Mode_Actuel=LED_DRIVING_DIMMING;
		Swi_post(ControlFlowSystem_Handle);
		Task_setPri(TaskPowerManagement_Handle,BLOCKED);

	}
	else
	{

		ComAtmelCPL(CMD_BATTERY_VOLTAGE,REQUEST,TrameData);
		Mailbox_pend(MailBoxDataOtherGrid, &BatteryVoltageOtherGrid, BIOS_WAIT_FOREVER);
		ComAtmelCPL(CMD_BATTERY_CURRENT,REQUEST,TrameData);
		Mailbox_pend(MailBoxDataOtherGrid, &IbattOtherGrid, BIOS_WAIT_FOREVER);
		
		PbattOtherGrid=IbattOtherGrid*BatteryVoltageOtherGrid;

		if(PbattOtherGrid<=BATTERY_POWER_MIN)
		{

			// Switch EDF
			// Alert
		}
		else
		{
			// la puissance vient du deuxieme grid 
			// Alert
			
		}

		Mode_Actuel=LED_DRIVING_DIMMING;
		Swi_post(ControlFlowSystem_Handle);
		Task_setPri(TaskPowerManagement_Handle,BLOCKED);

	}


}
