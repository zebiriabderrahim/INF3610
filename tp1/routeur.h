/*
 * router.h
 *
 *  Created on: 26 July 2020
 *      Author: Guy BOIS
 */

#ifndef SRC_ROUTEUR_H_EXT_
#define SRC_ROUTEUR_H_EXT_

#include <os.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define COMPUTE_CRC(packet) computeCRC((uint16_t*)packet,sizeof(*packet))
#define _COMPUTE_CRC(packet) _computeCRC((uint16_t*)packet,sizeof(*packet))
#define TASK_STK_SIZE 8192

/* ************************************************
 *                TASK PRIOS
 **************************************************/

#define          TaskGeneratePRIO   			15
#define			 TaskStatsPRIO 					12
#define          TaskComputingPRIO  			21
#define          TaskFIFOForwardingPRIO 		22
#define          TaskOutputPortPRIO     		20
#define          TaskResetPRIO     				14
#define          TaskStopPRIO     				13


#define			WAITFORComputing				5
#define			NB_CRC_TO_MATCH					8500


#define 		 FULL_TRACE 					0


// Routing info.

#define NB_OUTPUT_PORTS 3
#define NB_FIFO 3

#define INT1_LOW      0x00000000
#define INT1_HIGH     0x3FFFFFFF
#define INT2_LOW      0x40000000
#define INT2_HIGH     0x7FFFFFFF
#define INT3_LOW      0x80000000
#define INT3_HIGH     0xBFFFFFFF
#define INT_BC_LOW    0xC0000000
#define INT_BC_HIGH   0xFFFFFFFF

// Reject source info.
#define REJECT_LOW1   0x10000000
#define REJECT_HIGH1  0x17FFFFFF
#define REJECT_LOW2   0x50000000
#define REJECT_HIGH2  0x57FFFFFF
#define REJECT_LOW3   0xD0000000
#define REJECT_HIGH3  0xD7FFFFFF

// Evenements lies aux taches
#define TASK_GENERATE_RDY  		0x01
#define TASK_COMPUTING_RDY  	0x02
#define TASK_FORWARDING_RDY  	0x04
#define TASK_OUTPUTPORT_RDY  	0x08
#define TASK_STATS_RDY  		0x10

// Mask
#define TASKS_ROUTER			0x1F     // Permet de demarrer ou stopper toutes les taches au meme moment

OS_FLAG_GRP RouterStatus;

OS_SEM	Sem;

typedef struct{
	int id;
	char* name;
} Info_Port;

typedef struct{
	int id;
	char* name;
} Info_FIFO;

Info_Port  	Port[NB_OUTPUT_PORTS];
Info_FIFO 	FIFO[NB_FIFO];

typedef enum {
	PACKET_VIDEO,
	PACKET_AUDIO,
	PACKET_AUTRE,
	NB_PACKET_TYPE
} PACKET_TYPE;

typedef struct {
    unsigned int src;
    unsigned int dst;
    PACKET_TYPE type;
	unsigned int crc;
    unsigned int data[13];
} Packet;

// Stacks
static CPU_STK TaskGenerateSTK[TASK_STK_SIZE];
static CPU_STK TaskComputingSTK[TASK_STK_SIZE];
static CPU_STK TaskFIFOForwardingSTK[NB_FIFO][TASK_STK_SIZE];
static CPU_STK TaskOutputPortSTK[NB_OUTPUT_PORTS][TASK_STK_SIZE];
static CPU_STK TaskStatsSTK[TASK_STK_SIZE];
static CPU_STK TaskResetSTK[TASK_STK_SIZE];
static CPU_STK TaskStopSTK[TASK_STK_SIZE];
static CPU_STK StartupTaskStk[TASK_STK_SIZE];



static OS_TCB TaskGenerateTCB;
static OS_TCB TaskStatsTCB;
static OS_TCB TaskComputingTCB;
static OS_TCB TaskFIFOForwardingTCB[NB_FIFO];
static OS_TCB TaskOutputPortTCB[NB_OUTPUT_PORTS];
static OS_TCB TaskResetTCB;
static OS_TCB TaskStopTCB;
static OS_TCB StartupTaskTCB;



/* ************************************************
 *                  Queues
 **************************************************/

OS_Q source_errQ;
OS_Q crc_errQ;

/* ************************************************
 *                  Semaphores
 **************************************************/

// Pas de sémaphore pour la partie 1 

/* ************************************************
 *                  Mutexes
 **************************************************/
OS_MUTEX mutRejete;
OS_MUTEX mutPrint;
OS_MUTEX mutAlloc;
OS_MUTEX mutPrint;


/*DECLARATION DES COMPTEURS POUR STATISTIQUES*/
int nbPacketCrees = 0;								// Nb de packets total crees
int nbPacketTraites = 0;							// Nb de paquets envoyés sur une interface
int nbPacketSourceRejeteTotal = 0;					// Nb de packets total rejetés pour mauvaise source
int nbPacketSourceRejete = 0;						// Nb de packets rejeté pour mauvaise source pour 30 sec
int nbPacketCRCRejete = 0;							// Nb de packets total rejetés pour mauvaise CRC
int nbPacketCRCRejeteTotal = 0;						// Nb de packets total rejetés total pour mauvaise CRC
CPU_TS64 nb_CPU_tick_CRC_total = 0;					// Nb de tick CPU (CPU_TS) passés dans computeCRC
int nb_calls_crc = 0;								// Nb d'appels à computeCRC
int packet_rejete_fifo_pleine_inputQ = 0;			// Utilisation de la fifo d'entrée
int packet_rejete_output_port_plein = 0;			// Utilisation des MB
int packet_rejete_fifo_pleine_Q = 0;
int delai_pour_vider_les_fifos_sec = 2;
int delai_pour_vider_les_fifos_msec = 0;
int print_paquets_rejetes = 0;
int limite_de_paquets= 20000;

/* ************************************************
 *              TASK PROTOTYPES
 **************************************************/

void TaskGenerate(void *data); 
void TaskComputing(void *data);
void TaskFIFOForwarding(void *data);
void TaskOutputPort(void *data);
void TaskStats(void* data);
void StartupTask(void *data);

void dispatch_packet (Packet* packet);

void create_application();
int create_tasks();
int create_events();
void err_msg(char* ,uint8_t);
unsigned int computeCRC(uint16_t* w, int nleft);
unsigned int _computeCRC(uint16_t* w, int nleft);

#endif /* SRC_ROUTEUR_H_EXT_ */
