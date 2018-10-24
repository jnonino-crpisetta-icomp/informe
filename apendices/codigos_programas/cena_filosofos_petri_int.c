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

/* Declarations */
#define maximo_comidas_cada_filosofo 10000

#define maximo_comidas_filosofo_uno maximo_comidas_cada_filosofo
#define maximo_comidas_filosofo_dos maximo_comidas_cada_filosofo
#define maximo_comidas_filosofo_tres maximo_comidas_cada_filosofo

#define disparo_t_cero		0x00000000
#define disparo_t_uno 		0x00010000
#define disparo_t_dos 		0x00020000
#define disparo_t_tres 		0x00030000
#define disparo_t_cuatro	0x00040000
#define disparo_t_cinco		0x00050000
#define disparo_t_seis		0x00060000

#define comprobacion_t_cero  	0x00000001
#define comprobacion_t_uno   	0x00000002
#define comprobacion_t_dos   	0x00000004
#define comprobacion_t_tres  	0x00000008
#define comprobacion_t_cuatro	0x00000010
#define comprobacion_t_cinco   	0x00000020
#define comprobacion_t_seis   	0x00000040

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
void* filosofo_uno();
void* filosofo_dos();
void* filosofo_tres();
void petri_intr_handler();
void mostrar_marcado();
sem_t semaforos[3];
sem_t console_mutex;

/* Data */
int cantidad_plazas = 10;
int cantidad_transiciones = 7;
int cantidad_palabras_vector_tiempo = 4;

int cantidad_comidas_filosofo_uno = 0;
int cantidad_comidas_filosofo_dos = 0;
int cantidad_comidas_filosofo_tres = 0;

/* Functions */
int main()
{
    init_platform();
    
    xil_printf("INICIO DE EJECUCION PETRI - %d Comidas para cada filosofo", maximo_comidas_cada_filosofo);

    /* Cargar los datos de la Red de Petri */
		//print("Comienza la Carga de la Matriz de Incidencia Positiva\r\n");
			*(m_incidencia_positiva_addr) = 0x00000001;
			*(m_incidencia_positiva_addr) = 0x00010000;
			*(m_incidencia_positiva_addr) = 0x00020000;
			*(m_incidencia_positiva_addr) = 0x00030000;
			*(m_incidencia_positiva_addr) = 0x00040000;
			*(m_incidencia_positiva_addr) = 0x00050000;
			*(m_incidencia_positiva_addr) = 0x00060000;
                                          
			*(m_incidencia_positiva_addr) = 0x01000000;
			*(m_incidencia_positiva_addr) = 0x01010001;
			*(m_incidencia_positiva_addr) = 0x01020000;
			*(m_incidencia_positiva_addr) = 0x01030000;
			*(m_incidencia_positiva_addr) = 0x01040000;
			*(m_incidencia_positiva_addr) = 0x01050000;
			*(m_incidencia_positiva_addr) = 0x01060000;
                                          
			*(m_incidencia_positiva_addr) = 0x02000000;
			*(m_incidencia_positiva_addr) = 0x02010000;
			*(m_incidencia_positiva_addr) = 0x02020001;
			*(m_incidencia_positiva_addr) = 0x02030000;
			*(m_incidencia_positiva_addr) = 0x02040000;
			*(m_incidencia_positiva_addr) = 0x02050000;
			*(m_incidencia_positiva_addr) = 0x02060000;
                                          
			*(m_incidencia_positiva_addr) = 0x03000000;
			*(m_incidencia_positiva_addr) = 0x03010000;
			*(m_incidencia_positiva_addr) = 0x03020000;
			*(m_incidencia_positiva_addr) = 0x03030001;
			*(m_incidencia_positiva_addr) = 0x03040000;
			*(m_incidencia_positiva_addr) = 0x03050000;
			*(m_incidencia_positiva_addr) = 0x03060000;
                                          
			*(m_incidencia_positiva_addr) = 0x04000000;
			*(m_incidencia_positiva_addr) = 0x04010000;
			*(m_incidencia_positiva_addr) = 0x04020000;
			*(m_incidencia_positiva_addr) = 0x04030000;
			*(m_incidencia_positiva_addr) = 0x04040001;
			*(m_incidencia_positiva_addr) = 0x04050000;
			*(m_incidencia_positiva_addr) = 0x04060000;
                                          
			*(m_incidencia_positiva_addr) = 0x05000000;
			*(m_incidencia_positiva_addr) = 0x05010000;
			*(m_incidencia_positiva_addr) = 0x05020000;
			*(m_incidencia_positiva_addr) = 0x05030000;
			*(m_incidencia_positiva_addr) = 0x05040000;
			*(m_incidencia_positiva_addr) = 0x05050001;
			*(m_incidencia_positiva_addr) = 0x05060000;
                                          
			*(m_incidencia_positiva_addr) = 0x06000000;
			*(m_incidencia_positiva_addr) = 0x06010000;
			*(m_incidencia_positiva_addr) = 0x06020000;
			*(m_incidencia_positiva_addr) = 0x06030001;
			*(m_incidencia_positiva_addr) = 0x06040001;
			*(m_incidencia_positiva_addr) = 0x06050000;
			*(m_incidencia_positiva_addr) = 0x06060000;
                                          
			*(m_incidencia_positiva_addr) = 0x07000000;
			*(m_incidencia_positiva_addr) = 0x07010000;
			*(m_incidencia_positiva_addr) = 0x07020000;
			*(m_incidencia_positiva_addr) = 0x07030000;
			*(m_incidencia_positiva_addr) = 0x07040001;
			*(m_incidencia_positiva_addr) = 0x07050001;
			*(m_incidencia_positiva_addr) = 0x07060000;
                                          
			*(m_incidencia_positiva_addr) = 0x08000000;
			*(m_incidencia_positiva_addr) = 0x08010000;
			*(m_incidencia_positiva_addr) = 0x08020000;
			*(m_incidencia_positiva_addr) = 0x08030001;
			*(m_incidencia_positiva_addr) = 0x08040000;
			*(m_incidencia_positiva_addr) = 0x08050001;
			*(m_incidencia_positiva_addr) = 0x08060000;
                                          
			*(m_incidencia_positiva_addr) = 0x09000000;
			*(m_incidencia_positiva_addr) = 0x09010000;
			*(m_incidencia_positiva_addr) = 0x09020000;
			*(m_incidencia_positiva_addr) = 0x09030000;
			*(m_incidencia_positiva_addr) = 0x09040000;
			*(m_incidencia_positiva_addr) = 0x09050000;
			*(m_incidencia_positiva_addr) = 0x09060000;
		//print("Termino Carga de la Matriz de Incidencia Positiva\r\n\n");

		//print("Comienza la Carga de la Matriz de Incidencia Negativa\r\n");
			*(m_incidencia_negativa_addr) = 0x00000000;
			*(m_incidencia_negativa_addr) = 0x00010000;
			*(m_incidencia_negativa_addr) = 0x00020000;
			*(m_incidencia_negativa_addr) = 0x00030001;
			*(m_incidencia_negativa_addr) = 0x00040000;
			*(m_incidencia_negativa_addr) = 0x00050000;
			*(m_incidencia_negativa_addr) = 0x00060000;
                                          
			*(m_incidencia_negativa_addr) = 0x01000000;
			*(m_incidencia_negativa_addr) = 0x01010000;
			*(m_incidencia_negativa_addr) = 0x01020000;
			*(m_incidencia_negativa_addr) = 0x01030000;
			*(m_incidencia_negativa_addr) = 0x01040001;
			*(m_incidencia_negativa_addr) = 0x01050000;
			*(m_incidencia_negativa_addr) = 0x01060000;
                                          
			*(m_incidencia_negativa_addr) = 0x02000000;
			*(m_incidencia_negativa_addr) = 0x02010000;
			*(m_incidencia_negativa_addr) = 0x02020000;
			*(m_incidencia_negativa_addr) = 0x02030000;
			*(m_incidencia_negativa_addr) = 0x02040000;
			*(m_incidencia_negativa_addr) = 0x02050001;
			*(m_incidencia_negativa_addr) = 0x02060000;
                                          
			*(m_incidencia_negativa_addr) = 0x03000001;
			*(m_incidencia_negativa_addr) = 0x03010000;
			*(m_incidencia_negativa_addr) = 0x03020000;
			*(m_incidencia_negativa_addr) = 0x03030000;
			*(m_incidencia_negativa_addr) = 0x03040000;
			*(m_incidencia_negativa_addr) = 0x03050000;
			*(m_incidencia_negativa_addr) = 0x03060000;
                                          
			*(m_incidencia_negativa_addr) = 0x04000000;
			*(m_incidencia_negativa_addr) = 0x04010001;
			*(m_incidencia_negativa_addr) = 0x04020000;
			*(m_incidencia_negativa_addr) = 0x04030000;
			*(m_incidencia_negativa_addr) = 0x04040000;
			*(m_incidencia_negativa_addr) = 0x04050000;
			*(m_incidencia_negativa_addr) = 0x04060000;
                                          
			*(m_incidencia_negativa_addr) = 0x05000000;
			*(m_incidencia_negativa_addr) = 0x05010000;
			*(m_incidencia_negativa_addr) = 0x05020001;
			*(m_incidencia_negativa_addr) = 0x05030000;
			*(m_incidencia_negativa_addr) = 0x05040000;
			*(m_incidencia_negativa_addr) = 0x05050000;
			*(m_incidencia_negativa_addr) = 0x05060000;
                                          
			*(m_incidencia_negativa_addr) = 0x06000001;
			*(m_incidencia_negativa_addr) = 0x06010001;
			*(m_incidencia_negativa_addr) = 0x06020000;
			*(m_incidencia_negativa_addr) = 0x06030000;
			*(m_incidencia_negativa_addr) = 0x06040000;
			*(m_incidencia_negativa_addr) = 0x06050000;
			*(m_incidencia_negativa_addr) = 0x06060000;
                                          
			*(m_incidencia_negativa_addr) = 0x07000000;
			*(m_incidencia_negativa_addr) = 0x07010001;
			*(m_incidencia_negativa_addr) = 0x07020001;
			*(m_incidencia_negativa_addr) = 0x07030000;
			*(m_incidencia_negativa_addr) = 0x07040000;
			*(m_incidencia_negativa_addr) = 0x07050000;
			*(m_incidencia_negativa_addr) = 0x07060000;
                                          
			*(m_incidencia_negativa_addr) = 0x08000001;
			*(m_incidencia_negativa_addr) = 0x08010000;
			*(m_incidencia_negativa_addr) = 0x08020001;
			*(m_incidencia_negativa_addr) = 0x08030000;
			*(m_incidencia_negativa_addr) = 0x08040000;
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
			*(p_cotas_addr) = 0x00000001;
			*(p_cotas_addr) = 0x01000001;
			*(p_cotas_addr) = 0x02000001;
			*(p_cotas_addr) = 0x03000001;
			*(p_cotas_addr) = 0x04000001;
			*(p_cotas_addr) = 0x05000001;
			*(p_cotas_addr) = 0x06000001;
			*(p_cotas_addr) = 0x07000001;
			*(p_cotas_addr) = 0x08000001;
			*(p_cotas_addr) = 0x09000000;
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
			*(load_vector_duracion_addr) = 0x00000010;//0E10 = 3600
			*(load_vector_duracion_addr) = 0x01000000;
			*(load_vector_duracion_addr) = 0x02000000;
			*(load_vector_duracion_addr) = 0x03000000;
                                         
			*(load_vector_duracion_addr) = 0x00010010;//0E10 = 3600
			*(load_vector_duracion_addr) = 0x01010000;
			*(load_vector_duracion_addr) = 0x02010000;
			*(load_vector_duracion_addr) = 0x03010000;
                                         
			*(load_vector_duracion_addr) = 0x00020010;//0E10 = 3600
			*(load_vector_duracion_addr) = 0x01020000;
			*(load_vector_duracion_addr) = 0x02020000;
			*(load_vector_duracion_addr) = 0x03020000;
                                         
			*(load_vector_duracion_addr) = 0x00030000;
			*(load_vector_duracion_addr) = 0x01030000;
			*(load_vector_duracion_addr) = 0x02030000;
			*(load_vector_duracion_addr) = 0x03030000;
                                         
			*(load_vector_duracion_addr) = 0x00040000;
			*(load_vector_duracion_addr) = 0x01040000;
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
			*(load_vector_incrementos_tiempo_addr) = 0x00030000;
			*(load_vector_incrementos_tiempo_addr) = 0x00040000;
			*(load_vector_incrementos_tiempo_addr) = 0x00050000;
			*(load_vector_incrementos_tiempo_addr) = 0x00060000;
		//print("Termino Carga del vector de de incrementos\r\n\n");

		//print("Comienza la Carga del vector de Marcado Inicial\r\n");
			*(m_marcado_addr) = 0x00000000;
			*(m_marcado_addr) = 0x01000000;
			*(m_marcado_addr) = 0x02000000;
			*(m_marcado_addr) = 0x03000001;
			*(m_marcado_addr) = 0x04000001;
			*(m_marcado_addr) = 0x05000001;
			*(m_marcado_addr) = 0x06000001;
			*(m_marcado_addr) = 0x07000001;
			*(m_marcado_addr) = 0x08000001;
			*(m_marcado_addr) = 0x09000000;
		//print("Termino Carga del vector de Marcado Inicial\r\n\n");

		//print("Comienza la Carga del vector de Transiciones con Interrupcion\r\n");
			*(t_intr_addr) = 0x00000001;
			*(t_intr_addr) = 0x00010001;
			*(t_intr_addr) = 0x00020001;
			*(t_intr_addr) = 0x00030000;
			*(t_intr_addr) = 0x00040000;
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
	pthread_t hilo_filosofo_uno;
	pthread_t hilo_filosofo_dos;
	pthread_t hilo_filosofo_tres;
    pthread_attr_t attr;

    //Iniciar Interrupcion.
         register_int_handler(XPAR_MICROBLAZE_0_INTC_TIMED_PETRI_AXI_0_INTERRUPT_INTR,petri_intr_handler,(void*)XPAR_INTC_0_BASEADDR);
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

        int *comidas_uno;
        int *comidas_dos;
        int *comidas_tres;

        //Iniciar cuenta de tiempo
        	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR,0,0x0);
        	XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_1_BASEADDR, 0, 0x00000000);
           	XTmrCtr_LoadTimerCounterReg	(XPAR_AXI_TIMER_1_BASEADDR,0);
           	ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR,0);
           	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR, 0, ControlStatus & (~XTC_CSR_LOAD_MASK));
           	valor_inicial_timer = XTmrCtr_GetTimerCounterReg(XPAR_AXI_TIMER_1_BASEADDR,0);
           	xil_printf("\r\n\nValor inicial del timer: %d\r\n\n", valor_inicial_timer);
           	XTmrCtr_Enable(XPAR_AXI_TIMER_1_BASEADDR, 0);

        status = pthread_create(&hilo_filosofo_uno, &attr, (void*)filosofo_uno, NULL);
    //      	if (status != 0)	{  	xil_printf ("ERROR LANZANDO HILO FILOSOFO UNO");	}
        status = pthread_create(&hilo_filosofo_dos, &attr, (void*)filosofo_dos, NULL);
    //       	if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO FILOSOFO DOS");	}
        status = pthread_create(&hilo_filosofo_tres, &attr, (void*)filosofo_tres, NULL);
    //       	if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO FILOSOFO TRES");	}

    	pthread_join(hilo_filosofo_uno,(void*)&comidas_uno);
    	pthread_join(hilo_filosofo_dos,(void*)&comidas_dos);
    	pthread_join(hilo_filosofo_tres,(void*)&comidas_tres);

        //Terminar cuenta de tiempo
           	valor_final_timer = XTmrCtr_GetTimerCounterReg(XPAR_AXI_TIMER_1_BASEADDR,0);

        sem_wait(&console_mutex);
           	xil_printf("Valor final del timer: %d\r\n\n", valor_final_timer);
//           	xil_printf("Valor final comidas Filosofo 1: %d\r\n", cantidad_comidas_filosofo_uno);
//           	xil_printf("Valor final comidas Filosofo 2: %d\r\n", cantidad_comidas_filosofo_dos);
//           	xil_printf("Valor final comidas Filosofo 3: %d\r\n", cantidad_comidas_filosofo_tres);
       	sem_post(&console_mutex);

    return (void*)0;
}

/* HILOS */

void* filosofo_uno()
{
	while(cantidad_comidas_filosofo_uno < maximo_comidas_filosofo_uno)
	{
		//PEDIR CUBIERTOS
			//Solicitar el disparo de T0
				*(new_disparo_addr) = disparo_t_cero;
			//Ver si T0 se ejecuto
				sem_wait(&semaforos[0]);
			//Sacar T3 de los disparos ejecutados
				*(sacar_disparo_addr) = disparo_t_cero;

		//COMER
				cantidad_comidas_filosofo_uno = cantidad_comidas_filosofo_uno + 1;

		//DEVOLVER CUBIERTOS
			//Ver si T3 se ejecuto - Es automatica
				*(new_disparo_addr) = disparo_t_tres;
				while ( !( *(leer_disparos_ejecutados) & comprobacion_t_tres))
				{	yield();	}

			//Sacar T3 de los disparos ejecutados
				*(sacar_disparo_addr) = disparo_t_tres;

		//IMPRIMIR EN PANTALLA
//			sem_wait(&console_mutex);
//				xil_printf("FILOSOFO 1: Comio %d veces\r\n",cantidad_comidas_filosofo_uno);
//				mostrar_marcado();
//			sem_post(&console_mutex);

	}//Fin del while

	pthread_exit((void*)cantidad_comidas_filosofo_uno);
	return NULL;
}

void* filosofo_dos()
{
	while(cantidad_comidas_filosofo_dos < maximo_comidas_filosofo_dos)
	{
		//PEDIR CUBIERTOS
			//Solicitar el disparo de T1
				*(new_disparo_addr) = disparo_t_uno;
			//Ver si T1 se ejecuto
				sem_wait(&semaforos[1]);
			//Sacar T1 de los disparos ejecutados
				*(sacar_disparo_addr) = disparo_t_uno;

		//COMER
				cantidad_comidas_filosofo_dos = cantidad_comidas_filosofo_dos + 1;

		//DEVOLVER CUBIERTOS
			//Ver si T4 se ejecuto - Es automatica
				*(new_disparo_addr) = disparo_t_cuatro;
				while ( !( *(leer_disparos_ejecutados) & comprobacion_t_cuatro))
				{	yield();	}
			//Sacar T4 de los disparos ejecutados
				*(sacar_disparo_addr) = disparo_t_cuatro;

		//IMPRIMIR EN PANTALLA
//			sem_wait(&console_mutex);
//				xil_printf("FILOSOFO 2: Comio %d veces\r\n",cantidad_comidas_filosofo_dos);
//				mostrar_marcado();
//			sem_post(&console_mutex);

	}//Fin del while

	pthread_exit((void*)cantidad_comidas_filosofo_dos);
	return NULL;
}

void* filosofo_tres()
{
	while(cantidad_comidas_filosofo_tres < maximo_comidas_filosofo_tres)
	{
		//PEDIR CUBIERTOS
			//Solicitar el disparo de T2
				*(new_disparo_addr) = disparo_t_dos;
			//Ver si T2 se ejecuto
				sem_wait(&semaforos[2]);
			//Sacar T2 de los disparos ejecutados
				*(sacar_disparo_addr) = disparo_t_dos;

		//COMER
				cantidad_comidas_filosofo_tres = cantidad_comidas_filosofo_tres + 1;

		//DEVOLVER CUBIERTOS
			//Ver si T5 se ejecuto - Es automatica
				*(new_disparo_addr) = disparo_t_cinco;
				while ( !( *(leer_disparos_ejecutados) & comprobacion_t_cinco))
				{	yield();	}
			//Sacar T5 de los disparos ejecutados
				*(sacar_disparo_addr) = disparo_t_cinco;

		//IMPRIMIR EN PANTALLA
//			sem_wait(&console_mutex);
//				xil_printf("FILOSOFO 3: Comio %d veces\r\n",cantidad_comidas_filosofo_tres);
//				mostrar_marcado();
//			sem_post(&console_mutex);

	}//Fin del while

	pthread_exit((void*)cantidad_comidas_filosofo_tres);
	return NULL;
}

void mostrar_marcado()
{
	int marcado_cero;
	int marcado_uno;
	int marcado_dos;
	int marcado_tres;
	int marcado_cuatro;
	int marcado_cinco;
	int marcado_seis;
	int marcado_siete;
	int marcado_ocho;
	int marcado_nueve;

	marcado_cero = *(leer_marcado_p_cero);
	xil_printf ("\r\nFilosofo1Comiendo = %08xH\r\n",marcado_cero);
	marcado_uno = *(leer_marcado_p_uno);
	xil_printf ("Filosofo2Comiendo = %08xH\r\n", marcado_uno);
	marcado_dos = *(leer_marcado_p_dos);
	xil_printf ("Filosofo3Comiendo = %08xH\r\n", marcado_dos);
	marcado_tres = *(leer_marcado_p_tres);
	xil_printf ("Plato1 = %08xH\r\n", marcado_tres);
	marcado_cuatro = *(leer_marcado_p_cuatro);
	xil_printf ("Plato2 = %08xH\r\n", marcado_cuatro);
	marcado_cinco = *(leer_marcado_p_cinco);
	xil_printf ("Plato3 = %08xH\r\n", marcado_cinco);
	marcado_seis = *(leer_marcado_p_seis);
	xil_printf ("Tenedor1 = %08xH\r\n", marcado_seis);
	marcado_siete = *(leer_marcado_p_siete);
	xil_printf ("Tenedor2 = %08xH\r\n", marcado_siete);
	marcado_ocho = *(leer_marcado_p_ocho);
	xil_printf ("Tenedor3 = %08xH\r\n", marcado_ocho);
	marcado_nueve = *(leer_marcado_p_nueve);
	xil_printf ("Marcado de P9 = %08xH\r\n", marcado_nueve);
}

void petri_intr_handler()
{
	sem_post(&semaforos[*(leer_t_num)]);
}
