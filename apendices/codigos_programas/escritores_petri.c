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
#include "xtmrctr_l.h"

/* Declarations */
#define valor_maximo 10


#define numero_de_hilo_uno		1
#define numero_de_hilo_dos		2
#define numero_de_hilo_tres 	3
#define numero_de_hilo_cuatro	4
#define numero_de_hilo_cinco	5

#define disparo_t_cero 	 0x00000000
#define disparo_t_uno 	 0x00010000
#define disparo_t_dos 	 0x00020000
#define disparo_t_tres 	 0x00030000
#define disparo_t_cuatro 0x00040000
#define disparo_t_cinco  0x00050000
#define disparo_t_seis 	 0x00060000
#define disparo_t_siete  0x00070000
#define disparo_t_ocho 	 0x00080000
#define disparo_t_nueve  0x00090000

#define comprobacion_t_cero  	0x00000001
#define comprobacion_t_uno   	0x00000002
#define comprobacion_t_dos   	0x00000004
#define comprobacion_t_tres  	0x00000008
#define comprobacion_t_cuatro	0x00000010
#define comprobacion_t_cinco  0x00000020
#define comprobacion_t_seis   0x00000040
#define comprobacion_t_siete  0x00000080
#define comprobacion_t_ocho		0x00000100
#define comprobacion_t_nueve  0x00000200

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

void* master_thread(void *);
void* hilo_escritor_1();
void* hilo_escritor_2();
void* hilo_escritor_3();
void* hilo_escritor_4();
void* hilo_escritor_5();

/* Data */
#define cantidad_plazas 15
#define cantidad_transiciones 10
#define cantidad_palabras_vector_tiempo 3
int cuentas=0;

/* Functions */
int main()
{
    init_platform();
    xil_printf("INICIO EJECUCION CON PETRI\r\n");
    xil_printf("CINCO HILOS, Valor Maximo = %d \r\n\n",valor_maximo);

	print("Comienza la Carga de la Matriz de Incidencia\r\n");
		*(m_incidencia_addr) = 0x0000FFFF;
		*(m_incidencia_addr) = 0x00010000;
		*(m_incidencia_addr) = 0x00020000;
		*(m_incidencia_addr) = 0x00030000;
		*(m_incidencia_addr) = 0x00040000;
		*(m_incidencia_addr) = 0x00050000;
		*(m_incidencia_addr) = 0x00060000;
		*(m_incidencia_addr) = 0x00070000;
		*(m_incidencia_addr) = 0x00080000;
		*(m_incidencia_addr) = 0x00090001;
                             
		*(m_incidencia_addr) = 0x01000000;
		*(m_incidencia_addr) = 0x01010001;
		*(m_incidencia_addr) = 0x0102FFFF;
		*(m_incidencia_addr) = 0x01030000;
		*(m_incidencia_addr) = 0x01040000;
		*(m_incidencia_addr) = 0x01050000;
		*(m_incidencia_addr) = 0x01060000;
		*(m_incidencia_addr) = 0x01070000;
		*(m_incidencia_addr) = 0x01080000;
		*(m_incidencia_addr) = 0x01090000;
                             
		*(m_incidencia_addr) = 0x0200FFFF;
		*(m_incidencia_addr) = 0x02010001;
		*(m_incidencia_addr) = 0x02020000;
		*(m_incidencia_addr) = 0x02030000;
		*(m_incidencia_addr) = 0x02040000;
		*(m_incidencia_addr) = 0x02050000;
		*(m_incidencia_addr) = 0x02060000;
		*(m_incidencia_addr) = 0x02070000;
		*(m_incidencia_addr) = 0x02080000;
		*(m_incidencia_addr) = 0x02090000;
                             
		*(m_incidencia_addr) = 0x03000001;
		*(m_incidencia_addr) = 0x0301FFFF;
		*(m_incidencia_addr) = 0x03020000;
		*(m_incidencia_addr) = 0x03030000;
		*(m_incidencia_addr) = 0x03040000;
		*(m_incidencia_addr) = 0x03050000;
		*(m_incidencia_addr) = 0x03060000;
		*(m_incidencia_addr) = 0x03070000;
		*(m_incidencia_addr) = 0x03080000;
		*(m_incidencia_addr) = 0x03090000;
                             
		*(m_incidencia_addr) = 0x04000000;
		*(m_incidencia_addr) = 0x04010000;
		*(m_incidencia_addr) = 0x0402FFFF;
		*(m_incidencia_addr) = 0x04030001;
		*(m_incidencia_addr) = 0x04040000;
		*(m_incidencia_addr) = 0x04050000;
		*(m_incidencia_addr) = 0x04060000;
		*(m_incidencia_addr) = 0x04070000;
		*(m_incidencia_addr) = 0x04080000;
		*(m_incidencia_addr) = 0x04090000;
                             
		*(m_incidencia_addr) = 0x05000000;
		*(m_incidencia_addr) = 0x05010000;
		*(m_incidencia_addr) = 0x05020001;
		*(m_incidencia_addr) = 0x0503FFFF;
		*(m_incidencia_addr) = 0x05040000;
		*(m_incidencia_addr) = 0x05050000;
		*(m_incidencia_addr) = 0x05060000;
		*(m_incidencia_addr) = 0x05070000;
		*(m_incidencia_addr) = 0x05080000;
		*(m_incidencia_addr) = 0x05090000;
                             
		*(m_incidencia_addr) = 0x06000000;
		*(m_incidencia_addr) = 0x06010000;
		*(m_incidencia_addr) = 0x06020000;
		*(m_incidencia_addr) = 0x06030001;
		*(m_incidencia_addr) = 0x0604FFFF;
		*(m_incidencia_addr) = 0x06050000;
		*(m_incidencia_addr) = 0x06060000;
		*(m_incidencia_addr) = 0x06070000;
		*(m_incidencia_addr) = 0x06080000;
		*(m_incidencia_addr) = 0x06090000;
                             
		*(m_incidencia_addr) = 0x07000000;
		*(m_incidencia_addr) = 0x07010000;
		*(m_incidencia_addr) = 0x07020000;
		*(m_incidencia_addr) = 0x07030000;
		*(m_incidencia_addr) = 0x0704FFFF;
		*(m_incidencia_addr) = 0x07050001;
		*(m_incidencia_addr) = 0x07060000;
		*(m_incidencia_addr) = 0x07070000;
		*(m_incidencia_addr) = 0x07080000;
		*(m_incidencia_addr) = 0x07090000;
                             
		*(m_incidencia_addr) = 0x08000000;
		*(m_incidencia_addr) = 0x08010000;
		*(m_incidencia_addr) = 0x08020000;
		*(m_incidencia_addr) = 0x08030000;
		*(m_incidencia_addr) = 0x08040001;
		*(m_incidencia_addr) = 0x0805FFFF;
		*(m_incidencia_addr) = 0x08060000;
		*(m_incidencia_addr) = 0x08070000;
		*(m_incidencia_addr) = 0x08080000;
		*(m_incidencia_addr) = 0x08090000;
                             
		*(m_incidencia_addr) = 0x09000000;
		*(m_incidencia_addr) = 0x09010000;
		*(m_incidencia_addr) = 0x09020000;
		*(m_incidencia_addr) = 0x09030000;
		*(m_incidencia_addr) = 0x09040000;
		*(m_incidencia_addr) = 0x09050001;
		*(m_incidencia_addr) = 0x0906FFFF;
		*(m_incidencia_addr) = 0x09070000;
		*(m_incidencia_addr) = 0x09080000;
		*(m_incidencia_addr) = 0x09090000;
                             
		*(m_incidencia_addr)  = 0x0A000000;
		*(m_incidencia_addr)  = 0x0A010000;
		*(m_incidencia_addr)  = 0x0A020000;
		*(m_incidencia_addr)  = 0x0A030000;
		*(m_incidencia_addr)  = 0x0A040000;
		*(m_incidencia_addr)  = 0x0A050000;
		*(m_incidencia_addr)  = 0x0A06FFFF;
		*(m_incidencia_addr)  = 0x0A070001;
		*(m_incidencia_addr)  = 0x0A080000;
		*(m_incidencia_addr)  = 0x0A090000;
                             
		*(m_incidencia_addr)  = 0x0B000000;
		*(m_incidencia_addr)  = 0x0B010000;
		*(m_incidencia_addr)  = 0x0B020000;
		*(m_incidencia_addr)  = 0x0B030000;
		*(m_incidencia_addr)  = 0x0B040000;
		*(m_incidencia_addr)  = 0x0B050000;
		*(m_incidencia_addr)  = 0x0B060001;
		*(m_incidencia_addr)  = 0x0B07FFFF;
		*(m_incidencia_addr)  = 0x0B080000;
		*(m_incidencia_addr)  = 0x0B090000;
                             
		*(m_incidencia_addr)  = 0x0C000000;
		*(m_incidencia_addr)  = 0x0C010000;
		*(m_incidencia_addr)  = 0x0C020000;
		*(m_incidencia_addr)  = 0x0C030000;
		*(m_incidencia_addr)  = 0x0C040000;
		*(m_incidencia_addr)  = 0x0C050000;
		*(m_incidencia_addr)  = 0x0C060000;
		*(m_incidencia_addr)  = 0x0C070000;
		*(m_incidencia_addr)  = 0x0C08FFFF;
		*(m_incidencia_addr)  = 0x0C090001;
                             
		*(m_incidencia_addr)  = 0x0D000000;
		*(m_incidencia_addr)  = 0x0D010000;
		*(m_incidencia_addr)  = 0x0D020000;
		*(m_incidencia_addr)  = 0x0D030000;
		*(m_incidencia_addr)  = 0x0D040000;
		*(m_incidencia_addr)  = 0x0D050000;
		*(m_incidencia_addr)  = 0x0D060000;
		*(m_incidencia_addr)  = 0x0D070000;
		*(m_incidencia_addr)  = 0x0D080001;
		*(m_incidencia_addr)  = 0x0D09FFFF;
                             
		*(m_incidencia_addr)  = 0x0E000000;
		*(m_incidencia_addr)  = 0x0E010000;
		*(m_incidencia_addr)  = 0x0E020000;
		*(m_incidencia_addr)  = 0x0E030000;
		*(m_incidencia_addr)  = 0x0E040000;
		*(m_incidencia_addr)  = 0x0E050000;
		*(m_incidencia_addr)  = 0x0E060000;
		*(m_incidencia_addr)  = 0x0E070001;
		*(m_incidencia_addr)  = 0x0E08FFFF;
		*(m_incidencia_addr)  = 0x0E090000;
	print("Termino Carga de la Matriz de Incidencia \r\n\n");

	print("Comienza la Carga de la Matriz de Inhibicion\r\n");
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
                             
		*(m_inhibicion_addr)  = 0x0A000000;
		*(m_inhibicion_addr)  = 0x0A010000;
		*(m_inhibicion_addr)  = 0x0A020000;
		*(m_inhibicion_addr)  = 0x0A030000;
		*(m_inhibicion_addr)  = 0x0A040000;
		*(m_inhibicion_addr)  = 0x0A050000;
		*(m_inhibicion_addr)  = 0x0A060000;
		*(m_inhibicion_addr)  = 0x0A070000;
		*(m_inhibicion_addr)  = 0x0A080000;
		*(m_inhibicion_addr)  = 0x0A090000;
                             
		*(m_inhibicion_addr)  = 0x0B000000;
		*(m_inhibicion_addr)  = 0x0B010000;
		*(m_inhibicion_addr)  = 0x0B020000;
		*(m_inhibicion_addr)  = 0x0B030000;
		*(m_inhibicion_addr)  = 0x0B040000;
		*(m_inhibicion_addr)  = 0x0B050000;
		*(m_inhibicion_addr)  = 0x0B060000;
		*(m_inhibicion_addr)  = 0x0B070000;
		*(m_inhibicion_addr)  = 0x0B080000;
		*(m_inhibicion_addr)  = 0x0B090000;
                             
		*(m_inhibicion_addr)  = 0x0C000000;
		*(m_inhibicion_addr)  = 0x0C010000;
		*(m_inhibicion_addr)  = 0x0C020000;
		*(m_inhibicion_addr)  = 0x0C030000;
		*(m_inhibicion_addr)  = 0x0C040000;
		*(m_inhibicion_addr)  = 0x0C050000;
		*(m_inhibicion_addr)  = 0x0C060000;
		*(m_inhibicion_addr)  = 0x0C070000;
		*(m_inhibicion_addr)  = 0x0C080000;
		*(m_inhibicion_addr)  = 0x0C090000;
                             
		*(m_inhibicion_addr)  = 0x0D000000;
		*(m_inhibicion_addr)  = 0x0D010000;
		*(m_inhibicion_addr)  = 0x0D020000;
		*(m_inhibicion_addr)  = 0x0D030000;
		*(m_inhibicion_addr)  = 0x0D040000;
		*(m_inhibicion_addr)  = 0x0D050000;
		*(m_inhibicion_addr)  = 0x0D060000;
		*(m_inhibicion_addr)  = 0x0D070000;
		*(m_inhibicion_addr)  = 0x0D080000;
		*(m_inhibicion_addr)  = 0x0D090000;
                             
		*(m_inhibicion_addr)  = 0x0E000000;
		*(m_inhibicion_addr)  = 0x0E010000;
		*(m_inhibicion_addr)  = 0x0E020000;
		*(m_inhibicion_addr)  = 0x0E030000;
		*(m_inhibicion_addr)  = 0x0E040000;
		*(m_inhibicion_addr)  = 0x0E050000;
		*(m_inhibicion_addr)  = 0x0E060000;
		*(m_inhibicion_addr)  = 0x0E070000;
		*(m_inhibicion_addr)  = 0x0E080000;
		*(m_inhibicion_addr)  = 0x0E090000;
	print("Termino Carga de la Matriz de Inhibicion\r\n\n");

	print("Comienza la Carga del vector de Cotas de Plazas \r\n");
		*(p_cotas_addr) = 0x00000001;
		*(p_cotas_addr) = 0x01000001;
		*(p_cotas_addr) = 0x02000001;
		*(p_cotas_addr) = 0x03000001;
		*(p_cotas_addr) = 0x04000001;
		*(p_cotas_addr) = 0x05000001;
		*(p_cotas_addr) = 0x06000001;
		*(p_cotas_addr) = 0x07000001;
		*(p_cotas_addr) = 0x08000001;
		*(p_cotas_addr) = 0x09000001;
		*(p_cotas_addr) = 0x0A000001;
		*(p_cotas_addr) = 0x0B000001;
		*(p_cotas_addr) = 0x0C000001;
		*(p_cotas_addr) = 0x0D000001;
		*(p_cotas_addr) = 0x0E000001;
	print("Termino Carga del vector de Cotas de Plazas\r\n\n");

	print("Comienza la Carga del vector de Transiciones Automaticas\r\n");
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
	print("Termino Carga del vector de Transiciones Automaticas\r\n\n");

	print("Comienza la Carga del vector EFT\r\n");
		*(load_vector_EFT_addr) = 0x00000000;
		*(load_vector_EFT_addr) = 0x01000000;
		*(load_vector_EFT_addr) = 0x02000000;
                                
		*(load_vector_EFT_addr) = 0x00010000;
		*(load_vector_EFT_addr) = 0x01010000;
		*(load_vector_EFT_addr) = 0x02010000;
                                
		*(load_vector_EFT_addr) = 0x00020000;
		*(load_vector_EFT_addr) = 0x01020000;
		*(load_vector_EFT_addr) = 0x02020000;
                                
		*(load_vector_EFT_addr) = 0x00030000;
		*(load_vector_EFT_addr) = 0x01030000;
		*(load_vector_EFT_addr) = 0x02030000;
                                
		*(load_vector_EFT_addr) = 0x00040000;
		*(load_vector_EFT_addr) = 0x01040000;
		*(load_vector_EFT_addr) = 0x02040000;
                                
		*(load_vector_EFT_addr) = 0x00050000;
		*(load_vector_EFT_addr) = 0x01050000;
		*(load_vector_EFT_addr) = 0x02050000;
                                
		*(load_vector_EFT_addr) = 0x00060000;
		*(load_vector_EFT_addr) = 0x01060000;
		*(load_vector_EFT_addr) = 0x02060000;
                                
		*(load_vector_EFT_addr) = 0x00070000;
		*(load_vector_EFT_addr) = 0x01070000;
		*(load_vector_EFT_addr) = 0x02070000;
                                
		*(load_vector_EFT_addr) = 0x00080000;
		*(load_vector_EFT_addr) = 0x01080000;
		*(load_vector_EFT_addr) = 0x02080000;
                                
		*(load_vector_EFT_addr) = 0x00090000;
		*(load_vector_EFT_addr) = 0x01090000;
		*(load_vector_EFT_addr) = 0x02090000;
	print("Termino Carga del vector de EFT\r\n\n");

	print("Comienza la Carga del vector de incrementos\r\n");
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
	print("Termino Carga del vector de de incrementos\r\n\n");

	print("Comienza la Carga del vector LFT\r\n");
		*(load_vector_LFT_addr) = 0x0000FFFF;
		*(load_vector_LFT_addr) = 0x0100FFFF;
		*(load_vector_LFT_addr) = 0x0200FFFF;
                                
		*(load_vector_LFT_addr) = 0x0001FFFF;
		*(load_vector_LFT_addr) = 0x0101FFFF;
		*(load_vector_LFT_addr) = 0x0201FFFF;
                                
		*(load_vector_LFT_addr) = 0x0002FFFF;
		*(load_vector_LFT_addr) = 0x0102FFFF;
		*(load_vector_LFT_addr) = 0x0202FFFF;
                                
		*(load_vector_LFT_addr) = 0x0003FFFF;
		*(load_vector_LFT_addr) = 0x0103FFFF;
		*(load_vector_LFT_addr) = 0x0203FFFF;
                                
		*(load_vector_LFT_addr) = 0x0004FFFF;
		*(load_vector_LFT_addr) = 0x0104FFFF;
		*(load_vector_LFT_addr) = 0x0204FFFF;
                                
		*(load_vector_LFT_addr) = 0x0005FFFF;
		*(load_vector_LFT_addr) = 0x0105FFFF;
		*(load_vector_LFT_addr) = 0x0205FFFF;
                                
		*(load_vector_LFT_addr) = 0x0006FFFF;
		*(load_vector_LFT_addr) = 0x0106FFFF;
		*(load_vector_LFT_addr) = 0x0206FFFF;
                                
		*(load_vector_LFT_addr) = 0x0007FFFF;
		*(load_vector_LFT_addr) = 0x0107FFFF;
		*(load_vector_LFT_addr) = 0x0207FFFF;
                                
		*(load_vector_LFT_addr) = 0x0008FFFF;
		*(load_vector_LFT_addr) = 0x0108FFFF;
		*(load_vector_LFT_addr) = 0x0208FFFF;
                                
		*(load_vector_LFT_addr) = 0x0009FFFF;
		*(load_vector_LFT_addr) = 0x0109FFFF;
		*(load_vector_LFT_addr) = 0x0209FFFF;
	print("Termino Carga del vector de LFT\r\n\n");

	print("Comienza la Carga del vector de Marcado Inicial\r\n");
		*(m_marcado_addr) = 0x00000001;
		*(m_marcado_addr) = 0x01000000;
		*(m_marcado_addr) = 0x02000001;
		*(m_marcado_addr) = 0x03000000;
		*(m_marcado_addr) = 0x04000001;
		*(m_marcado_addr) = 0x05000000;
		*(m_marcado_addr) = 0x06000000;
		*(m_marcado_addr) = 0x07000001;
		*(m_marcado_addr) = 0x08000000;
		*(m_marcado_addr) = 0x09000000;
		*(m_marcado_addr) = 0x0A000001;
		*(m_marcado_addr) = 0x0B000000;
		*(m_marcado_addr) = 0x0C000001;
		*(m_marcado_addr) = 0x0D000000;
		*(m_marcado_addr) = 0x0E000000;
	print("Termino Carga del vector de Marcado Inicial\r\n\n");

	print("Comienza la Carga del vector de Transiciones con Interrupcion\r\n");
		*(t_intr_addr) = 0x00000000;
		*(t_intr_addr) = 0x00010000;
		*(t_intr_addr) = 0x00020000;
		*(t_intr_addr) = 0x00030000;
		*(t_intr_addr) = 0x00040000;
		*(t_intr_addr) = 0x00050000;
		*(t_intr_addr) = 0x00060000;
		*(t_intr_addr) = 0x00070000;
		*(t_intr_addr) = 0x00080000;
		*(t_intr_addr) = 0x00090000;
	print("Termino Carga del vector de Transiciones con Interrupcion\r\n");

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
	pthread_t hilo_1;
	pthread_t hilo_2;
	pthread_t hilo_3;
	pthread_t hilo_4;
	pthread_t hilo_5;
    pthread_attr_t attr;
  	pthread_attr_init (&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int *a;
   	int *b;
	int *c;
	int *d;
	int *e;

    int status;
    unsigned int valor_inicial_timer;
    int ControlStatus;
   	unsigned int valor_final_timer;
		
    //Iniciar cuenta de tiempo
    	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR,0,0x0);
       	XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_1_BASEADDR, 0, 0x00000000);
       	XTmrCtr_LoadTimerCounterReg	(XPAR_AXI_TIMER_1_BASEADDR,0);
       	ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR,0);
       	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR, 0, ControlStatus & (~XTC_CSR_LOAD_MASK));
       	valor_inicial_timer = XTmrCtr_GetTimerCounterReg(XPAR_AXI_TIMER_1_BASEADDR,0);
       	xil_printf("Valor inicial del timer: %d\r\n\n", valor_inicial_timer);
       	XTmrCtr_Enable(XPAR_AXI_TIMER_1_BASEADDR, 0);

		status = pthread_create(&hilo_1, &attr, (void*)hilo_escritor_1, NULL);
		//if (status != 0)	{  	xil_printf ("ERROR LANZANDO HILO ESCRITOR UNO");	}
		status = pthread_create(&hilo_2, &attr, (void*)hilo_escritor_2, NULL);
		//if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO ESCRITOR DOS");	}
		status = pthread_create(&hilo_3, &attr, (void*)hilo_escritor_3, NULL);
		//if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO ESCRITOR TRES");	}
		status = pthread_create(&hilo_4, &attr, (void*)hilo_escritor_4, NULL);
		//if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO ESCRITOR CUATRO");	}
		status = pthread_create(&hilo_5, &attr, (void*)hilo_escritor_5, NULL);
		//if (status != 0)	{	xil_printf ("ERROR LANZANDO HILO ESCRITOR CINCO");	}
	
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

void* hilo_escritor_1()
{
	while(cuentas<valor_maximo)
	{
		//Pedir T0
			*(new_disparo_addr) = disparo_t_cero;

		//Ver si T0 se ejecuto
			while ( !(*(leer_disparos_ejecutados) & comprobacion_t_cero) )
			{yield();}

		//Sacar T0 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_cero;

		//Escribir
			cuentas=cuentas+1;

		//Solicitar el disparo de T1
			*(new_disparo_addr) = disparo_t_uno;

		//Ver si T1 se ejecuto
			while ( !(*(leer_disparos_ejecutados) & comprobacion_t_uno) )
			{yield();}

		//Sacar T1 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_uno;

	}//Fin del while de variable compartida

	pthread_exit((void*)cuentas);
	return NULL;
}

void* hilo_escritor_2()
{
	while(cuentas<valor_maximo)
	{
		//Pedir T2
			*(new_disparo_addr) = disparo_t_dos;

		//Ver si T2 se ejecuto
			while ( !(*(leer_disparos_ejecutados) & comprobacion_t_dos) )
			{yield();}
			
		//Sacar T2 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_dos;

		//Escribir	
			cuentas=cuentas+1;

		//Solicitar el disparo de T3
			*(new_disparo_addr) = disparo_t_tres;

		//Ver si T3 se ejecuto
			while ( !(*(leer_disparos_ejecutados) & comprobacion_t_tres) )
			{yield();}

		//Sacar T3 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_tres;

	}//Fin del while de variable

	pthread_exit((void*)cuentas);
	return NULL;
}

void* hilo_escritor_3(void *c)
{
	while(cuentas<valor_maximo)
	{
		//Pedir T4
			*(new_disparo_addr) = disparo_t_cuatro;

		//Ver si T4 se ejecuto
			while ( !( *(leer_disparos_ejecutados) & comprobacion_t_cuatro) )
			{yield();}
			
		//Sacar T4 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_cuatro;
		
		//Escribir	
			cuentas=cuentas+1;

		//Solicitar el disparo de T5
			*(new_disparo_addr) = disparo_t_cinco;

		//Ver si T5 se ejecuto
			while ( !( *(leer_disparos_ejecutados) & comprobacion_t_cinco) )
			{yield();}
			
		//Sacar T5 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_cinco;

	}//Fin del while de variable

	pthread_exit((void*)cuentas);
	return NULL;
}

void* hilo_escritor_4(void *d)
{
	while(cuentas<valor_maximo)
	{
		//Pedir T6
			*(new_disparo_addr) = disparo_t_seis;

		//Ver si T6 se ejecuto
			while ( !( *(leer_disparos_ejecutados) & comprobacion_t_seis) )
			{yield();}
			
		//Sacar T6 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_seis;

		//Escribir	
			cuentas=cuentas+1;

		//Solicitar el disparo de T7
			*(new_disparo_addr) = disparo_t_siete;

		//Ver si T7 se ejecuto
			while ( !( *(leer_disparos_ejecutados) & comprobacion_t_siete) )
			{yield();}
			
		//Sacar T7 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_siete;
	}//Fin del while de variable

	pthread_exit((void*)cuentas);
	return NULL;
}

void* hilo_escritor_5(void *e)
{
	while(cuentas<valor_maximo)
	{
		//Pedir T8
			*(new_disparo_addr) = disparo_t_ocho;

		//Ver si T8 se ejecuto
			while ( !( *(leer_disparos_ejecutados) & comprobacion_t_ocho) )
			{yield();}
			
		//Sacar T8 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_ocho;

		//Escribir	
			cuentas=cuentas+1;

		//Solicitar el disparo de T9
			*(new_disparo_addr) = disparo_t_nueve;

		//Ver si T9 se ejecuto
			while ( !( *(leer_disparos_ejecutados) & comprobacion_t_nueve) )
			{yield();}
			
		//Sacar T9 de los disparos ejecutados
			*(sacar_disparo_addr) = disparo_t_nueve;
	}//Fin del while de variable

	pthread_exit((void*)cuentas);
	return NULL;
}