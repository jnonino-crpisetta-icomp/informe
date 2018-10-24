//---------------------------------------------------------------------
// Filename:          user_logic.v
// Version:           2.01.b
// Description:       User logic module.
// Date:              Wed Sep 12 20:32:03 2012 (by Create and Import P
//                    eripheral Wizard)
// Verilog Standard:  Verilog-2001
//---------------------------------------------------------------------
// Naming Conventions:
//   active low signals:                   "*_n"
//   clock signals:                        "clk", "clk_div#", "clk_#x"
//   reset signals:                        "rst", "rst_n"
//   generics:                             "C_*"
//   user defined types:                   "*_TYPE"
//   state machine next state:             "*_ns"
//   state machine current state:          "*_cs"
//   combinatorial signals:                "*_com"
//   pipelined or register delay signals:  "*_d#"
//   counter signals:                      "*cnt*"
//   clock enable signals:                 "*_ce"
//   internal version of output port:      "*_i"
//   device pins:                          "*_pin"
//   ports:                                "Names begin with Uppercase"
//   processes:                            "*_PROCESS"
//   component instantiations:             "<ENTITY_>I_<#|FUNC>"
//---------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
// Company: Universidad Nacional de Córdoba
// Engineer: 	Julián Nonino
//					Carlos Renzo Pisetta
// 
// Create Date:    15:45:21 06/01/2012 
// Design Name: 	 IP core Procesador de Petri
// Module Name:    user_logic.v 
// Project Name:   Procesador de Petri
// Target Devices: Digilent Atlys
// Tool versions:  Xilinx 14.2
// Description: 	 Procesador de Redes de Petri para sincronizar 
//                   procesos en hardware
//
// Dependencies: 
//
// Revision: 
// Revision: 
//		Revision 01 - Julián Nonino - Creacion de los paramteros y 
//								 	  entradas de datos para operar
//
//		Revision 02 - Julián Nonino - Creacion de los elementos internos
//									  para la operacion 
//
//		Revision 03 - Julián Nonino - Creacion de un modulo que realice 
//									  la suma entre el marcado inicial y cada columna 
//									  de la matriz de incidencia. A su vez, arma un 
//									  vector que define las tranciones posibles segun
//									  haya o no valores negativos.
//
// 		Revision 04 - Julián Nonino -Creacion del proceso que determina
//									 si la transicion produce un nuevo estado valido
//									 segun las cotas aplicadas a las plazas
//
// 		Revision 05 - Renzo Pisetta - Agregado de componente de carga de 
//									  datos, matriz de incidencia, inhibicion
//									  y marcado incial.
//
//		Revision 06 - Julián Nonino - Creacion de los modulos de 
//									  actualizacion de las colas de entrada y salida
//									  Modificacion de la parte de actualizacion de 
//									  marcado utilizando las colas de entrada y 
//									  salida.
//
//		Revision 07 - Renzo Pisetta - Implementacion de los modulo para 
//									  carga de datos cotas, plazas ilimitadas,
//									  transiciones automaticas.
//
//		Revision 08 - Julián Nonino - Implementacion del vector que 
//									  determina si una transicion esta sensibilidaza
//									  según los arcos inhibidores.
//
//		Revision 09 - Julián Nonino - Implementacion de automatizacion 
//									  de disparos, se creo un registro para marcar 
//									  las transiciones que se determinaron 
//									  automaticas.
//
//		Revision 10 - Julián Nonino - Implementacion de un bit que 
//									  informa si la red sigue activa o no
//
//		Revision 11 - Renzo Pisetta - Modificacion del modulo de carga 
//									  de datos. Matriz de incidencia, matriz de 
//									  inhibicion, marcado inicial, cota, plazas 
//									  ilimitadas.
//
//		Revision 12 - Julián Nonino - Eliminacion del registro plazas 
//									  ilimitadas. De ahora en adelante, si una plaza
//									  debería ser ilimitada, se le va a asignar el 
//									  valor maximo de cota.
//
//		Revision 13 - Renzo Pisetta - Implementar entrada de disparos. 
//									  Correccion de errores en colas.
//
//		Revision 14 - Julián Nonino - Agregado de la salida de disparos 
//									  y de una señal de ack para que desde 
//									  el exterior
//                    Renzo Pisetta - verifico si se realizo la 
//									  escritura deseada.
//
//		Revision 15 - Julián Nonino - Agrego un vector de transiciones 
//									  sensibilizadas, de esta manera, el vector de 
//									  disparos posibles queda para cuando la 
//									  transicion esta sensibilizada 
//									  y ademas, cumple con las restricciones 
//									  temporales.
//
//		Revision 16 - Julián Nonino - Creacion de los vectores EFT, LFT
//									  y cuenta de tiempo para la implementacion de 
//									  Redes de Petri con Tiempo
//
//		Revision 17 - Julián Nonino -Implementación de la actualización 
//									 de la marcas de tiempo.
//									 Implementación del vector de habilitacion segun
//									 marcas temporales
//
//		Revision 18 - Julián Nonino - Modificación del algoritmo de 
//									  actualizacion de marcas de tiempo.
//									  Ahora se actualiza en el bloque que actualiza
//									  el marcado.
//
//		Revision 19 - Julián Nonino - Modifique la carga de vectores de
//									  tiempo EFT y LFT para que permita que se 
//									  cargue un valor de 64 bits.
//
//		Revision 20 - Julián Nonino - Modificacion colas de entrada y 
//									  salida.
//
//		Revision 21 - Julián Nonino - Acomode el orden de las 
//									  direcciones para acceder a los datos.
//
//		Revision 22 - Julián Nonino - Realice una modificacion en la 
//									  actualización del marcado que resolvio el
//									  problema de los incrementos en las colas.
//
//		Revision 23 - Renzo Pisetta - Agregar Interrupciones.
//
//		Revision 24 - Renzo Pisetta -Resetear transiciones_automaticas,
//                                   vector_EFT,vector_LFT,
//									 vector_incremento_de_tiempo.
//
//		Revision 25 - Renzo Pisetta - Crear registro de salida con 
//									  numero de transicion que interrumpe.
//
//		Revision 26 - Julián Nonino - Modificada la generacion de la 
//									  señal red_activa
//
//		Revision 27 - Julián Nonino - Se crea este nuevo IP core que al 
//									  comienzo es identico al time_petri_axi
//									  por esta razon se mantienen todas las 
//									  revisiones anteriores. Este IP core se 
//									  modificara para implementar la resolucion
//									  de Redes de Petri Temporizadas.
//
//		Revision 28 - Julián Nonino - Modificacion de los registros 
//									  relacionados con el tiempo.
//									  Se elimino el vector LFT y se cambio el nombre
//									  del vector EFT a vector_duracion.
//									  Se modifico la logica de carga de datos.
//
//		Revision 29 - Julián Nonino - Modificacion de la logica de 
//									  ejecucion de la red para trbajar con Redes
//									  de Petri Temporizadas. 
//
//////////////////////////////////////////////////////////////////////////////////
`uselib lib=unisims_ver
`uselib lib=proc_common_v3_00_a

module user_logic
(
  // -- ADD USER PORTS BELOW THIS LINE ---------------
  // --USER ports added here 
  Interrupt,                      //Puerto de interrupion
  // -- ADD USER PORTS ABOVE THIS LINE ---------------

  // -- DO NOT EDIT BELOW THIS LINE ------------------
  // -- Bus protocol ports, do not add to or delete 
  Bus2IP_Clk,                     // Bus to IP clock
  Bus2IP_Resetn,                  // Bus to IP reset
  Bus2IP_Data,                    // Bus to IP data bus
  Bus2IP_BE,                      // Bus to IP byte enables
  Bus2IP_RdCE,                    // Bus to IP read chip enable
  Bus2IP_WrCE,                    // Bus to IP write chip enable
  IP2Bus_Data,                    // IP to Bus data bus
  IP2Bus_RdAck,                   // IP to Bus read transfer 
  //                                 acknowledgement
  IP2Bus_WrAck,                   // IP to Bus write transfer 
  //                                 acknowledgement
  IP2Bus_Error                    // IP to Bus error response
  // -- DO NOT EDIT ABOVE THIS LINE ------------------
); // user_logic
 
//Cantidad de plazas de la red de Petri modelada
	parameter cant_plazas                           = 10;
//Cantidad de transiciones de la red de Petri modelada		
	parameter cant_transiciones                     = 7;	
//Numero de Bits necesarios para representar el numero cant_transiciones	
	parameter bits_cant_transiciones                = 3;
// Tamano de los elementos de los arreglos marcado y resultados, medido 
// en bits
	parameter tamano_de_elementos                   = 16;
//Tamano de los elementos de las matrices de incidencia, medido en bits
	parameter tamano_de_elementos_incidencia        = 3; 	
//Tamano de contador del contador para entrada de disparos	
//Tamano de contador del contador para entrada de disparos	
	parameter tamano_cola                           = 5;	
// Cantidad de bits para expresar el numero de unidades en las que se 
// incrementea cada transicion	
	parameter tamano_vector_incremento_de_tiempo    = 5;	
// Tamano de los elementos de los arreglos de cuenta de tiempo, medido 
// en bits	
	parameter tamano_de_elementos_tiempo            = 48;	

parameter C_NUM_REG                      = 32;
parameter C_SLV_DWIDTH                   = 32;

output                                    Interrupt;

input                                     Bus2IP_Clk;
input                                     Bus2IP_Resetn;
input      [C_SLV_DWIDTH-1 : 0]           Bus2IP_Data;
input      [C_SLV_DWIDTH/8-1 : 0]         Bus2IP_BE;
input      [C_NUM_REG-1 : 0]              Bus2IP_RdCE;
input      [C_NUM_REG-1 : 0]              Bus2IP_WrCE;
output     [C_SLV_DWIDTH-1 : 0]           IP2Bus_Data;
output                                    IP2Bus_RdAck;
output                                    IP2Bus_WrAck;
output                                    IP2Bus_Error;

  wire       [31 : 0]                       slv_reg_write_sel;
  wire       [31 : 0]                       slv_reg_read_sel;
  reg        [C_SLV_DWIDTH-1 : 0]           slv_ip2bus_data;
  wire                                      slv_read_ack;
  wire                                      slv_write_ack;
  integer                                   byte_index, bit_index;

  assign
    slv_reg_write_sel = Bus2IP_WrCE[31:0],
    slv_reg_read_sel  = Bus2IP_RdCE[31:0],
    slv_read_ack      = Bus2IP_RdCE[0] || Bus2IP_RdCE[1] || 
          Bus2IP_RdCE[2]  || Bus2IP_RdCE[3]  || Bus2IP_RdCE[4]  || 
          Bus2IP_RdCE[5]  || Bus2IP_RdCE[6]  || Bus2IP_RdCE[7]  || 
          Bus2IP_RdCE[8]  || Bus2IP_RdCE[9]  || Bus2IP_RdCE[10] || 
          Bus2IP_RdCE[11] || Bus2IP_RdCE[12] || Bus2IP_RdCE[13] ||
          Bus2IP_RdCE[14] || Bus2IP_RdCE[15] || Bus2IP_RdCE[16] || 
          Bus2IP_RdCE[17] || Bus2IP_RdCE[18] || Bus2IP_RdCE[19] || 
          Bus2IP_RdCE[20] || Bus2IP_RdCE[21] || Bus2IP_RdCE[22] || 
          Bus2IP_RdCE[23] || Bus2IP_RdCE[24] || Bus2IP_RdCE[25] || 
          Bus2IP_RdCE[26] || Bus2IP_RdCE[27] || Bus2IP_RdCE[28] || 
          Bus2IP_RdCE[29] || Bus2IP_RdCE[30] || Bus2IP_RdCE[31];

// ------------------------------------------------------------
// Lectura de registros
// ------------------------------------------------------------

  always @( slv_reg_read_sel )
    begin 
      case ( slv_reg_read_sel )
        //Se usan para escribir datos, por eso aca no se leen
				//32'b10000000000000000000000000000000 : slv_ip2bus_data <= ;//00
				//32'b01000000000000000000000000000000 : slv_ip2bus_data <= ;//04
				//32'b00100000000000000000000000000000 : slv_ip2bus_data <= ;//08
				//32'b00010000000000000000000000000000 : slv_ip2bus_data <= ;//0C
				//32'b00001000000000000000000000000000 : slv_ip2bus_data <= ;//10
				//32'b00000100000000000000000000000000 : slv_ip2bus_data <= ;//14
				//32'b00000010000000000000000000000000 : slv_ip2bus_data <= ;//18
				//32'b00000001000000000000000000000000 : slv_ip2bus_data <= ;//1C
				//32'b00000000100000000000000000000000 : slv_ip2bus_data <= ;//20
				//32'b00000000010000000000000000000000 : slv_ip2bus_data <= ;//24
				//32'b00000000001000000000000000000000 : slv_ip2bus_data <= ;//28
				//32'b00000000000100000000000000000000 : slv_ip2bus_data <= ;//2C
				//32'b00000000000010000000000000000000 : slv_ip2bus_data <= ;//30
			//Usadas para leer
				32'b00000000000001000000000000000000 : slv_ip2bus_data <= disparos_en_espera;	//34;
				32'b00000000000000100000000000000000 : slv_ip2bus_data <= disparos_posibles;	//38;
				32'b00000000000000010000000000000000 : slv_ip2bus_data <= disparos_ya_ejecutados;	//3C;
				32'b00000000000000001000000000000000 : slv_ip2bus_data <= {{32-cant_transiciones{1'b0}},num_intr};	//40
				32'b00000000000000000100000000000000 : slv_ip2bus_data <= {{31{1'b0}},red_activa};	//44
				32'b00000000000000000010000000000000 : slv_ip2bus_data <= marcado[0];	//48			
				32'b00000000000000000001000000000000 : slv_ip2bus_data <= marcado[1];	//4C
				32'b00000000000000000000100000000000 : slv_ip2bus_data <= marcado[2];	//50
				32'b00000000000000000000010000000000 : slv_ip2bus_data <= marcado[3];	//54
				32'b00000000000000000000001000000000 : slv_ip2bus_data <= marcado[4];	//58
				32'b00000000000000000000000100000000 : slv_ip2bus_data <= marcado[5];	//5C
				32'b00000000000000000000000010000000 : slv_ip2bus_data <= marcado[6];	//60
				32'b00000000000000000000000001000000 : slv_ip2bus_data <= marcado[7];	//64
				32'b00000000000000000000000000100000 : slv_ip2bus_data <= marcado[8];	//68
				32'b00000000000000000000000000010000 : slv_ip2bus_data <= marcado[9];	//6C
				//32'b00000000000000000000000000001000 : slv_ip2bus_data <= ;//70
				//32'b00000000000000000000000000000100 : slv_ip2bus_data <= ;//74
				//32'b00000000000000000000000000000010 : slv_ip2bus_data <= ;//78
				//32'b00000000000000000000000000000001 : slv_ip2bus_data <= ;//7C
        default : slv_ip2bus_data <= 0;
      endcase

    end // SLAVE_REG_READ_PROC

// ------------------------------------------------------------
// IP to Bus signals
// ------------------------------------------------------------

  assign IP2Bus_Data = (slv_read_ack == 1'b1) ? slv_ip2bus_data :  0 ;
  assign IP2Bus_WrAck = slv_write_ack;
  assign IP2Bus_RdAck = slv_read_ack;
  assign IP2Bus_Error = 0;
  
///////////////////////////////////////////////////////////////////////
//																	 //
//			PROCESADOR DE REDES DE PETRI CON TIEMPO					 //
//																	 //
///////////////////////////////////////////////////////////////////////  
  
  
/*****REGISTROS NECESARIOS PARA LA EJECUCION DE LA RED DE PETRI*******/
  
	// Datos
		//Marcado
			reg [tamano_de_elementos-1:0] marcado [cant_plazas-1:0]/* synthesis syn_keep = 1 */;
			reg [tamano_de_elementos-1:0] marcado_inicial [cant_plazas-1:0]/* synthesis syn_keep = 1 */;
		//Matriz de incidencia positiva
			reg [tamano_de_elementos_incidencia-1:0] matriz_incidencia_positiva
			 [cant_plazas-1:0] [cant_transiciones-1:0]/* synthesis syn_keep = 1 */;	
		//Matriz de incidencia negativa
			reg [tamano_de_elementos_incidencia-1:0] matriz_incidencia_negativa 
			[cant_plazas-1:0] [cant_transiciones-1:0]/* synthesis syn_keep = 1 */;	
		//Arcos inhibidores
			reg matriz_inhibicion [cant_plazas-1:0] [cant_transiciones-1:0]/* synthesis syn_keep = 1 */;
		//Plazas acotadas
			reg [tamano_de_elementos-2:0] cotas_plazas [cant_plazas-1:0]/* synthesis syn_keep = 1 */;
		//Transiciones automaticas
			reg [cant_transiciones-1:0] transiciones_automaticas/* synthesis syn_keep = 1 */;
		//Redes de Petri con Tiempo
			reg [tamano_de_elementos_tiempo-1:0] vector_duracion 
			[cant_transiciones-1:0]/* synthesis syn_keep = 1 */;
			reg [tamano_vector_incremento_de_tiempo-1:0] vector_incremento_de_tiempo 
			[cant_transiciones-1:0]/* synthesis syn_keep = 1 */;
			reg [tamano_de_elementos_tiempo-1:0] vector_tiempo_sensibilizacion 
			[cant_transiciones-1:0]/* synthesis syn_keep = 1 */;	
		//Determinacion de disparos posibles
			reg [cant_transiciones-1:0] t_sensibilizadas/* synthesis syn_keep = 1 */;
			reg [cant_transiciones-1:0] disparos_posibles/* synthesis syn_keep = 1 */;
		//Mascara Interrupciones
			reg [cant_transiciones-1:0] intr_mask/* synthesis syn_keep = 1 */;
		//Transicion Interrupt
			reg[bits_cant_transiciones-1:0] num_intr/* synthesis syn_keep = 1 */;
		//Vector de prioridades
			reg [bits_cant_transiciones-1:0] vector_prioridades 
			[cant_transiciones-1:0];

	//Resultado ecuacion: marcado+matriz_incidencia_positiva-matriz_incidencia_negativa
		reg signed [tamano_de_elementos-1:0] resultado
		[cant_plazas-1:0][cant_transiciones-1:0]/* synthesis syn_keep = 1 */;
		//Registros Verificacion SIGNO
			reg [cant_plazas-1:0] sign_matrix 
			[cant_transiciones-1:0] /* synthesis syn_keep = 1 */;
			reg [cant_transiciones-1:0] t_enable_sign/* synthesis syn_keep = 1 */;
		//Registros Verificacion INHIBICIÓN
			reg [cant_plazas-1:0] inhibe_matrix 
			[cant_transiciones-1:0] /* synthesis syn_keep = 1 */;
			reg [cant_transiciones-1:0] t_enable_inhibicion/* synthesis syn_keep = 1 */;	
		//Registros Verificacion COTAS
			reg [cant_plazas-1:0] limit_matrix 
			[cant_transiciones-1:0]/* synthesis syn_keep = 1 */;
			reg [cant_transiciones-1:0] t_enable_limit/* synthesis syn_keep = 1 */;
		//Registros Verificacion TIEMPO
			reg [cant_transiciones-1:0] t_enable_time/* synthesis syn_keep = 1 */;
		//Registros Mascara semi ejecucion
			reg [cant_transiciones-1:0] mask_semi_ejecucion/* synthesis syn_keep = 1 */;

	//Señales de salida
		//Señal de carga de datos
			reg carga/* synthesis syn_keep = 1 */;

/*************DETERMINACION DE LOS NUEVOS POSIBLES ESTADOS************/

	//Determinacion de la matriz "resultado".	
	integer columnas;
	integer filas;
	always @(posedge Bus2IP_Clk)
		begin
			for (columnas=0;columnas<cant_transiciones;columnas=columnas+1)
			 //Recorro columnas
				begin
					for (filas=0;filas<cant_plazas;filas=filas+1)
					// Recorro filas
					begin
						resultado[filas][columnas] = marcado[filas] + 
						matriz_incidencia_positiva[filas][columnas] - 
						matriz_incidencia_negativa[filas][columnas]; //Ecuacion
						// Verificacion habilitacion por signo - Creacion de matriz
							sign_matrix [columnas][filas] = 
							resultado[filas][columnas][tamano_de_elementos-1];
						//Verificacion habilitacion por inhibicion - Creacion de matriz
							inhibe_matrix [columnas][filas] = 
							(|marcado[filas]) & matriz_inhibicion[filas][columnas];
						// Verificacion de habilitacion por cotas - Creacion de matriz
							if (resultado[filas]
							[columnas][tamano_de_elementos-1]==1'b0 &&
							 resultado[filas][columnas] > cotas_plazas[filas]) // Verifico si supero la cota
								begin
									limit_matrix [columnas][filas] = 1'b0; // Cota SI superada
								end
							else  limit_matrix [columnas][filas] = 1'b1; // Cota NO superada 
						end //Fin for recorre filas
				end //Fin for recorre columnas
		end
	
/***********DETERMINACION DE LOS VECTORES DE HABILITACION************/
	
	integer columnas_habilitaciones;
	integer k;
	always @(*)	
		begin
			for (columnas_habilitaciones=0 ; columnas_habilitaciones<cant_transiciones ; columnas_habilitaciones=columnas_habilitaciones+1) //Recorro columnas
				begin
/*SIGNO*/
	//Determinacion del vector de habilitacion de transiciones segun el "signo"
					t_enable_sign [columnas_habilitaciones] = 
					~(|(sign_matrix[columnas_habilitaciones][cant_plazas-1:0]));
					
/*ARCOS INHIBIDORES*/
	//Determinacion del vector de habilitacion de transicones segun los "arcos inhibidores"
				t_enable_inhibicion [columnas_habilitaciones] =
				~(|(inhibe_matrix[columnas_habilitaciones][cant_plazas-1:0]));
					
/*COTAS*/
	//Determinacion del vector de habilitacion de transicones segun las "cotas de plazas"
					t_enable_limit [columnas_habilitaciones] =
					&(limit_matrix[columnas_habilitaciones][cant_plazas-1:0]);
				
				end			
/*REDES TEMPORALES*/		
 	//Determinacion del vector de habilitacion de transiciones segun las "marcas temporales"
				for (k=0 ; k<cant_transiciones ; k=k+1) //Recorro transiciones
					begin
						if (vector_tiempo_sensibilizacion[k]>=vector_duracion[k]) t_enable_time[k] = 1'b1;
						else t_enable_time[k] = 1'b0;
					end
			
			//Determinacion de las transciones habilitadas, es decir, disparos posibles
				t_sensibilizadas = t_enable_sign & t_enable_inhibicion & t_enable_limit;
				disparos_posibles = t_enable_sign & t_enable_inhibicion & t_enable_limit & t_enable_time;
						
		end //FIN de always @(*)
		
/****************************SEÑAN DE RED ACTIVA**********************/

	reg red_activa;
	always @(*)
		begin
			red_activa = |disparos_posibles;
		end
	
/*************ACTUALIZACION DEL LOS VECTORES DE ESTADO DE*************/
/********************LAS COLAS DE ENTRADA Y SALIDA********************/

	reg [cant_transiciones-1:0] disparos_en_espera;
	reg [cant_transiciones-1:0] colas_entrada_llenas;
	reg [cant_transiciones-1:0] disparos_ya_ejecutados;	
	reg [cant_transiciones-1:0] colas_salida_llenas;
	reg [cant_transiciones-1:0] colas_salida_vacias;
	integer indice;
	always@(*)
		begin
			for(indice=0 ; indice<cant_transiciones ; indice=indice+1)
				begin
					disparos_en_espera[indice]     =
					 |counter_cola_disparos_entrada[indice];
					colas_entrada_llenas[indice]   =
					 &counter_cola_disparos_entrada[indice];
					disparos_ya_ejecutados[indice] =
					 |counter_cola_disparos_salida[indice];
					colas_salida_llenas[indice]    =
					 &counter_cola_disparos_salida[indice];
					colas_salida_vacias[indice]    =
					 ~|counter_cola_disparos_salida[indice];
				end
		end

/************************COLA DE ENTRADA DE DISPAROS******************/
		
	reg [cant_transiciones-1:0] disparo_entrada_incrementa;
	reg [cant_transiciones-1:0] disparo_entrada_decrementa;
	reg [tamano_cola-1:0] counter_cola_disparos_entrada 
	    [cant_transiciones-1:0]/* synthesis syn_keep = 1 */;
	integer index_entrada;
	always @(posedge Bus2IP_Clk)
		begin
			if(Bus2IP_Resetn==1'b0)
				begin
					for (index_entrada=0 ; index_entrada<cant_transiciones ; index_entrada=index_entrada+1)
						begin
							counter_cola_disparos_entrada[index_entrada]<=
							{tamano_cola{1'b0}};
						end
				end
		else	
			for (index_entrada=0 ; index_entrada<cant_transiciones ; index_entrada=index_entrada+1)
				begin
					//Incremento cola entrada
						if ({disparo_entrada_incrementa[index_entrada] , disparo_entrada_decrementa[index_entrada]} == 2'b10) 
						//Debe incrementar 
							begin
								counter_cola_disparos_entrada[index_entrada] <= 
								 counter_cola_disparos_entrada[index_entrada] + 1'b1;
							end //FIN if(disparo_entrante[index_entrada] == 1'b1)
					//Decremento cola entrada	
						else if ({disparo_entrada_incrementa[index_entrada] , disparo_entrada_decrementa[index_entrada]} == 2'b01) 
						//Debe decrementar  
							begin
								counter_cola_disparos_entrada[index_entrada] <= 
								 counter_cola_disparos_entrada[index_entrada] - 1'b1;
							end //FIN if(disparo_dec_entrante[index_entrada] == 1'b1)
					//Otras opciones
						else
							begin
								counter_cola_disparos_entrada[index_entrada] <= 
								 counter_cola_disparos_entrada[index_entrada];
							end
				end //FIN for (index_entrada=0 ; index_entrada<cant_transiciones ; index_entrada=index_entrada+1)
		end //FIN always

/*********************COLA DE SALIDA DE DISPAROS**********************/

	reg [tamano_cola-1:0] counter_cola_disparos_salida [cant_transiciones-1:0]/* synthesis syn_keep = 1 */;
	reg [cant_transiciones-1:0] disparo_salida_incrementa;
	reg [cant_transiciones-1:0] disparo_salida_decrementa;
	integer index_salida;
	always @(posedge Bus2IP_Clk)
		begin
			if(Bus2IP_Resetn==1'b0)
				begin
					for (index_salida=0 ; index_salida<cant_transiciones ; index_salida=index_salida+1)
						begin
							counter_cola_disparos_salida[index_salida]<=
							 {tamano_cola{1'b0}};
						end
				end
		else	
			for (index_salida=0 ; index_salida<cant_transiciones ; index_salida=index_salida+1)
				begin
					//Incremento cola salida
						if ({disparo_salida_incrementa[index_salida] , disparo_salida_decrementa[index_salida]} == 2'b10) //Debe incrementar 
							begin
								counter_cola_disparos_salida[index_salida] <= counter_cola_disparos_salida[index_salida] + 1'b1;
							end //FIN if(disparo_salida_decrementa[index_salida] == 1'b1)
					//Decremento cola salida	
						else if ({disparo_salida_incrementa[index_salida] , disparo_salida_decrementa[index_salida]} == 2'b01) //Debe decrementar 
							begin
								counter_cola_disparos_salida[index_salida] <= counter_cola_disparos_salida[index_salida] - 1'b1;//{tamano_cola{1'b1}};
							end //FIN if(disparo_salida_decrementa[index_salida] == 1'b1)
					//Otras opciones
						else 
							begin
								counter_cola_disparos_salida[index_salida] <= counter_cola_disparos_salida[index_salida];
							end
					end //FIN for (index_salida=0 ; index_salida<cant_transiciones ; index_salida=index_salida+1)
		end //FIN always

/***********************ACTUALIZACION DEL MARCADO*********************/

	integer index2;
	integer index3;
	integer index4;
	integer index5;
	always @(negedge Bus2IP_Clk)
		begin
			if(carga==1'b1)	//Proceso de carga
				begin
					mask_semi_ejecucion <= {tamano_de_elementos_tiempo{1'b1}};
					for (index4=0 ; index4<cant_plazas ; index4=index4+1)
						begin
							marcado[index4] <= marcado_inicial[index4];
						end
					for (index5=0 ; index5<cant_transiciones ; index5=index5+1)
						begin
							vector_tiempo_sensibilizacion[index5] <=
							 {tamano_de_elementos_tiempo{1'b0}};
						end
				end	//Fin if(carga==1'b1)
			else //Ejecucion de la red
				begin
					for(index2=0 ; index2<cant_transiciones ; index2=index2+1)
					begin
						//Semi disparo, extraer los tokens de las plazas de entrada a la transicion
						if ((transiciones_automaticas[index2] |
						disparos_en_espera[index2]) & t_sensibilizadas[index2] == 1'b1)
							begin
								//Actualizo el marcado, saco los tokens de las plazas de entrada
									for (index3=0 ; index3<cant_plazas ; index3=index3+1)
										begin					
											marcado[index3] <= marcado[index3] - matriz_incidencia_negativa[index3][index2];
										end
										
								//Incremento el contador de la cola de disparos de salida correspondiete
									disparo_salida_incrementa <=
									 {cant_transiciones{1'b0}};
									
								//Decremento el contador de la cola de disparos en espera correspondiente
									disparo_entrada_decrementa <= 
									{cant_transiciones{1'b0}};
									if (disparos_en_espera[index2] == 1'b1) disparo_entrada_decrementa[index2] <=
									 1'b1;	
									else disparo_entrada_decrementa[index2] <= 1'b0;								
					//Vuelvo a habilitar la transicion
						mask_semi_ejecucion[index2] <= 1'b0;
				end
			else if (colas_salida_llenas[index2]==1'b0 & 
			mask_semi_ejecucion[index2] == 1'b0 & 
			vector_tiempo_sensibilizacion[index2] >= 
			vector_duracion[index2]) 
			// Veo si el disparo en espera esta en las transiciones sensibilizadas
				begin
			//Actualizo el marcado, pongo los tokens en las plazas de salida
					for (index3=0 ; index3<cant_plazas ; index3=index3+1)
						begin					
							marcado[index3] <= marcado[index3] + matriz_incidencia_positiva[index3][index2];
						end
						
			//Incremento el contador de la cola de disparos de salida correspondiete
					disparo_salida_incrementa <= {cant_transiciones{1'b0}};
					disparo_salida_incrementa[index2] <= 1'b1;
					
			//Reseteo el vector de decremento del contador de la cola de disparos en espera
					disparo_entrada_decrementa <= {cant_transiciones{1'b0}};
					
			//Reseteo la marca de tiempo de la transicion que se disparo
					vector_tiempo_sensibilizacion[index2] <= 
					{tamano_de_elementos_tiempo{1'b0}};
									
			//Vuelvo a habilitar la transicion
					mask_semi_ejecucion[index2] <= 1'b1;
							
				end //Fin if ( disparos_en_espera[index2] & disparos_posibles[index2] == 1'b1)
		else //NO hay disparo en espera para esa transicion o el disparo aun no es posible
			begin
				disparo_entrada_decrementa[index2] 	<= 1'b0;
				disparo_salida_incrementa[index2] 	<= 1'b0;
								
			//Actualizo marcas temporales
				if (mask_semi_ejecucion[index2] == 1'b1) vector_tiempo_sensibilizacion[index2] <= 
				{tamano_de_elementos_tiempo{1'b0}};
				if (mask_semi_ejecucion[index2] == 1'b0) //Transicion sensibilizada
					begin
						if (vector_tiempo_sensibilizacion[index2]+vector_incremento_de_tiempo[index2] >=
						 {tamano_de_elementos_tiempo{1'b1}} )
							begin
								vector_tiempo_sensibilizacion[index2] <= 
								{tamano_de_elementos_tiempo{1'b1}};
							end
						else
							begin
								vector_tiempo_sensibilizacion[index2] <= 
								vector_tiempo_sensibilizacion[index2]+
								vector_incremento_de_tiempo[index2];
							end
					end
				end //FIN else no hay disparo en espera para esa transicion o el disparo aun no es posible
					end //Fin for
			end //Fin else
	end //Fin always

/************************GENERADOR DE INTERRUPCIONES******************/

	reg [1:0]Interrupt_reg;
	assign Interrupt=(!Interrupt_reg[1])&Interrupt_reg[0];
	integer columnas_intr;
	wire [cant_transiciones-1:0]intr_activadas;
	assign intr_activadas=disparos_ya_ejecutados & intr_mask;
	
	always@(negedge Bus2IP_Clk)
		begin
			if (Bus2IP_Resetn==1'b0)
				begin
					num_intr<=0;
					Interrupt_reg<=2'b00;
				end
			else
				begin
					Interrupt_reg<=
					{Interrupt_reg[0],(|(disparos_ya_ejecutados & 
					intr_mask))&!intr_reset};
				//prioridad transicion menor
					for (columnas_intr=cant_transiciones-1 ; columnas_intr>=0 ; columnas_intr=columnas_intr-1)
						begin
							if(intr_activadas[columnas_intr]==1'b1)	
							num_intr<=columnas_intr;
						end
				end
		end

/****************************CARGA DE DATOS**********************/

	wire ack_escritura;
	assign slv_write_ack = ack_escritura_reg;
	
//BUS (se debe cumplir: tamano_bus=bits_bus_plazas+
//                      bits_bus_transiciones+bits_bus_elemento)

	//Tamano del Bus de datos
		localparam tamano_bus				= C_SLV_DWIDTH;
	//Tamano del Bus de direcciones	
		localparam tamano_bus_address		= C_NUM_REG;
	// Cantidad de bits del dato del bus dedicado a indicar 
//	   la fila de la matriz		
		localparam bits_bus_plazas			= 8;		
	//Cantidad de bits del dato del bus dedicado a indicar 
//	  la columna de la matriz		
		localparam bits_bus_transiciones	= 8;				
	//Cantidad de bits del dato del bus dedicado a indicar el elemento	
		localparam bits_bus_elementos		= 16;				
 
	//Definicion de los posibles address para escritura
		localparam m_marcado    					=
		 32'b10000000000000000000000000000000;//00
		localparam m_incidencia_positiva			=
		 32'b01000000000000000000000000000000;//04
		localparam m_incidencia_negativa			=
		 32'b00100000000000000000000000000000;//08
		localparam m_inhibicion 					=
		 32'b00010000000000000000000000000000;//0C
		localparam p_cotas							=
		 32'b00001000000000000000000000000000;//10
		localparam t_automatica 					=
		 32'b00000100000000000000000000000000;//14
		localparam load_vector_duracion				=
		 32'b00000010000000000000000000000000;//18
		localparam load_vector_incrementos_tiempo	=
		 32'b00000001000000000000000000000000;//1C		
		localparam t_intr							=
		 32'b00000000100000000000000000000000;//20
		localparam v_prioridades					=
		 32'b00000000010000000000000000000000;//24
		localparam new_disparo  					=
		 32'b00000000001000000000000000000000;//28
		localparam sacar_disparo					=
		 32'b00000000000100000000000000000000;//2C
		localparam error							=
		 32'b00000000000010000000000000000000;//30
	
	reg ack_escritura_reg;
	reg intr_reset;
	integer filas_reset;
	integer columnas_reset;
	always@(negedge Bus2IP_Clk, negedge Bus2IP_Resetn)
		begin
			if (Bus2IP_Resetn==1'b0)
				begin//Reseteo de variables
					disparo_entrada_incrementa <={cant_transiciones{1'b0}};
					transiciones_automaticas <={cant_transiciones{1'b0}};
					intr_mask <={cant_transiciones{1'b0}};
					intr_reset<=0;
					carga<=1;
					for (filas_reset=0;filas_reset<cant_plazas;
					   filas_reset=filas_reset+1)
					begin
					marcado_inicial[filas_reset] 							<= {tamano_de_elementos{1'b0}};
					cotas_plazas [filas_reset] 							<= {tamano_de_elementos{1'b0}};
						for (columnas_reset=0 ; columnas_reset<cant_transiciones ; columnas_reset=columnas_reset+1)
							begin
								matriz_incidencia_positiva
								[filas_reset][columnas_reset] 	<=
								 {tamano_de_elementos{1'b0}};
								matriz_incidencia_negativa
								[filas_reset][columnas_reset] 	<=
								 {tamano_de_elementos{1'b0}};
								matriz_inhibicion 
								[filas_reset][columnas_reset] <= 1'b0;
								vector_duracion [columnas_reset]<=
								{tamano_de_elementos_tiempo{1'b0}};//Vector tiempo minimo
								vector_incremento_de_tiempo 
								[columnas_reset]<=1'b1;
							end //FIN for (filas_reset=0 ; filas_reset<cant_plazas ; filas_reset=filas_reset+1)
						end //FIN for (columnas_reset=0 ; columnas_reset<cant_transiciones ; columnas_reset=columnas_reset+1)
				end //Fin if(Bus2IP_Resetn==1'b0)
		else //No es Bus2IP_Resetn
			begin
				case (slv_reg_write_sel)
					m_marcado: //Carga del marcado inicial
						begin
							carga<=1'b1;
							intr_reset<=0;
							marcado_inicial 
							[Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]]<=
							Bus2IP_Data[bits_bus_elementos-1:0];
							disparo_entrada_incrementa <= {cant_transiciones{1'b0}};
							disparo_salida_decrementa <= {cant_transiciones{1'b0}};								
							ack_escritura_reg = 1'b1;
						end //FIN del caso m_marcado
					m_incidencia: //Carga de la matriz de incidencia
						begin
							carga<=1'b1;
							intr_reset<=0;
							matriz_incidencia 
							[Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]]
							[Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]] <= 
							Bus2IP_Data[bits_bus_elementos-1:0];
							disparo_entrada_incrementa <= 
							{cant_transiciones{1'b0}};
							disparo_salida_decrementa <= 
							{cant_transiciones{1'b0}};
							ack_escritura_reg = 1'b1;	
						end //FIN del caso m_incidencia
					m_inhibicion: //Carga de la matriz de inhibicion
						begin
							carga<=1'b1;
							intr_reset<=0;
							matriz_inhibicion 
							[Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]]
							[Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]] <= 
							 Bus2IP_Data[0];
							disparo_entrada_incrementa <=
							 {cant_transiciones{1'b0}};
							disparo_salida_decrementa <=
							 {cant_transiciones{1'b0}};
							ack_escritura_reg = 1'b1;							
						end	//FIN del caso m_inhibicion	
					p_cotas: //Carga del vector de cotas
						begin
							carga<=1'b1;
							intr_reset<=0;
							cotas_plazas 
							[Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]] <=
							Bus2IP_Data[bits_bus_elementos-2:0];
							disparo_entrada_incrementa <=
							{cant_transiciones{1'b0}};
							disparo_salida_decrementa <= 
							{cant_transiciones{1'b0}};
							ack_escritura_reg = 1'b1;
						end //FIN del caso p_cotas
		t_automatica:
			begin
				carga<=1'b1;
				intr_reset<=0;
				transiciones_automaticas
				[Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]]<=
				Bus2IP_Data[0];//generar vector sacar de aca
				disparo_entrada_incrementa <=
				{cant_transiciones{1'b0}};
				disparo_salida_decrementa <= 
				{cant_transiciones{1'b0}};
				ack_escritura_reg = 1'b1;
			end //FIN del caso t_automatica
		load_vector_duracion:
			begin
				carga<=1'b1;
				intr_reset<=0;
				if (Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]==
				8'b00000000) //Parte menos significativa de duracion
					begin
						vector_duracion [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]]
						[15:0]<=Bus2IP_Data[bits_bus_elementos-1:0];
					end
				else if (Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]==8'b00000001)
					begin
						vector_duracion [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]]
						[31:16]<=Bus2IP_Data[bits_bus_elementos-1:0];
					end
				else if (Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]==8'b00000010)//Parte mas significativa de duracion
					begin
						vector_duracion [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]]
						[47:32]<=Bus2IP_Data[bits_bus_elementos-1:0];
					end
//				else if (Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]==8'b00000011)//Parte mas significativa de duracion
//					begin
//						vector_duracion [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]]
//		[63:48]<=Bus2IP_Data[bits_bus_elementos-1:0];
//					end
//				else if (Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]==8'b00000100)//Parte mas significativa de duracion
//					begin
//						vector_duracion [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]]
//		[79:64]<=Bus2IP_Data[bits_bus_elementos-1:0];
//					end
//				else if (Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]==8'b00000101)//Parte mas significativa de duracion
//					begin
//						vector_duracion [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]]
//		[95:80]<=Bus2IP_Data[bits_bus_elementos-1:0];
//					end
//				else if (Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]==8'b00000110)//Parte mas significativa de duracion
//					begin
//						vector_duracion [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]]
//		[111:81]<=Bus2IP_Data[bits_bus_elementos-1:0];
//					end
//				else if (Bus2IP_Data[tamano_bus-1:tamano_bus-bits_bus_plazas]==8'b00000111)//Parte mas significativa de duracion
//					begin
//						vector_duracion [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]]
//		[127:112]<=Bus2IP_Data[bits_bus_elementos-1:0];
//					end
				disparo_entrada_incrementa <= {cant_transiciones{1'b0}};
				disparo_salida_decrementa <= {cant_transiciones{1'b0}};
				ack_escritura_reg = 1'b1;
			end
		load_vector_incrementos_tiempo:
			begin
				carga<=1'b1;
				intr_reset<=0;
				vector_incremento_de_tiempo [Bus2IP_Data
				[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-
				bits_bus_transiciones]]<=Bus2IP_Data[bits_bus_elementos-1:0];//generar vector sacar de aca
				disparo_entrada_incrementa <= {cant_transiciones{1'b0}};
				disparo_salida_decrementa <= {cant_transiciones{1'b0}};
				ack_escritura_reg = 1'b1;
			end
		t_intr:
			begin
				carga<=1'b1;
				intr_reset<=0;
				intr_mask [Bus2IP_Data
				[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-
				bits_bus_transiciones]]<=Bus2IP_Data[0];
				disparo_entrada_incrementa <={cant_transiciones{1'b0}};
				disparo_salida_decrementa <= {cant_transiciones{1'b0}};
				ack_escritura_reg = 1'b1;
			end
		new_disparo://prepara el vector de nuevo disparo y sube el flag de incremento
			begin
				carga<=1'b0;
				intr_reset<=0;
			//Verificar si la cola esta llena o no
				if(colas_entrada_llenas
				  [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-
				  bits_bus_plazas-bits_bus_transiciones]]==1'b0) //Cola NO llena
				begin
					disparo_entrada_incrementa <= {cant_transiciones{1'b0}};
					disparo_entrada_incrementa [Bus2IP_Data[tamano_bus-bits_bus_plazas-1:tamano_bus-bits_bus_plazas-bits_bus_transiciones]] <= 1'b1;//generar vector sacar de aca
					disparo_salida_decrementa <= {cant_transiciones{1'b0}};
					ack_escritura_reg = 1'b1;
				end
			else //Cola LLENA
				begin
					disparo_entrada_incrementa <={cant_transiciones{1'b0}};
					disparo_salida_decrementa <= {cant_transiciones{1'b0}};
					ack_escritura_reg = 1'b0;
					end
			end //FIN del caso new_disparo
		sacar_disparo:
			begin
				carga<=1'b0;
			//Verificar si el disparo se encuentra en la cola de salida
			if (colas_salida_vacias
			[Bus2IP_Data[tamano_bus-bits_bus_plazas-1:
			tamano_bus-bits_bus_plazas-bits_bus_transiciones]]==1'b0 ) 
			// La cola del disparo que se quiere sacar no esta vacia
				begin
					disparo_salida_decrementa <= {cant_transiciones{1'b0}};
					disparo_salida_decrementa 
					[Bus2IP_Data[tamano_bus-bits_bus_plazas-1:
					tamano_bus-bits_bus_plazas-bits_bus_transiciones]] <= 1'b1;
					disparo_entrada_incrementa <={cant_transiciones{1'b0}};
					ack_escritura_reg = 1'b1;
				if(intr_mask[Bus2IP_Data
					[tamano_bus-bits_bus_plazas-1:tamano_bus-
					bits_bus_plazas-bits_bus_transiciones]]==1'b1)
						intr_reset<=1;
					else	intr_reset<=0;	
				end
			else //Cola vacia - El disparo que se quiere sacar NO fue ejecutado aun
				begin
					disparo_salida_decrementa <= {cant_transiciones{1'b0}};
					disparo_entrada_incrementa <={cant_transiciones{1'b0}};
					ack_escritura_reg = 1'b0;
					intr_reset<=0;	
				end
			end //FIN del caso sacar_disparo
		default:
			begin
				carga<=1'b0;
				intr_reset<=0;
				disparo_entrada_incrementa <={cant_transiciones{1'b0}};
				disparo_salida_decrementa <= {cant_transiciones{1'b0}};
				ack_escritura_reg = 1'b0;							
			end //FIN del caso default
	 endcase	//FIN	del case	
	end //FIN del else	
end	//FIN always@(negedge Bus2IP_Clk, posedge Bus2IP_Resetn)  
		
endmodule
