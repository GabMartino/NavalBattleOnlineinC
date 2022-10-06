/* BATTLE SERVICE GIOCO */ 
/*
	Author: Gabriele Martino
	
	IL seguente codice contiene le definizioni delle funzioni dichiarate in 
		battle_gioco.h
	tra cui l'implementazione del gioco battaglia navale in p2p e 
	altre funzioni ristrette all'utilizzo all'interno del gioco

*/
#include "battle_service_c.h"
int my_counter = 0, counter_avversario = 0;
fd_set read_fds_u, master_u;

struct nave{
	int riga;
	int colonna;
	int colpito;
	struct nave* next;
};
struct nave* flotta;
struct nave* flotta_avversaria;
struct  sockaddr_in me_addr;

/* STAMPA COMANDI DI GIOCO */
void print_comandi_gioco(){
	printf("	Sono disponibili i seguenti comandi:\n!help --> mostra l'elenco dei comandi disponibili\n!disconnect --> disconnette il client dall'attuale partita\n!shot square --> fai un tentativo con la casella square\n!show --> visualizza griglia di gioco\n\n");


}
/* TORNA IL NUMERO DI NAVI PRESENTI NELLA PROPRIA FLOTTA */
int cerca_nave(int riga, int colonna){
	struct nave* p = flotta;
	while(p){
		if(p->riga == riga && p->colonna == colonna){
			if(p->colpito){
				return 1;
			}else{
				return 2;
			}
		}
		p = p->next;
	}
	return 0;
}
/* CERCA NEL CAMPO AVVERSARIO SETACCIATO SE LA CASELLA È STATA COLPITA (1), ABBIAMO FATTO ACQUA (0) OPPURE NON ABBIAMO ANCORA SPARATO IN QUEL PUNTO */
int cerca_nave_avversaria(int riga, int colonna){
	struct nave* p = flotta_avversaria;
	while(p){
		if(p->riga == riga && p->colonna == colonna){
			if(p->colpito){
				return 1;
			}else{
				return 2;
			}
		}
		p = p->next;
	}
	return 0;
}
/* CREA E INSERISCE UNA NUOVA NAVE. UTILIZZATA NELLA CREAZIONE DELLA PROPRIA FLOTTA, E INSERIMENTO NAVI NEL SEGNA POSTO AVVERSARIO */
void inserisci_nave(int riga, int colonna, int colpito, int flot){
	struct nave* p = (struct nave*)malloc(sizeof(struct nave));
	p->riga = riga;
	p->colonna = colonna;
	p->colpito = colpito;
	if(!flot){
		p->next = flotta;
		flotta = p;
	}else{
		p->next = flotta_avversaria;
		flotta_avversaria = p;
	}
}
/* SETTA SE UNA NAVE È STATA COLPITA */
void setta_nave(int riga,int colonna){
	
	struct nave* p = flotta;
	while(p){
		if(p->riga == riga && p->colonna == colonna){
			p->colpito = 1;
			return;
		}
		p = p->next;
	}
}
/* STAMPA CAMPO DI BATTAGLIA */
void stampa_campo(){
	int i,j,k;
	// mio campo
	printf("   MIO CAMPO DI BATTAGLIA 	    CAMPO DI BATTAGLIA AVVERSARIO AVVISTATO\n");
	printf("    1   2   3   4   5   6		    1   2   3   4   5   6  \n");
	for(i= 0;i<6;i++){
		
		printf("  -------------------------		  -------------------------\n");
		printf("%d |",i+1);
		for(j = 0;j<6;j++){
			
			if(cerca_nave(i+1,j+1) == 1 ){
				printf(" X ");
			}else if (cerca_nave(i+1,j+1) == 2){
				printf(" P ");
			}else{
				printf(" _ ");
			}
			printf("|");
		}
		printf("		%d |",i+1);
		for(k = 0;k<6;k++){
			
			if(cerca_nave_avversaria(i+1,k+1) == 1){
				printf(" X ");
			}else if(cerca_nave_avversaria(i+1,k+1) == 2){
				printf(" ^ ");
			}else{
				printf(" _ ");
			}
			printf("|");
		}
		printf("\n");
	}
	printf("  -------------------------		  -------------------------\n\n");
	// campo avversario
	
}
/* LIBERA LA MEMORIA DALLE FLOTTE */
void pulisci_flotte(){
	struct nave* p = flotta;
	while(p){
		flotta = p->next;
		free(p);
		p = flotta;
	}
	p = flotta_avversaria;
	while(p){
		flotta_avversaria = p->next;
		free(p);
		p = flotta_avversaria;
	}
}
/* INIZIALIZZA CONNESSIONE UDP */
void inizializzazione_UDP(int* porta_utente, uint32_t* ip_utente, int*flag){
	/* creazione socket di gioco */
	socket_gioco = socket(AF_INET, SOCK_DGRAM,0);
	if(socket_client == -1){
		perror("Errore creazione socket");
		exit(1);
	}
	//impostazione del mio indirizzo
	memset(&me_addr, 0, sizeof(me_addr));
	me_addr.sin_family = AF_INET ;
	me_addr.sin_port = htons(porta);
	me_addr.sin_addr.s_addr = INADDR_ANY;
	
	controll = bind(socket_gioco, (struct sockaddr*)&me_addr, sizeof(me_addr));
	if(controll == -1){
		perror(" Errore binding di rete");
		*flag = 1;
		return ;
	}
	//impostazione indirizzo destinatario
	memset(&gioco_addr, 0, sizeof(gioco_addr));
	gioco_addr.sin_family = AF_INET ;
	gioco_addr.sin_port = htons(*porta_utente);
	gioco_addr.sin_addr.s_addr = *ip_utente;
	struct in_addr ip_client;
	ip_client.s_addr = *ip_utente;
	printf("Avvio connessione con %s sulla porta %d \n\n",inet_ntoa(ip_client),porta);
}
/* INSERISCI NAVI SUL TUO CAMPO DI BATTAGLIA */
void inserimento_navi(){
	int i, riga, colonna;
	int check;
	printf("----------SEI IN GIOCO CON %s -----------\n",utente);
	printf(" Posizione 7 caselle come < riga colonna > [1,6]:\n");
	for(i = 0;i<= 6;i++){
		check = scanf(" %d %d",&riga,&colonna);
		if(check != 2 || riga<1 || colonna <1 || riga >=7 || colonna>=7 ){
			printf("Errore inserimento nave. Riprova.\n");
			i--;
		}else if(cerca_nave(riga,colonna)){
			printf("Nave gia presente. Riprova.\n");
			i--;
		}else{
			inserisci_nave(riga,colonna,0,0);
		}
		clear();//pulitura buffer di sicurezza
	}
}
/* CHIUDI GIOCO LIBERANDO LA MEMORIA E CHIUDENDO IL SOCKET DI GIOCO */
void chiusura_gioco(){
	pulisci_flotte();
	FD_CLR(socket_gioco,&master_u);
	FD_CLR(socket_gioco,&read_fds_u);
	FD_CLR(STDIN,&master_u);
	FD_CLR(STDIN,&read_fds_u);
	close(socket_gioco);
}
/* SPARA MISSILE */
int spara(){
	uint32_t riga,colonna, colpito, command = 1;
	int check;
	socklen_t dim = sizeof(gioco_addr);
	short unsigned int flag = 0;
	do{
		
		if(flag){
			printf(" Casella fuori dal campo o già sparato in quel punto. Inserisci un altra casella.\n");
		}
		check = scanf(" %d %d",&riga,&colonna);
		if(check != 2 || riga<1 || colonna <1 || riga >=7 || colonna>=7 || cerca_nave_avversaria(riga,colonna) ){
			flag = 1;
		}else{
			flag = 0;
		}
		clear();
	}while(flag);
	
	// invio comando
	
	controll = sendto(socket_gioco,(void*)&command,sizeof(command),0,(const struct sockaddr*)&gioco_addr,sizeof(gioco_addr));
	// invio riga e colonna
	controll = sendto(socket_gioco,(void*)&riga,sizeof(riga),0,(const struct sockaddr*)&gioco_addr,sizeof(gioco_addr));
	controll = sendto(socket_gioco,(void*)&colonna,sizeof(colonna),0,(const struct sockaddr*)&gioco_addr,sizeof(gioco_addr));
	//aspetta responso
	controll = recvfrom(socket_gioco,(void*)&colpito,sizeof(colpito),0,(struct sockaddr*)&gioco_addr,&dim);
	if(controll ==-1){
		perror("Errore invio (spara)\n");
		chiusura_gioco();
		return 0;
	}
	if(colpito){
		counter_avversario++;
		inserisci_nave(riga,colonna,1,1);
		if(counter_avversario == 7){

			printf(" HAI ABBATTUTO TUTTE LE NAVI DI %s COMPLIMENTI!!!\n",utente);
			chiusura_gioco();
			return 1;
		}
		
		stampa_campo();
		printf("		Hai colpito il bersaglio!!!\n");
	}else{
		inserisci_nave(riga,colonna,0,1);
		stampa_campo();
		printf("		Bersaglio mancato!!!\n");
	}
	
	return 0;
}
/* RICEVI MISSILE E CONTROLLA SE SEI STATO COLPITO */
int aspetta_missile(){
	uint32_t riga,colonna, colpito;
	socklen_t dim = sizeof(gioco_addr);
	controll = recvfrom(socket_gioco,(void*)&riga,sizeof(riga),0,(struct sockaddr*)&gioco_addr,&dim);	
	controll = recvfrom(socket_gioco,(void*)&colonna,sizeof(colonna),0,(struct sockaddr*)&gioco_addr,&dim);
	if(cerca_nave(riga,colonna)){
		colpito = 1;
		my_counter++;
		setta_nave(riga,colonna);
		controll = sendto(socket_gioco,(void*)&colpito,sizeof(colpito),0,(const struct sockaddr*)&gioco_addr,sizeof(gioco_addr));
		if(my_counter == 7){
			printf(" %s HA ABBATTUTO TUTTE LE TUE NAVI. HAI PERSO.\n",utente);
			chiusura_gioco();
			return 1;
		}
		stampa_campo();
		printf("		Sei stato colpito!!!!\n");
		
	}else{
		colpito = 0;
		controll = sendto(socket_gioco,(void*)&colpito,sizeof(colpito),0,(const struct sockaddr*)&gioco_addr,sizeof(gioco_addr));
		printf("		Non sei stato colpito!!!!\n");
		stampa_campo();
	}
	return 0;
}
/* AVVIO DEL GIOCO */
void avvia_gioco(int porta_utente, uint32_t ip_utente,int turn){
	/* indico che mi trovo in gioco */
	int flag = 0;
	locator = 1;
	char comando[20];
	int fd_max_u, i, check;
	struct timeval timer;
	
	inizializzazione_UDP(&porta_utente, &ip_utente, &flag);
	if(flag){
		return; // gestione errori nella connessione
	}
	inserimento_navi();
	print_comandi_gioco();
	stampa_campo();
	FD_ZERO(&read_fds_u);
	FD_ZERO(&master_u);
	FD_SET(socket_gioco, &master_u); 
	fd_max_u = socket_gioco;
	while(1){
		timer.tv_sec = 60;
		timer.tv_usec = 0;
		if(turn){
			FD_SET(STDIN,&master_u);
			printf("E' il tuo turno.\n");
			printf("#");
			fflush(stdout);
		}else{
			FD_CLR(STDIN,&master_u);
			printf("Attendi il tuo turno.\n");
		}
		read_fds_u = master_u;
		check = select(fd_max_u+1, &read_fds_u,NULL,NULL,&timer);
		if(!check){
			printf("Tempo scaduto. \n");
			/* invio il comando per avvisare la chiusura del socket perchè essendo
			l'udp connectionless (almeno usato in questo modo) l'altro client non saprà quando non potrà piu ricevere */
			uint32_t command = 0;
			controll = sendto(socket_gioco,(void*)&command,sizeof(command),0,(const struct sockaddr*)&gioco_addr,sizeof(gioco_addr));
			chiusura_gioco();
			return;
		}
		for(i = 0;i<= fd_max_u;i++){
			if(FD_ISSET(i,&read_fds_u)){
				if(i== STDIN){// SCRITTURA COMANDO
					//clear();
					scanf("%s",comando);
					uint32_t command;
					if(!strcmp(comando,"!help")){
						print_comandi_gioco();
						
						//FD_SET(STDIN,&master_u);
						break;
					}else if(!strcmp(comando,"!disconnect")){
						
						command = 0;
						
						controll = sendto(socket_gioco,(void*)&command,sizeof(command),0,(const struct sockaddr*)&gioco_addr,sizeof(gioco_addr));
						chiusura_gioco();
						//FD_CLR(STDIN,&read_fds_u);
						printf("Disconnessione avvenuta con successo: TI SEI ARRESO\n");
						printf("\n\n");
						return;
					}else if(!strcmp(comando,"!shot")){
						if(spara()){
							return;
						}
						turn = 0;
						//FD_CLR(STDIN,&read_fds_u);
					}else if(!strcmp(comando,"!show")){
						stampa_campo();
						
						//FD_SET(STDIN,&master_u);
						break;

					}else{
						printf("Comando non riconosciuto. \n");
					}
					
				}else{// ARRIVO COMANDO DALL'ESTERNO
					uint32_t command;
					socklen_t dim = sizeof(gioco_addr);
					controll = recvfrom(socket_gioco,(void*)&command,sizeof(command),0,(struct sockaddr*)&gioco_addr,&dim);
					//printf(" command = %d controll = %d \n",command,controll );
					if(command == 0 || controll == 0){
						printf(" %s si è disconnesso. HAI VINTO.\n\n\n",utente);
						chiusura_gioco();
						return;
					}else if(command== 1){// IN ARRIVO UN MISSILE
						if(aspetta_missile()){
							return;
						}
						turn = 1;
						//FD_SET(STDIN,&master_u);
						break;
					}else{
						printf("command = %d e controll = %d \n",command,controll);
					}
				}
				

			}
		}
	}

}
