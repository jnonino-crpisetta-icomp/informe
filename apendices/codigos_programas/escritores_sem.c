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
#include "semaphore.h"
#include "xtmrctr_l.h"
#include "xtmrctr.h"
#include "xutil.h"

#define valor_maximo 10

/* Threads */
pthread_t hilo_1;
pthread_t hilo_2;
pthread_t hilo_3;
pthread_t hilo_4;
pthread_t hilo_5;

void* master_thread(void *);
void* hilo_escritor_1();
void* hilo_escritor_2();
void* hilo_escritor_3();
void* hilo_escritor_4();
void* hilo_escritor_5();

/* Data */
int cuentas=0;

/* Timer */
XTmrCtr *timer;

/* Semaphores */
sem_t  sem_hilo_uno;
sem_t  sem_hilo_dos;
sem_t  sem_hilo_tres;
sem_t  sem_hilo_cuatro;
sem_t  sem_hilo_cinco;

/* Functions */
int main()
{
    init_platform();
    xil_printf("INICIO EJECUCION CON SEMAFOROS\r\n");
    xil_printf("CINCO HILOS, Valor Maximo = %d \r\n\n",valor_maximo);
    
	/* Initialize xilkernel */
    	xilkernel_init();

    	xmk_add_static_thread(master_thread, 0);

    /* Start the kernel */
       	xilkernel_start();

    /* Never reached */
        cleanup_platform();

    return 0;
}

void* master_thread(void *arg)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int *a;
	int *b;
	int *c;
	int *d;
	int *e;

    //Crear semaforos
	int status;
    status = sem_init(&sem_hilo_uno, 1, 1);
    if(status == 0)
    { /*print("Semaforo creado correctamente\r\n\n");*/	}
    else
    {
    	print("Error al crear el semaforo\r\n\n");
    	pthread_exit(NULL);
    }
   	status = sem_init(&sem_hilo_dos, 1, 0);
    if(status == 0)
    { /*print("Semaforo creado correctamente\r\n\n");*/	}
    else
    {
    	print("Error al crear el semaforo\r\n\n");
    	pthread_exit(NULL);
    }
    status = sem_init(&sem_hilo_tres, 1, 0);
    if(status == 0)
    { /*print("Semaforo creado correctamente\r\n\n");*/	}
    else
    {
    	print("Error al crear el semaforo\r\n\n");
    	pthread_exit(NULL);
    }
    status = sem_init(&sem_hilo_cuatro, 1, 0);
    if(status == 0)
    { /*print("Semaforo creado correctamente\r\n\n");*/	}
    else
    {
    	print("Error al crear el semaforo\r\n\n");
    	pthread_exit(NULL);
    }
    status = sem_init(&sem_hilo_cinco, 1, 0);
    if(status == 0)
    { /*print("Semaforo creado correctamente\r\n\n");*/	}
    else
    {
		print("Error al crear el semaforo\r\n\n");
		pthread_exit(NULL);
    }

    unsigned long int valor_inicial_timer;
    unsigned long int valor_final_timer;
    int ControlStatus;

    //Iniciar cuenta de tiempo
		XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR,0,0x0);
		XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_1_BASEADDR, 0, 0x00000000);
		XTmrCtr_LoadTimerCounterReg	(XPAR_AXI_TIMER_1_BASEADDR,0);
		ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR,0);
		XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR, 0, ControlStatus & (~XTC_CSR_LOAD_MASK));
		valor_inicial_timer = XTmrCtr_GetTimerCounterReg(XPAR_AXI_TIMER_1_BASEADDR,0);
		xil_printf("Valor inicial del timer: %d\r\n\n", valor_inicial_timer);
		XTmrCtr_Enable(XPAR_AXI_TIMER_1_BASEADDR, 0);

    //Lanzamiendo de los hilos escritores
		status = pthread_create(&hilo_1, &attr, (void*)hilo_escritor_1, NULL);
		status = pthread_create(&hilo_2, &attr, (void*)hilo_escritor_2, NULL);
		status = pthread_create(&hilo_3, &attr, (void*)hilo_escritor_3, NULL);
		status = pthread_create(&hilo_4, &attr, (void*)hilo_escritor_4, NULL);
		status = pthread_create(&hilo_5, &attr, (void*)hilo_escritor_5, NULL);

		pthread_join(hilo_1, (void*)&a);
		pthread_join(hilo_2, (void*)&b);
		pthread_join(hilo_3, (void*)&c);
		pthread_join(hilo_4, (void*)&d);
		pthread_join(hilo_5, (void*)&e);

    //Terminar cuenta de tiempo
        valor_final_timer = XTmrCtr_GetTimerCounterReg(XPAR_AXI_TIMER_1_BASEADDR,0);
    	xil_printf("Valor final del timer: %d\r\n\n\n", valor_final_timer);

    return (void*)0;
}

/* HILOS ESCRITORES */
void* hilo_escritor_1()
{
	while(cuentas<valor_maximo)
	{
		//Pedir Semaforo
			sem_wait(&sem_hilo_uno);
			cuentas=cuentas+1;
		//Devolver semaforo
			sem_post(&sem_hilo_dos);
	}
	pthread_exit((void*)cuentas);
	return NULL;
}

void* hilo_escritor_2()
{
	while(cuentas<valor_maximo)
	{
		//Pedir Semaforo
			sem_wait(&sem_hilo_dos);
			cuentas=cuentas+1;
		//Devolver semaforo
			sem_post(&sem_hilo_tres);
	}
	pthread_exit((void*)cuentas);
	return NULL;
}
void* hilo_escritor_3()
{
	while(cuentas<valor_maximo)
	{
		//Pedir Semaforo
			sem_wait(&sem_hilo_tres);
			cuentas=cuentas+1;
		//Devolver semaforo
			sem_post(&sem_hilo_cuatro);
	}
	pthread_exit((void*)cuentas);
	return NULL;
}

void* hilo_escritor_4()
{
	while(cuentas<valor_maximo)
	{
		//Pedir Semaforo
			sem_wait(&sem_hilo_cuatro);
			cuentas=cuentas+1;
		//Devolver semaforo
			sem_post(&sem_hilo_cinco);
	}
	pthread_exit((void*)cuentas);
	return NULL;
}

void* hilo_escritor_5()
{
	while(cuentas<valor_maximo)
	{
		//Pedir Semaforo
			sem_wait(&sem_hilo_cinco);
			cuentas=cuentas+1;
		//Devolver semaforo
			sem_post(&sem_hilo_uno);
	}
		pthread_exit((void*)cuentas);
		return NULL;
}
