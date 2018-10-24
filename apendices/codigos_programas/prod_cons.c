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

//Threads
void* consumidor();
void* productor();

//Funciones
void mostrar_marcado();

/* Data */
int cantidad_plazas = 10;
int cantidad_transiciones = 10;
int cantidad_palabras_vector_tiempo = 4;
static int buffer;
static int cantidad_de_acciones;

//Direcciones para escribir en Petri
	Xuint32 *m_marcado_addr    									  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR);
	Xuint32 *m_incidencia_addr 									  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x4);
	Xuint32 *m_inhibicion_addr 									  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x8);
	Xuint32 *p_cotas_addr										      = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0xC);
	Xuint32 *t_automatica_addr 									  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x10);
	Xuint32 *load_vector_EFT_addr								  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x14);
	Xuint32 *load_vector_incrementos_tiempo_addr	= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x18);
	Xuint32 *load_vector_LFT_addr								  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x1C);
	Xuint32 *new_disparo_addr  									  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x20);
	Xuint32 *sacar_disparo_addr									  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x24);
	Xuint32 *t_intr_addr										      = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x28);
	Xuint32 *error_addr											      = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x2C);

//Direcciones para leer en Petri
	Xuint32 *leer_disparos_en_espera		= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x30);
	Xuint32 *leer_disparos_posibles			= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x34);
	Xuint32 *leer_disparos_ejecutados   = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x38);
	Xuint32 *leer_t_num								  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x3C);
	Xuint32 *red_activa								  = (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x40);
	Xuint32 *leer_marcado_p_cero				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x44);
	Xuint32 *leer_marcado_p_uno					= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x48);
	Xuint32 *leer_marcado_p_dos					= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x4C);
	Xuint32 *leer_marcado_p_tres				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x50);
	Xuint32 *leer_marcado_p_cuatro			= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x54);
	Xuint32 *leer_marcado_p_cinco				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x58);
	Xuint32 *leer_marcado_p_seis				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x5C);
	Xuint32 *leer_marcado_p_siete				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x60);
	Xuint32 *leer_marcado_p_ocho				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x64);
	Xuint32 *leer_marcado_p_nueve				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x68);
	Xuint32 *leer_marcado_p_diez				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x6C);
	Xuint32 *leer_marcado_p_once				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x70);
	Xuint32 *leer_marcado_p_doce				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x74);
	Xuint32 *leer_marcado_p_trece				= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x78);
	Xuint32 *leer_marcado_p_catorce			= (Xuint32 *)(XPAR_TIME_PETRI_AXI_0_BASEADDR + 0x7C);

/* Functions */
int main()
{
    init_platform();
    
    /* Initialize xilkernel */
    	xilkernel_init();
    
    //Carga de datos
	    print("\n\n Comienza la Carga de la Matriz de Incidencia \r\n\n");
		   	*(m_incidencia_addr) = 0x0000FFFF;
		   	*(m_incidencia_addr) = 0x00010001;
		   	*(m_incidencia_addr) = 0x0002FFFF;
		   	*(m_incidencia_addr) = 0x00030001;
		   	*(m_incidencia_addr) = 0x00040000;
		   	*(m_incidencia_addr) = 0x00050000;
		   	*(m_incidencia_addr) = 0x00060000;
		   	*(m_incidencia_addr) = 0x00070000;
		   	*(m_incidencia_addr) = 0x00080000;
		   	*(m_incidencia_addr) = 0x00090000;
                                 
		   	*(m_incidencia_addr) = 0x0100FFFF;
		   	*(m_incidencia_addr) = 0x01010000;
		   	*(m_incidencia_addr) = 0x01020000;
		   	*(m_incidencia_addr) = 0x01030001;
		   	*(m_incidencia_addr) = 0x01040000;
		   	*(m_incidencia_addr) = 0x01050000;
		   	*(m_incidencia_addr) = 0x01060000;
		   	*(m_incidencia_addr) = 0x01070000;
		   	*(m_incidencia_addr) = 0x01080000;
		   	*(m_incidencia_addr) = 0x01090000;
                                 
		   	*(m_incidencia_addr) = 0x02000000;
		   	*(m_incidencia_addr) = 0x02010001;
		   	*(m_incidencia_addr) = 0x0202FFFF;
		   	*(m_incidencia_addr) = 0x02030000;
		   	*(m_incidencia_addr) = 0x02040000;
		   	*(m_incidencia_addr) = 0x02050000;
		   	*(m_incidencia_addr) = 0x02060000;
		   	*(m_incidencia_addr) = 0x02070000;
		   	*(m_incidencia_addr) = 0x02080000;
		   	*(m_incidencia_addr) = 0x02090000;
                                 
		   	*(m_incidencia_addr) = 0x0300FFFF;
		   	*(m_incidencia_addr) = 0x03010001;
		   	*(m_incidencia_addr) = 0x03020000;
		   	*(m_incidencia_addr) = 0x03030000;
		   	*(m_incidencia_addr) = 0x03040000;
		   	*(m_incidencia_addr) = 0x03050000;
		   	*(m_incidencia_addr) = 0x03060000;
		   	*(m_incidencia_addr) = 0x03070000;
		   	*(m_incidencia_addr) = 0x03080000;
		   	*(m_incidencia_addr) = 0x03090000;
                                 
		   	*(m_incidencia_addr) = 0x04000001;
		   	*(m_incidencia_addr) = 0x0401FFFF;
		   	*(m_incidencia_addr) = 0x04020000;
		   	*(m_incidencia_addr) = 0x04030000;
		   	*(m_incidencia_addr) = 0x04040000;
		   	*(m_incidencia_addr) = 0x04050000;
		   	*(m_incidencia_addr) = 0x04060000;
		   	*(m_incidencia_addr) = 0x04070000;
		   	*(m_incidencia_addr) = 0x04080000;
		   	*(m_incidencia_addr) = 0x04090000;
                                 
		   	*(m_incidencia_addr) = 0x05000000;
		   	*(m_incidencia_addr) = 0x05010000;
		   	*(m_incidencia_addr) = 0x0502FFFF;
		   	*(m_incidencia_addr) = 0x05030001;
		   	*(m_incidencia_addr) = 0x05040000;
		   	*(m_incidencia_addr) = 0x05050000;
		   	*(m_incidencia_addr) = 0x05060000;
		   	*(m_incidencia_addr) = 0x05070000;
		   	*(m_incidencia_addr) = 0x05080000;
		   	*(m_incidencia_addr) = 0x05090000;
                                 
		   	*(m_incidencia_addr) = 0x06000000;
		   	*(m_incidencia_addr) = 0x06010000;
		   	*(m_incidencia_addr) = 0x06020001;
		   	*(m_incidencia_addr) = 0x0603FFFF;
		   	*(m_incidencia_addr) = 0x06040000;
		   	*(m_incidencia_addr) = 0x06050000;
		   	*(m_incidencia_addr) = 0x06060000;
		   	*(m_incidencia_addr) = 0x06070000;
		   	*(m_incidencia_addr) = 0x06080000;
		   	*(m_incidencia_addr) = 0x06090000;
                                 
		   	*(m_incidencia_addr) = 0x07000000;
		   	*(m_incidencia_addr) = 0x07010000;
		   	*(m_incidencia_addr) = 0x07020000;
		   	*(m_incidencia_addr) = 0x07030000;
		   	*(m_incidencia_addr) = 0x07040000;
		   	*(m_incidencia_addr) = 0x07050000;
		   	*(m_incidencia_addr) = 0x07060000;
		   	*(m_incidencia_addr) = 0x07070000;
		   	*(m_incidencia_addr) = 0x07080000;
		   	*(m_incidencia_addr) = 0x07090000;
                                 
		   	*(m_incidencia_addr) = 0x08000000;
		   	*(m_incidencia_addr) = 0x08010000;
		   	*(m_incidencia_addr) = 0x08020000;
		   	*(m_incidencia_addr) = 0x08030000;
		   	*(m_incidencia_addr) = 0x08040000;
		   	*(m_incidencia_addr) = 0x08050000;
		   	*(m_incidencia_addr) = 0x08060000;
		   	*(m_incidencia_addr) = 0x08070000;
		   	*(m_incidencia_addr) = 0x08080000;
		   	*(m_incidencia_addr) = 0x08090000;
                                 
		   	*(m_incidencia_addr) = 0x09000000;
		   	*(m_incidencia_addr) = 0x09010000;
		   	*(m_incidencia_addr) = 0x09020000;
		   	*(m_incidencia_addr) = 0x09030000;
		   	*(m_incidencia_addr) = 0x09040000;
		   	*(m_incidencia_addr) = 0x09050000;
		   	*(m_incidencia_addr) = 0x09060000;
		   	*(m_incidencia_addr) = 0x09070000;
		   	*(m_incidencia_addr) = 0x09080000;
		   	*(m_incidencia_addr) = 0x09090000;
		print("\n\n Termino Carga de la Matriz de Incidencia \r\n\n");

		print("\n\n Comienza la Carga de la Matriz de Inhibicion \r\n\n");
		   	*(m_inhibicion_addr) = 0x00000000;
		   	*(m_inhibicion_addr) = 0x00010000;
		   	*(m_inhibicion_addr) = 0x00020000;
		   	*(m_inhibicion_addr) = 0x00030000;
		   	*(m_inhibicion_addr) = 0x00040000;
		   	*(m_inhibicion_addr) = 0x00050000;
		   	*(m_inhibicion_addr) = 0x00060000;
		   	*(m_inhibicion_addr) = 0x00070000;
		   	*(m_inhibicion_addr) = 0x00080000;
		   	*(m_inhibicion_addr) = 0x00090000;
                                 
		   	*(m_inhibicion_addr) = 0x01000000;
		   	*(m_inhibicion_addr) = 0x01010000;
		   	*(m_inhibicion_addr) = 0x01020000;
		   	*(m_inhibicion_addr) = 0x01030000;
		   	*(m_inhibicion_addr) = 0x01040000;
		   	*(m_inhibicion_addr) = 0x01050000;
		   	*(m_inhibicion_addr) = 0x01060000;
		   	*(m_inhibicion_addr) = 0x01070000;
		   	*(m_inhibicion_addr) = 0x01080000;
		   	*(m_inhibicion_addr) = 0x01090000;
                                 
		   	*(m_inhibicion_addr) = 0x02000000;
		   	*(m_inhibicion_addr) = 0x02010000;
		   	*(m_inhibicion_addr) = 0x02020000;
		   	*(m_inhibicion_addr) = 0x02030000;
		   	*(m_inhibicion_addr) = 0x02040000;
		   	*(m_inhibicion_addr) = 0x02050000;
		   	*(m_inhibicion_addr) = 0x02060000;
		   	*(m_inhibicion_addr) = 0x02070000;
		   	*(m_inhibicion_addr) = 0x02080000;
		   	*(m_inhibicion_addr) = 0x02090000;
                                 
		   	*(m_inhibicion_addr) = 0x03000000;
		   	*(m_inhibicion_addr) = 0x03010000;
		   	*(m_inhibicion_addr) = 0x03020000;
		   	*(m_inhibicion_addr) = 0x03030000;
		   	*(m_inhibicion_addr) = 0x03040000;
		   	*(m_inhibicion_addr) = 0x03050000;
		   	*(m_inhibicion_addr) = 0x03060000;
		   	*(m_inhibicion_addr) = 0x03070000;
		   	*(m_inhibicion_addr) = 0x03080000;
		   	*(m_inhibicion_addr) = 0x03090000;
                                 
		   	*(m_inhibicion_addr) = 0x04000000;
		   	*(m_inhibicion_addr) = 0x04010000;
		   	*(m_inhibicion_addr) = 0x04020000;
		   	*(m_inhibicion_addr) = 0x04030000;
		   	*(m_inhibicion_addr) = 0x04040000;
		   	*(m_inhibicion_addr) = 0x04050000;
		   	*(m_inhibicion_addr) = 0x04060000;
		   	*(m_inhibicion_addr) = 0x04070000;
		   	*(m_inhibicion_addr) = 0x04080000;
		   	*(m_inhibicion_addr) = 0x04090000;
                                 
		   	*(m_inhibicion_addr) = 0x05000000;
		   	*(m_inhibicion_addr) = 0x05010000;
		   	*(m_inhibicion_addr) = 0x05020000;
		   	*(m_inhibicion_addr) = 0x05030000;
		   	*(m_inhibicion_addr) = 0x05040000;
		   	*(m_inhibicion_addr) = 0x05050000;
		   	*(m_inhibicion_addr) = 0x05060000;
		   	*(m_inhibicion_addr) = 0x05070000;
		   	*(m_inhibicion_addr) = 0x05080000;
		   	*(m_inhibicion_addr) = 0x05090000;
                                 
		   	*(m_inhibicion_addr) = 0x06000000;
		   	*(m_inhibicion_addr) = 0x06010000;
		   	*(m_inhibicion_addr) = 0x06020000;
		   	*(m_inhibicion_addr) = 0x06030000;
		   	*(m_inhibicion_addr) = 0x06040000;
		   	*(m_inhibicion_addr) = 0x06050000;
		   	*(m_inhibicion_addr) = 0x06060000;
		   	*(m_inhibicion_addr) = 0x06070000;
		   	*(m_inhibicion_addr) = 0x06080000;
		   	*(m_inhibicion_addr) = 0x06090000;
                                 
		   	*(m_inhibicion_addr) = 0x07000000;
		   	*(m_inhibicion_addr) = 0x07010000;
		   	*(m_inhibicion_addr) = 0x07020000;
		   	*(m_inhibicion_addr) = 0x07030000;
		   	*(m_inhibicion_addr) = 0x07040000;
		   	*(m_inhibicion_addr) = 0x07050000;
		   	*(m_inhibicion_addr) = 0x07060000;
		   	*(m_inhibicion_addr) = 0x07070000;
		   	*(m_inhibicion_addr) = 0x07080000;
		   	*(m_inhibicion_addr) = 0x07090000;
                                 
		   	*(m_inhibicion_addr) = 0x08000000;
		   	*(m_inhibicion_addr) = 0x08010000;
		   	*(m_inhibicion_addr) = 0x08020000;
		   	*(m_inhibicion_addr) = 0x08030000;
		   	*(m_inhibicion_addr) = 0x08040000;
		   	*(m_inhibicion_addr) = 0x08050000;
		   	*(m_inhibicion_addr) = 0x08060000;
		   	*(m_inhibicion_addr) = 0x08070000;
		   	*(m_inhibicion_addr) = 0x08080000;
		   	*(m_inhibicion_addr) = 0x08090000;
                                 
		   	*(m_inhibicion_addr) = 0x09000000;
		   	*(m_inhibicion_addr) = 0x09010000;
		   	*(m_inhibicion_addr) = 0x09020000;
		   	*(m_inhibicion_addr) = 0x09030000;
		   	*(m_inhibicion_addr) = 0x09040000;
		   	*(m_inhibicion_addr) = 0x09050000;
		   	*(m_inhibicion_addr) = 0x09060000;
		   	*(m_inhibicion_addr) = 0x09070000;
		   	*(m_inhibicion_addr) = 0x09080000;
		   	*(m_inhibicion_addr) = 0x09090000;
		print("\n\n Termino Carga de la Matriz de Inhibicion \r\n\n");

		print("\n\n Comienza la Carga del vector de Cotas de Plazas \r\n\n");
		   	*(p_cotas_addr) = 0x00000001;//mutex
		   	*(p_cotas_addr) = 0x01000005;//vacios
		   	*(p_cotas_addr) = 0x02000005;//llenos
		   	*(p_cotas_addr) = 0x03000001;//P3
		   	*(p_cotas_addr) = 0x04000001;//P4
		   	*(p_cotas_addr) = 0x05000001;//P5
		   	*(p_cotas_addr) = 0x06000001;//P6
		   	*(p_cotas_addr) = 0x0700FFFF;
		   	*(p_cotas_addr) = 0x0800FFFF;
		   	*(p_cotas_addr) = 0x0900FFFF;
		print("\n\n Termino Carga del vector de Cotas de Plazas \r\n\n");

		print("\n\n Comienza la Carga del vector de Transiciones Automaticas \r\n\n");
		   	*(t_automatica_addr) = 0x00000000;
		   	*(t_automatica_addr) = 0x00010000;
		   	*(t_automatica_addr) = 0x00020000;
		   	*(t_automatica_addr) = 0x00030000;
		   	*(t_automatica_addr) = 0x00040000;
		   	*(t_automatica_addr) = 0x00050000;
		   	*(t_automatica_addr) = 0x00060000;
		   	*(t_automatica_addr) = 0x00070000;
		   	*(t_automatica_addr) = 0x00080000;
		   	*(t_automatica_addr) = 0x00090000;
		print("\n\n Termino Carga del vector de Transiciones Automaticas \r\n\n");

		print("\n\n Comienza la Carga del vector EFT \r\n\n");
		   	*(load_vector_EFT_addr) = 0x00000000;
		   	*(load_vector_EFT_addr) = 0x01000000;
		   	*(load_vector_EFT_addr) = 0x02000000;
		   	*(load_vector_EFT_addr) = 0x03000000;
                                    
		   	*(load_vector_EFT_addr) = 0x00010000;
		   	*(load_vector_EFT_addr) = 0x01010000;
		   	*(load_vector_EFT_addr) = 0x02010000;
		   	*(load_vector_EFT_addr) = 0x03010000;
                                    
		   	*(load_vector_EFT_addr) = 0x000200FF;
		   	*(load_vector_EFT_addr) = 0x01020000;
		   	*(load_vector_EFT_addr) = 0x02020000;
		   	*(load_vector_EFT_addr) = 0x03020000;
                                    
		   	*(load_vector_EFT_addr) = 0x00030000;
		   	*(load_vector_EFT_addr) = 0x01030000;
		   	*(load_vector_EFT_addr) = 0x02030000;
		   	*(load_vector_EFT_addr) = 0x03030000;
                                    
		   	*(load_vector_EFT_addr) = 0x00040000;
		   	*(load_vector_EFT_addr) = 0x01040000;
		   	*(load_vector_EFT_addr) = 0x02040000;
		   	*(load_vector_EFT_addr) = 0x03040000;
                                    
		   	*(load_vector_EFT_addr) = 0x00050000;
		   	*(load_vector_EFT_addr) = 0x01050000;
		   	*(load_vector_EFT_addr) = 0x02050000;
		   	*(load_vector_EFT_addr) = 0x03050000;
                                    
		   	*(load_vector_EFT_addr) = 0x00060000;
		   	*(load_vector_EFT_addr) = 0x01060000;
		   	*(load_vector_EFT_addr) = 0x02060000;
		   	*(load_vector_EFT_addr) = 0x03060000;
                                    
		   	*(load_vector_EFT_addr) = 0x00070000;
		   	*(load_vector_EFT_addr) = 0x01070000;
		   	*(load_vector_EFT_addr) = 0x02070000;
		   	*(load_vector_EFT_addr) = 0x03070000;
                                    
		   	*(load_vector_EFT_addr) = 0x00080000;
		   	*(load_vector_EFT_addr) = 0x01080000;
		   	*(load_vector_EFT_addr) = 0x02080000;
		   	*(load_vector_EFT_addr) = 0x03080000;
                                    
		   	*(load_vector_EFT_addr) = 0x00090000;
		   	*(load_vector_EFT_addr) = 0x01090000;
		   	*(load_vector_EFT_addr) = 0x02090000;
		   	*(load_vector_EFT_addr) = 0x03090000;
		print("\n\n Termino Carga del vector de EFT \r\n\n");

		print("\n\n Comienza la Carga del vector de incrementos \r\n\n");
		   	*(load_vector_incrementos_tiempo_addr) = 0x00000001;
		   	*(load_vector_incrementos_tiempo_addr) = 0x00010001;
		   	*(load_vector_incrementos_tiempo_addr) = 0x00020001;
		   	*(load_vector_incrementos_tiempo_addr) = 0x00030001;
		   	*(load_vector_incrementos_tiempo_addr) = 0x00040001;
		   	*(load_vector_incrementos_tiempo_addr) = 0x00050001;
		   	*(load_vector_incrementos_tiempo_addr) = 0x00060001;
		   	*(load_vector_incrementos_tiempo_addr) = 0x00070001;
		   	*(load_vector_incrementos_tiempo_addr) = 0x00080001;
		   	*(load_vector_incrementos_tiempo_addr) = 0x00090001;
		print("\n\n Termino Carga del vector de de incrementos \r\n\n");

		print("\n\n Comienza la Carga del vector LFT \r\n\n");
		   	*(load_vector_LFT_addr) = 0x0000FFFF;
		   	*(load_vector_LFT_addr) = 0x0100FFFF;
		   	*(load_vector_LFT_addr) = 0x0200FFFF;
		   	*(load_vector_LFT_addr) = 0x0300FFFF;
                                    
		   	*(load_vector_LFT_addr) = 0x0001FFFF;
		   	*(load_vector_LFT_addr) = 0x0101FFFF;
		   	*(load_vector_LFT_addr) = 0x0201FFFF;
		   	*(load_vector_LFT_addr) = 0x0301FFFF;
                                    
		   	*(load_vector_LFT_addr) = 0x0002FFFF;
		   	*(load_vector_LFT_addr) = 0x0102FFFF;
		   	*(load_vector_LFT_addr) = 0x0202FFFF;
		   	*(load_vector_LFT_addr) = 0x0302FFFF;
                                    
		   	*(load_vector_LFT_addr) = 0x0003FFFF;
		   	*(load_vector_LFT_addr) = 0x0103FFFF;
		   	*(load_vector_LFT_addr) = 0x0203FFFF;
		   	*(load_vector_LFT_addr) = 0x0303FFFF;
                                    
		   	*(load_vector_LFT_addr) = 0x0004FFFF;
		   	*(load_vector_LFT_addr) = 0x0104FFFF;
		   	*(load_vector_LFT_addr) = 0x0204FFFF;
		   	*(load_vector_LFT_addr) = 0x0304FFFF;
                                    
		   	*(load_vector_LFT_addr) = 0x0005FFFF;
		   	*(load_vector_LFT_addr) = 0x0105FFFF;
		   	*(load_vector_LFT_addr) = 0x0205FFFF;
		   	*(load_vector_LFT_addr) = 0x0305FFFF;
                                    
		   	*(load_vector_LFT_addr) = 0x0006FFFF;
		   	*(load_vector_LFT_addr) = 0x0106FFFF;
		   	*(load_vector_LFT_addr) = 0x0206FFFF;
		   	*(load_vector_LFT_addr) = 0x0306FFFF;
                                    
		   	*(load_vector_LFT_addr) = 0x0007FFFF;
		   	*(load_vector_LFT_addr) = 0x0107FFFF;
		   	*(load_vector_LFT_addr) = 0x0207FFFF;
		   	*(load_vector_LFT_addr) = 0x0307FFFF;
                                    
		   	*(load_vector_LFT_addr) = 0x0008FFFF;
		   	*(load_vector_LFT_addr) = 0x0108FFFF;
		   	*(load_vector_LFT_addr) = 0x0208FFFF;
		   	*(load_vector_LFT_addr) = 0x0308FFFF;
                                    
		   	*(load_vector_LFT_addr) = 0x0009FFFF;
		   	*(load_vector_LFT_addr) = 0x0109FFFF;
		   	*(load_vector_LFT_addr) = 0x0209FFFF;
		   	*(load_vector_LFT_addr) = 0x0309FFFF;
		print("\n\n Termino Carga del vector de LFT \r\n\n");

		print("\n\n Comienza la Carga del vector de Marcado Inicial \r\n\n");
			*(m_marcado_addr) = 0x00000001;//mutex
			*(m_marcado_addr) = 0x01000005;//vacios
			*(m_marcado_addr) = 0x02000000;//llenos
			*(m_marcado_addr) = 0x03000001;//P3 - Productores esperando
			*(m_marcado_addr) = 0x04000000;//P4 - Produciendo
			*(m_marcado_addr) = 0x05000001;//P5 - Consumidores esperando
			*(m_marcado_addr) = 0x06000000;//P6 - COnsumiendo
			*(m_marcado_addr) = 0x07000000;
			*(m_marcado_addr) = 0x08000000;
			*(m_marcado_addr) = 0x09000000;
		print("\n\n Termino Carga del vector de Marcado Inicial \r\n\n");

    pthread_attr_t attr;

	#if SCHED_TYPE == SCHED_PRIO
    	struct sched_param spar;
	#endif

    pthread_attr_init (&attr);
	
	buffer = 0;
	
    //Creacion y lanzamiendo de los hilos sumadores
    	xmk_add_static_thread(consumidor, 0);
    	xmk_add_static_thread(productor, 0);

    /* Start the kernel */
     	xilkernel_start();

    /* Never reached */
       	cleanup_platform();

    return 0;
}

void* consumidor()
{
	int disparo_t_dos 	= 0x00020000;
	int disparo_t_tres 	= 0x00030000;
	char *nombre = "CONSUMIDOR";
	int disparos_ya_ejecutados;
	
	while (cantidad_de_acciones < 10)
	{
		//Pedir T2
			*(new_disparo_addr) = disparo_t_dos;
			*(error_addr) = 0xFFFFFFFF;
			
		//Ver si T2 se ejecuto
			disparos_ya_ejecutados = *(leer_disparos_ejecutados);
			while ( (disparos_ya_ejecutados & 0x00000004) != 0x00000004)
			{	disparos_ya_ejecutados = *(leer_disparos_ejecutados);	}
				
		//Sacar T2
			*(sacar_disparo_addr) = disparo_t_dos;
			*(error_addr) = 0x00000000;
		
		//Operar sobre el BUFFER
			buffer = buffer - 1;
			xil_printf ("Soy el hilo %s - Valor del buffer = %d\r\n", nombre, buffer);
		
		//Pedir T3
			*(new_disparo_addr) = disparo_t_tres;
			*(error_addr) = 0xFFFFFFFF;
		
		//Ver si T3 se ejecuto
			disparos_ya_ejecutados = *(leer_disparos_ejecutados);
			while ( (disparos_ya_ejecutados & 0x00000008) != 0x00000008)
			{	disparos_ya_ejecutados = *(leer_disparos_ejecutados);	}
			
		//Sacar T3
			*(sacar_disparo_addr) = disparo_t_tres;
			*(error_addr) = 0x00000000;
	}

	return NULL;
}

void* productor()
{
	int disparo_t_cero 	= 0x00000000;
    int disparo_t_uno 	= 0x00010000;
	char *nombre = "PRODUCTOR";
	int disparos_ya_ejecutados;

	while (cantidad_de_acciones < 10)
	{
		//Pedir T0
			*(new_disparo_addr) = disparo_t_cero;
			*(error_addr) = 0xFFFFFFFF;

		//Ver si T0 se ejecuto
			disparos_ya_ejecutados = *(leer_disparos_ejecutados);
			while ( (disparos_ya_ejecutados & 0x00000001) != 0x00000001)
			{	disparos_ya_ejecutados = *(leer_disparos_ejecutados);	}
				
		//Sacar T0
			*(sacar_disparo_addr) = disparo_t_cero;
			*(error_addr) = 0x00000000;
		
		//Operar sobre el BUFFER
			buffer = buffer + 1;
			xil_printf ("Soy el hilo %s - Valor del buffer = %d\r\n", nombre, buffer);

		//Pedir T1
			*(new_disparo_addr) = disparo_t_uno;
			*(error_addr) = 0xFFFFFFFF;
		
		//Ver si T1 se ejecuto
			disparos_ya_ejecutados = *(leer_disparos_ejecutados);
			while ( (disparos_ya_ejecutados & 0x00000002) != 0x00000002)
			{	disparos_ya_ejecutados = *(leer_disparos_ejecutados);	}
		
		//Sacar T1
			*(sacar_disparo_addr) = disparo_t_uno;
			*(error_addr) = 0x00000000;
	}

	return NULL;
}