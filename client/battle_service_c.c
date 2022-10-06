/* BATTLE SERVICE CLIENT */
/* 
	Author: Gabriele Martino
	
	Il seguente codice contiene funzioni utili all'inizializzazione
	e alla connessione del client con il server e alla connessione del
	client con un altro utente per giocare

*/

#include "battle_service_c.h"

void print_command(){
	printf("\n\nSono disponibili i seguenti comandi:\n!help --> mostra l'elenco dei comandi disponibili\n!who --> mostra l'elenco dei client connessi al server\n!connect username --> avvia una partita con l'utente username\n!quit --> disconnette il client dal server\n\n");

}

void inizializza_client(char* server_ip,char* port){
	struct  sockaddr_in server_addr;
	
	// creazione socket del client
	socket_client = socket(AF_INET, SOCK_STREAM,0);
	if(socket_client == -1){
		perror("errore creazione socket");
		exit(1);
	}
	//impostazione indirizzo server
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET ;
	server_addr.sin_port = htons(atoi(port));
	inet_pton(AF_INET, (char*)server_ip, &server_addr.sin_addr);
	
	printf("%s  %d\n",server_ip,atoi(port));

	// avvio connessione
	controll = connect(socket_client,(const struct sockaddr*)&server_addr,sizeof(server_addr));	
	if(controll == -1){
		close(socket_client);
		perror("Errore connessione al server ");
		exit(1);
	}else{
		printf("Connessione al server %s (porta %d) effettuata con successo\n\n",server_ip,atoi(port));
	}
}
void gestisci(){
	if(locator){
		uint32_t command = 0;
		sendto(socket_gioco,(void*)&command,sizeof(command),0,(const struct sockaddr*)&gioco_addr,sizeof(gioco_addr));
		close(socket_gioco);
	}
	
	close(socket_client);
	printf("\n");
	exit(1);
}
void autenticazione(){
	short flag;
	int dim;
	nome = (char*)malloc(20);
	do{
		flag = 0;
		do{
			if(flag){
				printf("---->Nickname maggiore di 20 caratteri<----\n");
			}
			printf("Inserisci il tuo nome:");
			scanf("%s",nome);
			flag++;
			clear();
		}while(strlen(nome)>20);
		flag = 0;
		do{
			if(flag){
				printf("--->Porta non valida. La porta deve essere tra 1024-65535.\n");
			}
			printf("Inserisci la porta UDP di ascolto:");
			scanf("%d",&porta);
			flag++;
			clear();
		}while(porta<1024 || porta >=65536);
		// invio dimensione
		flag= 0;
		dim = strlen(nome)+1;
		//printf("%d\n",dim);
		controll = send(socket_client,(void*)&dim,4,0);
		if(controll == -1){
			close(socket_client);
			perror("Errore invio dimensioni\n");
			exit(1);
		}
		// invio nome
		controll = send(socket_client,(void*)nome,dim,0);
		if(controll == -1){
			close(socket_client);
			perror("Errore invio dati\n");
			exit(1);
		}
		//invio porta
		controll = send(socket_client,(void*)&porta,4,0);
		if(controll == -1){
			close(socket_client);
			perror("Errore invio dati\n");
			exit(1);
		}
		//conferma iscrizione
		controll = recv(socket_client,(void*)&flag,2,0);
		if(flag){
			printf("----->>>Nickname gia in uso. Riprova<<<----\n");
		}else{
			printf("Registrazione effettuata con successo\n");
			printf("Nickname = %s, porta UDP = %d \n",nome,porta);
		}
	}while(flag);
}

/* RICHIESTA ELENCO CLIENT */
void richiesta_elenco_client(){
	uint32_t command = 1, dim;
	controll = send(socket_client,(void*)&command,sizeof(command),0);
	if(controll == -1){
		perror("Error send: ");
		return;
	}
	//numero client
	controll = recv(socket_client,(void*)&command,sizeof(command),0);
	printf(" Client connessi al server: \n");
	int i, state;
	char* stato;
	for(i = 0;i<command;i++){
		//ricevo nome
		controll = recv(socket_client,(void*)&dim,sizeof(dim),0);
		char*buff = (char*)malloc(dim);
		controll = recv(socket_client,(void*)buff,dim,0);
		// ricevo stato
		controll = recv(socket_client,(void*)&state,sizeof(state),0);
		if(state){
			stato = "Occupato";
		}else{
			stato = "Libero";
		}
		if(!strcmp(nome,buff)){
			printf(" Me--->	%s(%s)\n",buff,stato);
		}else{
			printf("	%s(%s)\n",buff,stato);
		}
		free(buff);
	}
	fflush(stdout);

}
/* RICHIESTA DI GIOCO DA PARTE DELL'UTENTE */
void richiesta_di_gioco(){
	uint32_t command = 2;
	int dim = strlen(utente)+1;
	// invio comando
	controll = send(socket_client,(void*)&command,sizeof(command),0);
	//invio dimensione username
	controll = send(socket_client,(void*)&dim,sizeof(dim),0);
	//invio username
	controll = send(socket_client,(void*)utente,dim,0);
	// aspetta responso
	controll = recv(socket_client,(void*)&command,sizeof(command),0);
	if(!command){
		printf("Nickname non presente \n");
		
	}else if(command == 1){
		printf("L'utente richiesto è occupato in un altra partita. \n");
		
	}else if(command == 2){
		printf("Errori a caso\n");
		
	}else if(command == 3){
		printf("Richiesta inviata con successo. Attendi la risposta dall'utente...\n");
		controll = recv(socket_client,(void*)&command,sizeof(command),0);
		if(controll == 0 || controll == -1 || command == 15){
			perror("Errore richiesta connessione");
		}else if(!command){
			printf("La tua richiesta di gioco è stata accettata\n");
			int porta_utente;
			// ricevo porta
			controll = recv(socket_client,(void*)&porta_utente,sizeof(porta),0);
			uint32_t ip_utente;
			//ricevo ip
			controll = recv(socket_client,(void*)&ip_utente,sizeof(ip_utente),0);
			/* AVVIO GIOCO */
			avvia_gioco(porta_utente,ip_utente,0);
			command = 15;
			controll = send(socket_client,(void*)&command,sizeof(command),0);
			
		}else{
			printf("La tua richiesta di gioco è stata rifiutata.\n");
		}
	}
}
/* DISCONNETTI DAL SERVER */
void disconnessione_dal_server(){
	uint32_t command = 3;
	controll = send(socket_client,(void*)&command,sizeof(command),0);
	if(controll == -1){
		perror("Error send: ");
		return;
	}
	printf("Disconnessione effettuata con successo.\n");
	close(socket_client);
}
/* RICHIESTA NOME UTENTE DALL'ALTRO CLIENT*/
void richiesta_dati_utente(){
	uint32_t dim;
	controll = recv(socket_client,(void*)&dim,sizeof(dim),0);
	if(controll == 0 || controll == -1){
		printf("Errore Server: Disconnessione in corso.\n");
		gestisci();
		exit(1);

	}
	utente = (char*)malloc(dim);
	controll = recv(socket_client,(void*)utente,dim,0);
	if(controll == 0 || controll == -1){
		printf("Errore Server: Disconnessione in corso.\n");
		gestisci();
		exit(1);

	}
}
/* PULITURA BUFFER */
void clear(){
	char c;
	while( (c = getchar() != '\n') && c != EOF);
}
/* ACCETTAZIONE RICHIESTA E AVVIO DEL GIOCO */
void accetta_richiesta_avvia_gioco(){
	printf("------>	%s HA CHIESTO DI GIOCARE CON TE\n 	accetta/rifiuta	[S/n]	",utente);
	char ok;
	do{
		fflush(stdin);
		scanf(" %c",&ok);
		if(ok == 'S' || ok == 's' || ok == 'N' || ok == 'n'){
			// avviso il server della scelta
			controll = send(socket_client,(void*)&ok,sizeof(ok),0);
			if(controll == 0 || controll == -1){
				printf("Errore Server: Disconnessione in corso.\n");
				gestisci();
				exit(1);
			}
			break;
		}else{
			printf("Inserisci una risposta valida.\n");
		}
	}while(1);
	clear();// pulitura buffer stdin di sicurezza
	if(ok == 'S' || ok == 's'){
		uint32_t command,ack;
		//ACCETTATO QUINDI RICEVO DATI DEL RICHIEDENTE
		int porta_utente;
		// ricevo porta
		controll = recv(socket_client,(void*)&porta_utente,sizeof(porta_utente),0);
		if(controll == -1 || controll== 0 ){
			perror("Errore disconnessione dal gioco ");
			return;
		}
		uint32_t ip_utente;
		//ricevo ip
		controll = recv(socket_client,(void*)&ip_utente,sizeof(ip_utente),0);
		if(controll == -1 || controll == 0 ){
			perror("Errore disconnessione dal gioco");
			return;
		}
		controll = recv(socket_client,(void*)&ack,sizeof(ack),0);
		if(controll == 0 || controll == -1 || ack == 15){
			printf("Errore nell'handshake \n");
			return;
		}
		/* AVVIO GIOCO */
		avvia_gioco(porta_utente,ip_utente,1);
		/////Avvio gioco
		command = 15;
		//avviso server della disconnessione
		printf("\n\n");
		controll = send(socket_client,(void*)&command,sizeof(command),0);
		if(controll == -1 || controll == 0){
			perror("Errore disconnessione dal gioco");

		}
		
	}else{
		printf(" Richiesta rifiutata.\n");
	}
}
