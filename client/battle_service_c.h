/* BATTLE CLIENT HEADER */
/*
	Author: Gabriele Martino
	
	Il seguente codice contiene dichiarazioni di funzioni di servizio per la connessione col server
	e di gestione della connessione con un altro peer
	richiede l'inclusione di "battle_gioco.h" che dichiara funzioni utilizzare per il gioco vero e proprio

*/
#include "battle_gioco.h"

/* VARIABILI D'AMBIENTE */
int socket_client, fdmax, controll, socket_gioco;

fd_set read_fds, master;
short unsigned int locator;
/* DATI CLIENTE ATTUALE */
char* nome;
char* utente;
int porta;


/* STAMPA COMANDI PER IL SERVER */
void print_command();
/* GESTISCI CHIUSURA SOCKET E PROCESSO TRAMITE IL SEGNALE CTRL+C */
void gestisci();
/* CONNESSIONE AL SERVER */
void inizializza_client(char* server_ip,char* port);
/* AUTENTICAZIONE CLIENT CON IL SERVER */
void autenticazione();
/* RICHIESTA ELENCO CLIENT */
void richiesta_elenco_client();
/* RICHIESTA DI GIOCO DA PARTE DELL'UTENTE */
void richiesta_di_gioco();
/* DISCONNETTI DAL SERVER */
void disconnessione_dal_server();
/* RICHIESTA NOME UTENTE DALL'ALTRO CLIENT*/
void richiesta_dati_utente();
/* ACCETTAZIONE RICHIESTA E AVVIO DEL GIOCO */
void accetta_richiesta_avvia_gioco();
/* PULITURA BUFFER */
void clear();
