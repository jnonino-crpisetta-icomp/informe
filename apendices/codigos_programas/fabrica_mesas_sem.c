/* Includes */
#include "xmk.h"
#include "os_config.h"
#include "sys/ksched.h"
#include "sys/init.h"
#include "config/config_param.h"
#include "stdio.h"
#include "xparameters.h"
#include "platform.h"
#include "platform_config.h"
#include <pthread.h>
#include <sys/types.h>
#include "xutil.h"
#include "xtmrctr_l.h"
#include "semaphore.h"

/* Declarations */
#define cantidad_de_mesas_para_armar	500

#define una_hora 5350
#define dos_horas 10700
#define tres_horas 16060
void ms_sleep(int ticks);
void* master_thread(void *);
void* cortar();
void* lijar();
void* pintar();
void* secar();
void* armar();

/* Data */
sem_t sem_acceso_materia_prima;
int cantidad_materia_prima = cantidad_de_mesas_para_armar;
sem_t sem_acceso_cortadas;
int cantidad_patas_cortadas = 0;
int cantidad_tablas_cortadas = 0;
sem_t sem_acceso_lijadas;
int cantidad_patas_lijadas = 0;
int cantidad_tablas_lijadas = 0;
sem_t sem_acceso_pintadas;
int cantidad_patas_pintadas = 0;
int cantidad_tablas_pintadas = 0;
sem_t sem_acceso_secadas;
int cantidad_patas_secadas = 0;
int cantidad_tablas_secadas = 0;
sem_t sem_acceso_armadas;
int cantidad_mesas_armadas = 0;


/* Functions */
int main()
{
    init_platform();
    
    xil_printf("INICIO DE EJECUCION SEMAFOROS - %d Mesas", cantidad_de_mesas_para_armar);

    /* Initialize xilkernel */
    xilkernel_init();

    /* Create the master thread */
    xmk_add_static_thread(master_thread, 0);
    
    /* Start the kernel */
    xilkernel_start();
    
    /* Never reached */
    cleanup_platform();
    
    return 0;
}

/* The master thread */
void* master_thread(void *arg)
{
	pthread_t hilo_cortador;
	pthread_t hilo_lijador;
	pthread_t hilo_pintor;
	pthread_t hilo_secador;
	pthread_t hilo_armador;
    pthread_attr_t attr;

   	int *mesas_armadas;

    int status;
   	unsigned int valor_inicial_timer;
    int ControlStatus;
   	unsigned int valor_final_timer;

	#if SCHED_TYPE == SCHED_PRIO
    	struct sched_param spar;
	#endif

    pthread_attr_init (&attr);              
                                            
	#if SCHED_TYPE == SCHED_PRIO
    	spar.sched_priority = PRIO_HIGHEST;
    	pthread_attr_setschedparam(&attr, &spar);
	#endif

    /* This is how we can join with a thread and reclaim its return value */
    	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    	status=0;
    	status = status + sem_init(&sem_acceso_cortadas, 1, 1);
    	status = status + sem_init(&sem_acceso_lijadas, 1, 1);
    	status = status + sem_init(&sem_acceso_pintadas, 1, 1);
    	status = status + sem_init(&sem_acceso_secadas, 1, 1);
    	status = status + sem_init(&sem_acceso_armadas, 1, 1);
    	if(status == 0)
    	{	print("\r\nSemaforos creado correctamente\r\n");	}
    	else
    	{
    		print("Error al crear el semaforo\r\n");
    		pthread_exit(NULL);
    	}

    //Iniciar cuenta de tiempo
    	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR,0,0x0);
    	XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_1_BASEADDR, 0, 0x00000000);
       	XTmrCtr_LoadTimerCounterReg	(XPAR_AXI_TIMER_1_BASEADDR,0);
       	ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR,0);
       	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR, 0, ControlStatus & (~XTC_CSR_LOAD_MASK));
       	valor_inicial_timer = XTmrCtr_GetTimerCounterReg(XPAR_AXI_TIMER_1_BASEADDR,0);
       	xil_printf("\r\n\nValor inicial del timer: %d\r\n\n", valor_inicial_timer);
       	XTmrCtr_Enable(XPAR_AXI_TIMER_1_BASEADDR, 0);

    status = pthread_create(&hilo_cortador, &attr, (void*)cortar, NULL);
	//	if (status != 0)	{  	xil_printf ("ERROR LANZANDO HILO CORTADOR");	}
    status = pthread_create(&hilo_lijador, &attr, (void*)lijar, NULL);
	//	if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO LIJADOR");	}
    status = pthread_create(&hilo_pintor, &attr, (void*)pintar, NULL);
	//	if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO PINTOR");	}
    status = pthread_create(&hilo_secador, &attr, (void*)secar, NULL);
	//	if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO SECADOR");	}
    status = pthread_create(&hilo_armador, &attr, (void*)armar, NULL);
	// 	if (status != 0)	{ 	xil_printf ("ERROR LANZANDO HILO ARMADOR");	}

	pthread_join(hilo_armador,(void*)&mesas_armadas);

    //Terminar cuenta de tiempo
       	valor_final_timer = XTmrCtr_GetTimerCounterReg(XPAR_AXI_TIMER_1_BASEADDR,0);
       	xil_printf("Valor final del timer: %d\r\n\n", valor_final_timer);

    xil_printf("SE ARMARON %d MESAS\r\n\n",cantidad_mesas_armadas);

    return (void*)0;
}

void* cortar()
{
	int i = 0;
	while(cantidad_materia_prima>0)
	{
		//Extraer materia prima
			sem_wait(&sem_acceso_materia_prima);
			cantidad_materia_prima = cantidad_materia_prima - 1;
			sem_post(&sem_acceso_materia_prima);

		//Cortar
		    ms_sleep(dos_horas);

		//Poner elementos en materiales cortados
			while (cantidad_patas_cortadas >= 8 && cantidad_tablas_cortadas >= 2)
			{
				//Esperar para no superar la cota de plazas
				yield();
			}
			sem_wait(&sem_acceso_cortadas);
			cantidad_patas_cortadas = cantidad_patas_cortadas + 4;
			cantidad_tablas_cortadas = cantidad_tablas_cortadas + 1;
			sem_post(&sem_acceso_cortadas);
	}//Fin del while
	pthread_exit((void*)i);
	return NULL;
}


void* lijar()
{
	int i = 0;
	while(cantidad_mesas_armadas < cantidad_de_mesas_para_armar)
	{
		//Extraer elementos cortados
			while (cantidad_patas_cortadas < 4 && cantidad_tablas_cortadas < 1)
			{
				//Espera hasta que haya elementos para lijar
				yield();
			}
			sem_wait(&sem_acceso_cortadas);
			cantidad_patas_cortadas = cantidad_patas_cortadas - 4;
			cantidad_tablas_cortadas = cantidad_tablas_cortadas - 1;
			sem_post(&sem_acceso_cortadas);

		//Lijar
			ms_sleep(tres_horas);

		//Poner elementos en materiales lijados
			while (cantidad_patas_lijadas != 0 && cantidad_tablas_lijadas != 0)
			{
				//Esperar para no superar la cota de plazas
				yield();
			}
			sem_wait(&sem_acceso_lijadas);
			cantidad_patas_lijadas = cantidad_patas_lijadas + 4;
			cantidad_tablas_lijadas = cantidad_tablas_lijadas + 1;
			sem_post(&sem_acceso_lijadas);
	}//Fin del while
	pthread_exit((void*)i);
	return NULL;
}

void* pintar()
{
	int i = 0;
	while (cantidad_mesas_armadas < cantidad_de_mesas_para_armar)
	{
		//Extraer elementos lijados
			while (cantidad_patas_lijadas < 4 && cantidad_tablas_cortadas < 1)
			{
				//Espera hasta que haya elementos para pintar
				yield();
			}
			sem_wait(&sem_acceso_lijadas);
			cantidad_patas_lijadas = cantidad_patas_lijadas - 4;
			cantidad_tablas_lijadas = cantidad_tablas_lijadas - 1;
			sem_post(&sem_acceso_lijadas);

		//Pintar
			ms_sleep(una_hora);

		//Poner elementos en materiales pintados
			while (cantidad_patas_pintadas != 0 && cantidad_tablas_pintadas != 0)
			{
				//Esperar para no superar la cota de plazas
				yield();
			}
			sem_wait(&sem_acceso_pintadas);
			cantidad_patas_pintadas = cantidad_patas_pintadas + 4;
			cantidad_tablas_pintadas = cantidad_tablas_pintadas + 1;
			sem_post(&sem_acceso_pintadas);
	}//Fin del while
	pthread_exit((void*)i);
	return NULL;
}

void* secar()
{
	int i = 0;
	while(cantidad_mesas_armadas < cantidad_de_mesas_para_armar)
	{
		//Extraer elementos pintados
			while (cantidad_patas_pintadas < 4 && cantidad_tablas_pintadas < 1)
			{
				//Espera hasta que haya elementos para secar
				yield();
			}
			sem_wait(&sem_acceso_pintadas);
			cantidad_patas_pintadas = cantidad_patas_pintadas - 4;
			cantidad_tablas_pintadas = cantidad_tablas_pintadas - 1;
			sem_post(&sem_acceso_pintadas);

		//Armar
			ms_sleep(dos_horas);

		//Poner elementos en mesas secadas
			while (cantidad_patas_secadas >= 8 && cantidad_tablas_secadas >=2)
			{yield();}
			sem_wait(&sem_acceso_secadas);
			cantidad_patas_secadas = cantidad_patas_secadas + 4;
			cantidad_tablas_secadas = cantidad_tablas_secadas + 1;
			sem_post(&sem_acceso_secadas);
	}//Fin del while
	pthread_exit((void*)i);
	return NULL;
}

void* armar()
{
	int i = 0;
	while(cantidad_mesas_armadas < cantidad_de_mesas_para_armar)
	{
		//Extraer elementos secados
			while (cantidad_patas_secadas < 4 && cantidad_tablas_secadas < 1)
			{
				//Espera hasta que haya elementos para armar
				yield();
			}
			sem_wait(&sem_acceso_secadas);
			cantidad_patas_secadas = cantidad_patas_secadas - 4;
			cantidad_tablas_secadas = cantidad_tablas_secadas - 1;
			sem_post(&sem_acceso_secadas);

		//Armar
			ms_sleep(una_hora);

		//Poner elementos en mesas armadas
			if (cantidad_mesas_armadas < cantidad_de_mesas_para_armar)
			{
				sem_wait(&sem_acceso_armadas);
				cantidad_mesas_armadas = cantidad_mesas_armadas + 1;
				sem_post(&sem_acceso_armadas);
			}
	}//Fin del while
	pthread_exit((void*)i);
	return NULL;
}
void ms_sleep(int ticks
{
	int count = 0;
	for(count = 0 ; count < ticks ; count++)
	{	}
}
