/* BATTLE CLIENT */
/*
	Author: Gabriele Martino
	
	IL seguente codice contiene il main del client 
	con inizializzazione della connessione col server
	Richiede funzioni di servizio dichiarate in "battle_service_c.h"

*/
#include "battle_service_c.h"
/*
	VARIABILI DEL CLIENT
	int socket_client, fdmax, controll;
	fd_set read_fds, master;

	char* nome,utente;
	int porta;
	struct nave* flotta;

*/

int main(int argc, char* argv[]){
	if(argc <=2){
		printf("Inserire <INDIRIZZO IP SERVER> <PORTA SU CUI È IN ASCOLTO>. Riesegui.\n");
		exit(1);
	}
	int i;
	char comando[20];
	utente = (char*)malloc(20);

	/* variabile che mi indica se sono in gioco o meno; utile per eliminare i socket in modo sicuro in caso di errore o uscita improvvisa */
	locator = 0;
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, gestisci);

	inizializza_client(argv[1],argv[2]);//alla fine della funzione sarà attivo un socket tcp con il server
	
	autenticazione();//dopo la connessione ci si registra sul server con username e porta udp che si vuole utilizzare
	print_command();

	/* Inizializzazione set variabili per l'I/O multiplexing */
	FD_ZERO(&read_fds);
	FD_ZERO(&master);
	FD_SET(socket_client, &master); 
	FD_SET(STDIN,&master);
	fdmax = socket_client;
	/* AVVIO ATTESA DI COMANDI PER IL SERVER */
	while(1){
		printf(">");  
		fflush(stdout);
		read_fds = master;
		select(fdmax+1, &read_fds,NULL,NULL,NULL);
		for(i = 0;i<=fdmax;i++){
			if(FD_ISSET(i,&read_fds)){
				/* gestione comandi */
				if(i == STDIN){
					scanf("%s",comando); 
					if(!strcmp(comando,"!help")){
						clear();
						print_command();

					}else if(!strcmp(comando,"!who")){
						clear();
						richiesta_elenco_client();

					}else if(!strcmp(comando,"!connect")){
						scanf("%s",utente);
						if(!strcmp(utente,nome)){
							printf("Non puoi inviare la richiesta a te stesso.\n");
							break;
						}
						clear();
						richiesta_di_gioco();
					}else if(!strcmp(comando,"!quit")){
						clear();
						disconnessione_dal_server();
						return 0;
					}else{
						printf("Comando non riconosciuto. \n");
					}
				}else{/* RICHIESTA DI CONNESSIONE DA ALTRO CLIENT */
					/* attesa nome dell'altro client */
					richiesta_dati_utente();
					accetta_richiesta_avvia_gioco();
					
				}
			}
			
		}
		
	}
	/* zona inaccessibile per sicurezza chiudiamo i socket */
	close(socket_gioco);
	close(socket_client);		
	return 0;
}
