/* BATTLE CLIENT HEADER */
/*
	Author: Gabriele Martino
	
	IL seguente codice contiene le dichiarazioni delle funzioni utilizzate del gioco vero e proprio

*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#define STDIN 0
/* VARIABILI D'AMBIENTE */
int socket_gioco;
struct  sockaddr_in gioco_addr;
/* AVVIAMENTO GIOCO */
void avvia_gioco(int porta_utente, uint32_t ip_utente,int turn);
