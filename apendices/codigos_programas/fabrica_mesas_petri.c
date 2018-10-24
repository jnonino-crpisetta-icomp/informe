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
#include "sys/intr.h"
#include "semaphore.h"
#include "xtmrctr_l.h"
#include "sys/timer.h"
/* Declarations */
#define cantidad_de_mesas_para_armar	5

// PARA 5 MESAS
	int marcado_inicial_p_cero	= 0x00000005;
	int cota_plaza_p_cero 		= 0x00000006;
	int cota_plaza_p_nueve 		= 0x09000006;

// PARA 50 MESAS
//	int marcado_inicial_p_cero 	= 0x00000032;
//	int cota_plaza_p_cero 		= 0x00000033;
//	int cota_plaza_p_nueve 		= 0x09000033;

// PARA 500 MESAS
//	int marcado_inicial_p_cero 	= 0x000001F4;
//	int cota_plaza_p_cero 		= 0x000001F5;
//	int cota_plaza_p_nueve 		= 0x090001F5;

#define disparo_t_cero		0x00000000
#define disparo_t_uno 		0x00010000
#define disparo_t_dos 		0x00020000
#define disparo_t_tres 		0x00030000
#define disparo_t_cuatro	0x00040000

//	Direcciones para escribir en Petri
		Xuint32 *m_marcado_addr                         = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR);
		Xuint32 *m_incidencia_positiva_addr             = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x4);
		Xuint32 *m_incidencia_negativa_addr             = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x8);
		Xuint32 *m_inhibicion_addr                      = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0xC);
		Xuint32 *p_cotas_addr                           = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x10);
		Xuint32 *t_automatica_addr                      = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x14);
		Xuint32 *load_vector_duracion_addr              = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x18);
		Xuint32 *load_vector_incrementos_tiempo_addr    = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x1C);
		Xuint32 *t_intr_addr                            = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x20);
		Xuint32 *prioridades                            = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x24);
		Xuint32 *new_disparo_addr                       = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x28);
		Xuint32 *sacar_disparo_addr                     = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x2C);
		Xuint32 *error_addr                             = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x30);

//	Direcciones para leer en Petri
		Xuint32 *leer_disparos_en_espera    = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x34);
		Xuint32 *leer_disparos_posibles     = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x38);
		Xuint32 *leer_disparos_ejecutados   = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x3C);
		Xuint32 *leer_t_num                 = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x40);
		Xuint32 *red_activa                 = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x44);
		Xuint32 *leer_marcado_p_cero        = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x48);
		Xuint32 *leer_marcado_p_uno         = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x4C);
		Xuint32 *leer_marcado_p_dos         = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x50);
		Xuint32 *leer_marcado_p_tres        = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x54);
		Xuint32 *leer_marcado_p_cuatro      = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x58);
		Xuint32 *leer_marcado_p_cinco       = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x5C);
		Xuint32 *leer_marcado_p_seis        = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x60);
		Xuint32 *leer_marcado_p_siete       = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x64);
		Xuint32 *leer_marcado_p_ocho        = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x68);
		Xuint32 *leer_marcado_p_nueve       = (Xuint32 *)(XPAR_TIMED_PETRI_AXI_0_BASEADDR + 0x6C);

void* master_thread(void *);
void* cortar();
void* lijar();
void* pintar();
void* secar();
void* armar();
void petri_intr_handler();

sem_t semaforos[5];
sem_t console_mutex;

/* Data */
int cantidad_plazas = 10;
int cantidad_transiciones = 7;
int cantidad_palabras_vector_tiempo = 4;
int cantidad_de_mesas = 0;

/* Functions */
int main()
{
    init_platform();
    
    xil_printf("INICIO DE EJECUCION PETRI - %d Mesas", cantidad_de_mesas_para_armar);

    /* Cargar los datos de la Red de Petri */
		//print("Comienza la Carga de la Matriz de Incidencia Positiva\r\n");
			*(m_incidencia_positiva_addr) = 0x00000000;
			*(m_incidencia_positiva_addr) = 0x00010000;
			*(m_incidencia_positiva_addr) = 0x00020000;
			*(m_incidencia_positiva_addr) = 0x00030000;
			*(m_incidencia_positiva_addr) = 0x00040000;
			*(m_incidencia_positiva_addr) = 0x00050000;
			*(m_incidencia_positiva_addr) = 0x00060000;
                                          
			*(m_incidencia_positiva_addr) = 0x01000004;
			*(m_incidencia_positiva_addr) = 0x01010000;
			*(m_incidencia_positiva_addr) = 0x01020000;
			*(m_incidencia_positiva_addr) = 0x01030000;
			*(m_incidencia_positiva_addr) = 0x01040000;
			*(m_incidencia_positiva_addr) = 0x01050000;
			*(m_incidencia_positiva_addr) = 0x01060000;
                                          
			*(m_incidencia_positiva_addr) = 0x02000001;
			*(m_incidencia_positiva_addr) = 0x02010000;
			*(m_incidencia_positiva_addr) = 0x02020000;
			*(m_incidencia_positiva_addr) = 0x02030000;
			*(m_incidencia_positiva_addr) = 0x02040000;
			*(m_incidencia_positiva_addr) = 0x02050000;
			*(m_incidencia_positiva_addr) = 0x02060000;
                                          
			*(m_incidencia_positiva_addr) = 0x03000000;
			*(m_incidencia_positiva_addr) = 0x03010004;
			*(m_incidencia_positiva_addr) = 0x03020000;
			*(m_incidencia_positiva_addr) = 0x03030000;
			*(m_incidencia_positiva_addr) = 0x03040000;
			*(m_incidencia_positiva_addr) = 0x03050000;
			*(m_incidencia_positiva_addr) = 0x03060000;
                                          
			*(m_incidencia_positiva_addr) = 0x04000000;
			*(m_incidencia_positiva_addr) = 0x04010001;
			*(m_incidencia_positiva_addr) = 0x04020000;
			*(m_incidencia_positiva_addr) = 0x04030000;
			*(m_incidencia_positiva_addr) = 0x04040000;
			*(m_incidencia_positiva_addr) = 0x04050000;
			*(m_incidencia_positiva_addr) = 0x04060000;
                                          
			*(m_incidencia_positiva_addr) = 0x05000000;
			*(m_incidencia_positiva_addr) = 0x05010000;
			*(m_incidencia_positiva_addr) = 0x05020004;
			*(m_incidencia_positiva_addr) = 0x05030000;
			*(m_incidencia_positiva_addr) = 0x05040000;
			*(m_incidencia_positiva_addr) = 0x05050000;
			*(m_incidencia_positiva_addr) = 0x05060000;
                                          
			*(m_incidencia_positiva_addr) = 0x06000000;
			*(m_incidencia_positiva_addr) = 0x06010000;
			*(m_incidencia_positiva_addr) = 0x06020001;
			*(m_incidencia_positiva_addr) = 0x06030000;
			*(m_incidencia_positiva_addr) = 0x06040000;
			*(m_incidencia_positiva_addr) = 0x06050000;
			*(m_incidencia_positiva_addr) = 0x06060000;
                                          
			*(m_incidencia_positiva_addr) = 0x07000000;
			*(m_incidencia_positiva_addr) = 0x07010000;
			*(m_incidencia_positiva_addr) = 0x07020000;
			*(m_incidencia_positiva_addr) = 0x07030004;
			*(m_incidencia_positiva_addr) = 0x07040000;
			*(m_incidencia_positiva_addr) = 0x07050000;
			*(m_incidencia_positiva_addr) = 0x07060000;
                                          
			*(m_incidencia_positiva_addr) = 0x08000000;
			*(m_incidencia_positiva_addr) = 0x08010000;
			*(m_incidencia_positiva_addr) = 0x08020000;
			*(m_incidencia_positiva_addr) = 0x08030001;
			*(m_incidencia_positiva_addr) = 0x08040000;
			*(m_incidencia_positiva_addr) = 0x08050000;
			*(m_incidencia_positiva_addr) = 0x08060000;
                                          
			*(m_incidencia_positiva_addr) = 0x09000000;
			*(m_incidencia_positiva_addr) = 0x09010000;
			*(m_incidencia_positiva_addr) = 0x09020000;
			*(m_incidencia_positiva_addr) = 0x09030000;
			*(m_incidencia_positiva_addr) = 0x09040001;
			*(m_incidencia_positiva_addr) = 0x09050000;
			*(m_incidencia_positiva_addr) = 0x09060000;
		//print("Termino Carga de la Matriz de Incidencia Positiva\r\n\n");

		//print("Comienza la Carga de la Matriz de Incidencia Negativa\r\n");
			*(m_incidencia_negativa_addr) = 0x00000001;
			*(m_incidencia_negativa_addr) = 0x00010000;
			*(m_incidencia_negativa_addr) = 0x00020000;
			*(m_incidencia_negativa_addr) = 0x00030000;
			*(m_incidencia_negativa_addr) = 0x00040000;
			*(m_incidencia_negativa_addr) = 0x00050000;
			*(m_incidencia_negativa_addr) = 0x00060000;
                                          
			*(m_incidencia_negativa_addr) = 0x01000000;
			*(m_incidencia_negativa_addr) = 0x01010004;
			*(m_incidencia_negativa_addr) = 0x01020000;
			*(m_incidencia_negativa_addr) = 0x01030000;
			*(m_incidencia_negativa_addr) = 0x01040000;
			*(m_incidencia_negativa_addr) = 0x01050000;
			*(m_incidencia_negativa_addr) = 0x01060000;
                                          
			*(m_incidencia_negativa_addr) = 0x02000000;
			*(m_incidencia_negativa_addr) = 0x02010001;
			*(m_incidencia_negativa_addr) = 0x02020000;
			*(m_incidencia_negativa_addr) = 0x02030000;
			*(m_incidencia_negativa_addr) = 0x02040000;
			*(m_incidencia_negativa_addr) = 0x02050000;
			*(m_incidencia_negativa_addr) = 0x02060000;
                                          
			*(m_incidencia_negativa_addr) = 0x03000000;
			*(m_incidencia_negativa_addr) = 0x03010000;
			*(m_incidencia_negativa_addr) = 0x03020004;
			*(m_incidencia_negativa_addr) = 0x03030000;
			*(m_incidencia_negativa_addr) = 0x03040000;
			*(m_incidencia_negativa_addr) = 0x03050000;
			*(m_incidencia_negativa_addr) = 0x03060000;
                                          
			*(m_incidencia_negativa_addr) = 0x04000000;
			*(m_incidencia_negativa_addr) = 0x04010000;
			*(m_incidencia_negativa_addr) = 0x04020001;
			*(m_incidencia_negativa_addr) = 0x04030000;
			*(m_incidencia_negativa_addr) = 0x04040000;
			*(m_incidencia_negativa_addr) = 0x04050000;
			*(m_incidencia_negativa_addr) = 0x04060000;
                                          
			*(m_incidencia_negativa_addr) = 0x05000000;
			*(m_incidencia_negativa_addr) = 0x05010000;
			*(m_incidencia_negativa_addr) = 0x05020000;
			*(m_incidencia_negativa_addr) = 0x05030004;
			*(m_incidencia_negativa_addr) = 0x05040000;
			*(m_incidencia_negativa_addr) = 0x05050000;
			*(m_incidencia_negativa_addr) = 0x05060000;
                                          
			*(m_incidencia_negativa_addr) = 0x06000000;
			*(m_incidencia_negativa_addr) = 0x06010000;
			*(m_incidencia_negativa_addr) = 0x06020000;
			*(m_incidencia_negativa_addr) = 0x06030001;
			*(m_incidencia_negativa_addr) = 0x06040000;
			*(m_incidencia_negativa_addr) = 0x06050000;
			*(m_incidencia_negativa_addr) = 0x06060000;
                                          
			*(m_incidencia_negativa_addr) = 0x07000000;
			*(m_incidencia_negativa_addr) = 0x07010000;
			*(m_incidencia_negativa_addr) = 0x07020000;
			*(m_incidencia_negativa_addr) = 0x07030000;
			*(m_incidencia_negativa_addr) = 0x07040004;
			*(m_incidencia_negativa_addr) = 0x07050000;
			*(m_incidencia_negativa_addr) = 0x07060000;
                                          
			*(m_incidencia_negativa_addr) = 0x08000000;
			*(m_incidencia_negativa_addr) = 0x08010000;
			*(m_incidencia_negativa_addr) = 0x08020000;
			*(m_incidencia_negativa_addr) = 0x08030000;
			*(m_incidencia_negativa_addr) = 0x08040001;
			*(m_incidencia_negativa_addr) = 0x08050000;
			*(m_incidencia_negativa_addr) = 0x08060000;
                                          
			*(m_incidencia_negativa_addr) = 0x09000000;
			*(m_incidencia_negativa_addr) = 0x09010000;
			*(m_incidencia_negativa_addr) = 0x09020000;
			*(m_incidencia_negativa_addr) = 0x09030000;
			*(m_incidencia_negativa_addr) = 0x09040000;
			*(m_incidencia_negativa_addr) = 0x09050000;
			*(m_incidencia_negativa_addr) = 0x09060000;
		//print("Termino Carga de la Matriz de Incidencia Negativa\r\n\n");

		//print("Comienza la Carga de la Matriz de Inhibicion\r\n");
			*(m_inhibicion_addr) = 0x00000000;
			*(m_inhibicion_addr) = 0x00010000;
			*(m_inhibicion_addr) = 0x00020000;
			*(m_inhibicion_addr) = 0x00030000;
			*(m_inhibicion_addr) = 0x00040000;
			*(m_inhibicion_addr) = 0x00050000;
			*(m_inhibicion_addr) = 0x00060000;
                                 
			*(m_inhibicion_addr) = 0x01000000;
			*(m_inhibicion_addr) = 0x01010000;
			*(m_inhibicion_addr) = 0x01020000;
			*(m_inhibicion_addr) = 0x01030000;
			*(m_inhibicion_addr) = 0x01040000;
			*(m_inhibicion_addr) = 0x01050000;
			*(m_inhibicion_addr) = 0x01060000;
                                 
			*(m_inhibicion_addr) = 0x02000000;
			*(m_inhibicion_addr) = 0x02010000;
			*(m_inhibicion_addr) = 0x02020000;
			*(m_inhibicion_addr) = 0x02030000;
			*(m_inhibicion_addr) = 0x02040000;
			*(m_inhibicion_addr) = 0x02050000;
			*(m_inhibicion_addr) = 0x02060000;
                                 
			*(m_inhibicion_addr) = 0x03000000;
			*(m_inhibicion_addr) = 0x03010000;
			*(m_inhibicion_addr) = 0x03020000;
			*(m_inhibicion_addr) = 0x03030000;
			*(m_inhibicion_addr) = 0x03040000;
			*(m_inhibicion_addr) = 0x03050000;
			*(m_inhibicion_addr) = 0x03060000;
                                 
			*(m_inhibicion_addr) = 0x04000000;
			*(m_inhibicion_addr) = 0x04010000;
			*(m_inhibicion_addr) = 0x04020000;
			*(m_inhibicion_addr) = 0x04030000;
			*(m_inhibicion_addr) = 0x04040000;
			*(m_inhibicion_addr) = 0x04050000;
			*(m_inhibicion_addr) = 0x04060000;
                                 
			*(m_inhibicion_addr) = 0x05000000;
			*(m_inhibicion_addr) = 0x05010000;
			*(m_inhibicion_addr) = 0x05020000;
			*(m_inhibicion_addr) = 0x05030000;
			*(m_inhibicion_addr) = 0x05040000;
			*(m_inhibicion_addr) = 0x05050000;
			*(m_inhibicion_addr) = 0x05060000;
                                 
			*(m_inhibicion_addr) = 0x06000000;
			*(m_inhibicion_addr) = 0x06010000;
			*(m_inhibicion_addr) = 0x06020000;
			*(m_inhibicion_addr) = 0x06030000;
			*(m_inhibicion_addr) = 0x06040000;
			*(m_inhibicion_addr) = 0x06050000;
			*(m_inhibicion_addr) = 0x06060000;
                                 
			*(m_inhibicion_addr) = 0x07000000;
			*(m_inhibicion_addr) = 0x07010000;
			*(m_inhibicion_addr) = 0x07020000;
			*(m_inhibicion_addr) = 0x07030000;
			*(m_inhibicion_addr) = 0x07040000;
			*(m_inhibicion_addr) = 0x07050000;
			*(m_inhibicion_addr) = 0x07060000;
                                 
			*(m_inhibicion_addr) = 0x08000000;
			*(m_inhibicion_addr) = 0x08010000;
			*(m_inhibicion_addr) = 0x08020000;
			*(m_inhibicion_addr) = 0x08030000;
			*(m_inhibicion_addr) = 0x08040000;
			*(m_inhibicion_addr) = 0x08050000;
			*(m_inhibicion_addr) = 0x08060000;
                                 
			*(m_inhibicion_addr) = 0x09000000;
			*(m_inhibicion_addr) = 0x09010000;
			*(m_inhibicion_addr) = 0x09020000;
			*(m_inhibicion_addr) = 0x09030000;
			*(m_inhibicion_addr) = 0x09040000;
			*(m_inhibicion_addr) = 0x09050000;
			*(m_inhibicion_addr) = 0x09060000;
		//print("Termino Carga de la Matriz de Inhibicion\r\n\n");

		//print("Comienza la Carga del vector de Cotas de Plazas \r\n");
			*(p_cotas_addr) = cota_plaza_p_cero;
			*(p_cotas_addr) = 0x01000009;
			*(p_cotas_addr) = 0x02000003;
			*(p_cotas_addr) = 0x03000005;
			*(p_cotas_addr) = 0x04000002;
			*(p_cotas_addr) = 0x05000005;
			*(p_cotas_addr) = 0x06000002;
			*(p_cotas_addr) = 0x07000009;
			*(p_cotas_addr) = 0x08000003;
			*(p_cotas_addr) = cota_plaza_p_nueve;
		//print("Termino Carga del vector de Cotas de Plazas\r\n\n");

		//print("Comienza la Carga del vector de Transiciones Automaticas\r\n");
			*(t_automatica_addr) = 0x00000000;
			*(t_automatica_addr) = 0x00010000;
			*(t_automatica_addr) = 0x00020000;
			*(t_automatica_addr) = 0x00030000;
			*(t_automatica_addr) = 0x00040000;
			*(t_automatica_addr) = 0x00050000;
			*(t_automatica_addr) = 0x00060000;
		//print("Termino Carga del vector de Transiciones Automaticas\r\n\n");

		//print("Comienza la Carga del vector Duracion\r\n");
			*(load_vector_duracion_addr) = 0x000049F0;//150000
			*(load_vector_duracion_addr) = 0x01000002;
			*(load_vector_duracion_addr) = 0x02000000;
			*(load_vector_duracion_addr) = 0x03000000;
                                         
			*(load_vector_duracion_addr) = 0x00016EE8;//225000
			*(load_vector_duracion_addr) = 0x01010003;
			*(load_vector_duracion_addr) = 0x02010000;
			*(load_vector_duracion_addr) = 0x03010000;
                                         
			*(load_vector_duracion_addr) = 0x000224F8;//75000
			*(load_vector_duracion_addr) = 0x01020001;
			*(load_vector_duracion_addr) = 0x02020000;
			*(load_vector_duracion_addr) = 0x03020000;
                                         
			*(load_vector_duracion_addr) = 0x000349F0;//150000
			*(load_vector_duracion_addr) = 0x01030002;
			*(load_vector_duracion_addr) = 0x02030000;
			*(load_vector_duracion_addr) = 0x03030000;
                                         
			*(load_vector_duracion_addr) = 0x000424F8;//75000
			*(load_vector_duracion_addr) = 0x01040001;
			*(load_vector_duracion_addr) = 0x02040000;
			*(load_vector_duracion_addr) = 0x03040000;
                                         
			*(load_vector_duracion_addr) = 0x00050000;
			*(load_vector_duracion_addr) = 0x01050000;
			*(load_vector_duracion_addr) = 0x02050000;
			*(load_vector_duracion_addr) = 0x03050000;
                                         
			*(load_vector_duracion_addr) = 0x00060000;
			*(load_vector_duracion_addr) = 0x01060000;
			*(load_vector_duracion_addr) = 0x02060000;
			*(load_vector_duracion_addr) = 0x03060000;
		//print("Termino Carga del vector Duracion\r\n\n");

		//print("Comienza la Carga del vector de incrementos\r\n");
			*(load_vector_incrementos_tiempo_addr) = 0x00000001;
			*(load_vector_incrementos_tiempo_addr) = 0x00010001;
			*(load_vector_incrementos_tiempo_addr) = 0x00020001;
			*(load_vector_incrementos_tiempo_addr) = 0x00030001;
			*(load_vector_incrementos_tiempo_addr) = 0x00040001;
			*(load_vector_incrementos_tiempo_addr) = 0x00050001;
			*(load_vector_incrementos_tiempo_addr) = 0x00060001;
		//print("Termino Carga del vector de de incrementos\r\n\n");

		//print("Comienza la Carga del vector de Marcado Inicial\r\n");
			*(m_marcado_addr) = marcado_inicial_p_cero;
			*(m_marcado_addr) = 0x01000000;
			*(m_marcado_addr) = 0x02000000;
			*(m_marcado_addr) = 0x03000000;
			*(m_marcado_addr) = 0x04000000;
			*(m_marcado_addr) = 0x05000000;
			*(m_marcado_addr) = 0x06000000;
			*(m_marcado_addr) = 0x07000000;
			*(m_marcado_addr) = 0x08000000;
			*(m_marcado_addr) = 0x09000000;
		//print("Termino Carga del vector de Marcado Inicial\r\n\n");

		//print("Comienza la Carga del vector de Transiciones con Interrupcion\r\n");
			*(t_intr_addr) = 0x00000001;
			*(t_intr_addr) = 0x00010001;
			*(t_intr_addr) = 0x00020001;
			*(t_intr_addr) = 0x00030001;
			*(t_intr_addr) = 0x00040001;
			*(t_intr_addr) = 0x00050000;
			*(t_intr_addr) = 0x00060000;
		//print("Termino Carga del vector de Transiciones con Interrupcion\r\n");

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

    //Iniciar Interrupcion.
         register_int_handler(XPAR_MICROBLAZE_0_INTC_TIMED_PETRI_AXI_0_INTERRUPT_INTR,petri_intr_handler,
         (void*)XPAR_INTC_0_BASEADDR);
         enable_interrupt(XPAR_MICROBLAZE_0_INTC_TIMED_PETRI_AXI_0_INTERRUPT_INTR);

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

        status=0;
    	status = status + sem_init(&semaforos[0], 1, 0);
    	status = status + sem_init(&semaforos[1], 1, 0);
    	status = status + sem_init(&semaforos[2], 1, 0);
    	status = status + sem_init(&semaforos[3], 1, 0);
    	status = status + sem_init(&semaforos[4], 1, 0);
    	status = status + sem_init(&console_mutex, 1, 1);
    	if(status == 0)
    	{	//print("\nSemaforos creado correctamente\r\n");

    	}
    	else
    	{
    		print("Error al crear el semaforo\r\n");
    		pthread_exit(NULL);
    	}

    /* This is how we can join with a thread and reclaim its return value */
    	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int *mesas_armadas;


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
  //  	if (status != 0)	{  	xil_printf ("ERROR LANZANDO HILO CORTADOR");	}
    status = pthread_create(&hilo_lijador, &attr, (void*)lijar, NULL);
  //  	if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO LIJADOR");	}
    status = pthread_create(&hilo_pintor, &attr, (void*)pintar, NULL);
  //  	if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO PINTOR");	}
    status = pthread_create(&hilo_secador, &attr, (void*)secar, NULL);
  //  	if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO SECADOR");	}
    status = pthread_create(&hilo_armador, &attr, (void*)armar, NULL);
  //  	if (status != 0)	{ 	xil_printf ("ERROR LANZANDO HILO ARMADOR");	}

    pthread_join(hilo_armador,(void*)&mesas_armadas);

    //Terminar cuenta de tiempo
      	valor_final_timer = XTmrCtr_GetTimerCounterReg(XPAR_AXI_TIMER_1_BASEADDR,0);
       	xil_printf("Valor final del timer: %d\r\n\n\n", valor_final_timer);

    xil_printf("SE ARMARON %d MESAS\r\n\n", cantidad_de_mesas);

    return (void*)0;
}

/* HILOS */

void* cortar()
{
	int i=0;

	while(cantidad_de_mesas < cantidad_de_mesas_para_armar)
	{
		//Pedir T0
			*(new_disparo_addr) = disparo_t_cero;

		//Ver si T0 se ejecuto
			sem_wait(&semaforos[0]);

		//Sacar T0 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_cero;

//			i=i+1;
//			sem_wait(&console_mutex);
//			xil_printf("Mesa %d: CORTADA\r\n",i);
//			sem_post(&console_mutex);
	}//Fin del while
	pthread_exit((void*)i);
	return NULL;
}

void* lijar()
{
	int i=0;

	while(cantidad_de_mesas < cantidad_de_mesas_para_armar)
	{
		//Solicitar el disparo de T1
			*(new_disparo_addr) = disparo_t_uno;


		//Ver si T1 se ejecuto
			sem_wait(&semaforos[1]);

		//Sacar T1 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_uno;
			i=i+1;
//			sem_wait(&console_mutex);
//			xil_printf("Mesa %d: LIJADA\r\n",i);
//			sem_post(&console_mutex);
	}//Fin del while
	pthread_exit((void*)i);
	return NULL;
}

void* pintar()
{
	int i=0;

	while (cantidad_de_mesas < cantidad_de_mesas_para_armar)
	{
		//Pedir T2
			*(new_disparo_addr) = disparo_t_dos;


		//Ver si T2 se ejecuto
			sem_wait(&semaforos[2]);

		//Sacar T2 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_dos;

//			i=i+1;
//			sem_wait(&console_mutex);
//			xil_printf("Mesa %d: PINTADA\r\n",i);
//			sem_post(&console_mutex);
	}//Fin del while
	pthread_exit((void*)i);
	return NULL;
}

void* secar()
{
	int i=0;

	while(cantidad_de_mesas < cantidad_de_mesas_para_armar)
	{
		//Solicitar el disparo de T3
			*(new_disparo_addr) = disparo_t_tres;
		//Ver si T3 se ejecuto
			sem_wait(&semaforos[3]);
		//Sacar T3 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_tres;
//			i=i+1;
//			sem_wait(&console_mutex);
//			xil_printf("Mesa %d: SECADA\r\n",i);
//			sem_post(&console_mutex);
	}//Fin del while
	pthread_exit((void*)i);
	return NULL;
}

void* armar()
{
	int i=0;

	while(cantidad_de_mesas < cantidad_de_mesas_para_armar)
	{
		//Pedir T4
			*(new_disparo_addr) = disparo_t_cuatro;
		//Ver si T4 se ejecuto
			sem_wait(&semaforos[4]);
		//Sacar T4 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_cuatro;
		//ARMAR UNA MESA
			cantidad_de_mesas = cantidad_de_mesas + 1;
//			i=i+1;
//			sem_wait(&console_mutex);
//			xil_printf("Mesa %d: ARMADA\r\n",i);
//			sem_post(&console_mutex);
	}//Fin del while

	pthread_exit((void*)i);
	return NULL;
}
void petri_intr_handler()
{
	sem_post(&semaforos[*(leer_t_num)]);
}
