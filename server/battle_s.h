/* DICHIARAZIONI DELLE FUNZIONI DI SERVIZIO Server*/
/*
	Author: Gabriele Martino
	
	Il seguente codice contiene le dichiarazione nelle funzioni utilizzate in 
		battle_server.c

*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

/* Struttura che conterrà le informazioni dei nuovi client */
struct elem{
	char* nome;
	short unsigned int state;
	int porta;
	int socket;
	uint32_t my_ip;
	struct elem* next;
};
struct elem* client;

/* Variabili per l'I/O Multiplexing */
int fdmax;
fd_set read_fds, master;

/* socket di ascolto, variabile per il checking, variabile per il nuovo socket provvisoria */
int listener, controll, client_socket;
socklen_t cl_len;
/* Struttura per l'indirizzo del server, struttura provvisoria per contenere le informazioni del nuovo client (provvisoria perchè verrano poi inserite nella struttura apposita sopra definita)*/
struct sockaddr_in server_addr, client_addr;

/* STAMPA CLIENT + PORTA UDP */
void stampa_client();

/* INSERISCI CLIENT IN LISTA */
void inserisci_client(char* nome, int dim, int porta, int sock, uint32_t ip_address);

/* RICERCA CLIENT PER SOCKET */
struct elem* cerca_client2(int sock);//torna un puntatore all'elemento se trovato, 0 altrimenti

/* RICERCA CLIENT PER NOME */
struct elem* cerca_client(char* nome);//torna un puntatore all'elemento se trovato, 0 altrimenti

/* CONTA CLIENT CONNESSI */
int num_client();

/* ELIMINA SOCKET E CLIENT PER SOCKET */
void elimina_socket_client2(int socket);

/* ERROR CHECK */
short controlla_errore(int controll,int* socket);// torna 1 se errore, 0 altrimenti

/* CHIUDE TUTTI I SOCKET CONNESSI */
void chiudi_tutti_socket();

/* GESTISCE LA CHIUSURA DEL SERVER CON IL SEGNALE CTRL+C */
void gestisci();

/* INIZIALIZZA SERVER */
void inizializza(int porta);

/* INSERISCI NUOVO CLIENT */
void setta_nuovo_client();

/* INVIO ELENCO CLIENT */
void invia_client(int i);

/* CHECK PER AVVIO DEL GIOCO */
void richiesta_gioco(int i);

/* AVVIO DEL GIOCO */
void avvio_gioco(int i,struct elem* utente);

