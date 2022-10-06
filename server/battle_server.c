/* BATTLE SERVER */
/*
	Author: Gabriele Martino
	
	IL seguente codice contiene il main del server con gestione delle connessioni 

*/
#include "battle_s.h"



int main(int argc,char* argv[]){
	if(argc<=1){
		printf("Porta in ascolto non inserita. Riesegui\n");
		exit(1);
	}
	int  i, porta = atoi(argv[1]) ;
	if(porta <1024 || porta >= 65536){
		printf("Porta fuori dal range [1024,65535]. Riesegui.\n");
		exit(1);
	}
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, gestisci);
	inizializza(porta);//inizializzazione del server

	FD_ZERO(&read_fds);
	FD_ZERO(&master);
	FD_SET(listener, &master); // Aggiungo il listener al set
	fdmax = listener;

	cl_len = sizeof(client_addr);
	for(;;) {
		read_fds = master;
		
		select(fdmax + 1, &read_fds, NULL, NULL, NULL);
		for(i = 0; i <= fdmax; i++) { // Scorro tutto il set
			if(FD_ISSET(i, &read_fds)) { // Trovato un desc. pront
				if(i == listener) {// È il listener
					setta_nuovo_client();
					stampa_client();
				}else{// è qualche client che chiede ascolto
					uint32_t command;
					/* RICEVI COMANDO */
					controll = recv(i,(void*)&command,sizeof(command),MSG_WAITALL);
					/* DISCONNETTI NEL CASO DI ERRORE O COMANDO !QUIT */
					if( controll == 0 || controll == -1 || command == 3){
						FD_CLR(i,&read_fds);
						FD_CLR(i,&master);
						elimina_socket_client2(i);
						printf("Client disconnesso\n");
						stampa_client();
						break;
					}
					/* GESTIONE COMANDO !WHO */
					if(command == 1){
						printf("Richiesta dell'elenco client.\n");
						invia_client(i);
					}else if(command== 2){/* GESTIONE COMANDO !CONNECT */
						printf("Richiesta di gioco.\n");
						richiesta_gioco(i);
						stampa_client();
					}else if(command==15){/* UTENTE DISCONNESSO DAL GIOCO */
						struct elem* me = cerca_client2(i);
						me->state = 0;
						printf("	%s ha finito di giocare \n",me->nome);
						printf("	%s è libero\n",me->nome);
					}else{/* GESTIONE ERRORE */
						perror(" Something goes wrong.");
						FD_CLR(i,&read_fds);
						FD_CLR(i,&master);
						elimina_socket_client2(i);
						printf("Client disconnesso\n");
						stampa_client();
					}

				}

			}
		}
	}
	chiudi_tutti_socket();
	return 0;
}

