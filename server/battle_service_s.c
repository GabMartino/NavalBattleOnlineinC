/* DEFINIZIONE DELLE FUNZIONI DI SERVIZIO Server*/
/*
	Author: Gabriele Martino
	
	IL seguente codice contiene la definizione delle funzioni dichiarate in
		"Battle_s.h"

*/
#include "battle_s.h"
/* STAMPA CLIENT */
void stampa_client(){
	struct elem* point = client;
	
	if(point){
		struct in_addr ip_client;
		printf("--------- Client connessi ------\n");
		printf("stato portaUDP socket   IP          nome\n");
		while( point){
			ip_client.s_addr = point->my_ip;
			printf("  %d     %d     %d     %s    %s\n",point->state,point->porta,point->socket,inet_ntoa(ip_client),point->nome);
			point = point->next;

		}
	}
}
/* INSERIMENTO CLIENT */
void inserisci_client(char* nome, int dim, int porta, int sock, uint32_t ip_address){
	struct elem* new_pointer = (struct elem*)malloc(sizeof(struct elem));

	new_pointer->nome = (char*)malloc(dim);
	strcpy(new_pointer->nome,nome);
	new_pointer->state = 0;
	new_pointer->porta = porta;
	new_pointer->next = client;
	new_pointer->socket = sock;
	new_pointer->my_ip = ip_address;
	client = new_pointer;
	printf("	%s è libero.\n",nome);

}
/*RICERCA CLIENT PER NUMERO SOCKET */

struct elem* cerca_client2(int sock){
	struct elem* point;
	point = client;
	while(point){
		if(point->socket == sock){
			return point;
		}
		point = point->next;

	}
	return 0;

}
/* RICERCA CLIENT PER NUMERO SOCKET */
struct elem* cerca_client(char* nome){
	struct elem* point = client;
	while(point){
		if(!strcmp(point->nome,nome)){
			return point;
		}
		point = point->next;

	}
	return 0;

}
/* RESTITUISCI IL NUMERO DEI CLIENT CONNESSI */
int num_client(){
	struct elem* point = client;
	int i = 0;
	while( point){
		i++;
		point = point->next;
	}
	return i;
}

/* ELIMINA SOCKET E CLIENT PER SOCKET */
void elimina_socket_client2(int socket){
	struct elem* pointer = cerca_client2(socket);
	if(pointer){
		struct elem* head = client;
		if(head == pointer){
			close(pointer->socket);
			free(pointer);
			client = head->next;	
			return;
		}
		while(head && head->next != pointer){
			head = head->next;
		}
		if(head->next == pointer){
			head->next = pointer->next;
			free(pointer);
		}

	}
}
/* ERROR CHECK */
short controlla_errore(int controll,int* socket){
	if(controll == -1){
		perror("Errore ricezione:");

	}else if(!controll){
		printf("Client disconnesso.\n");
	}else{
		return 0;
	}

	close(*socket);
	return 1;
}

void chiudi_tutti_socket(){
	int i;
	for(i= 0;i<fdmax;i++){
		if(FD_ISSET(i,&read_fds) && i != listener){
			FD_CLR(i,&read_fds);
			close(i);
		}

	}
	close(listener);//chiudo il server alla fine per evitare blocchi temporanei nei socket del sistema
}
void gestisci(){
	chiudi_tutti_socket();
	exit(1);
}

/* INIZIALIZZO E SETTO I PARAMETRI PER L'I/O MULTIPLEXING */
void inizializza(int porta){

	int  controll;
	//impostazione indirizzo
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(porta);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	//creazione socket di ascolto
	listener = socket(AF_INET,SOCK_STREAM,0);
	if(listener == -1){
		perror("Errore creazione socket ");
		exit(1);
	}
	//binding di rete
	controll = bind(listener, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if(controll == -1){
		perror("Errore binding di rete ");
		exit(1);
	}

	controll = listen(listener,100);
	if(controll == -1){
		perror("Errore messa in ascolto ");
		exit(1);
	}
	printf("Indirizzo: %s  (Porta: %d) \n",inet_ntoa(server_addr.sin_addr),porta);
	
}
/* INSERISCI NUOVO CLIENT */
void setta_nuovo_client(){
	client_socket = accept(listener,(struct sockaddr *)&client_addr, &cl_len);
	printf("Connessione stabilita con il client.\n");
	FD_SET(client_socket, &master);
	int dim;
	short flag;
	char* buff;
	do{
		flag = 0;
		controll = recv(client_socket,(void*)&dim,sizeof(dim),0);
		if(controlla_errore(controll,&client_socket)){	
			FD_CLR(client_socket, &master);
			break;
		}

		//ricezione dati
		buff = (char*)malloc(dim);
		//ricezione nome
		controll = recv(client_socket,(void*)buff,dim,0);
		if(controlla_errore(controll,&client_socket)){							
			FD_CLR(client_socket, &master);
			break;
		}

		//ricezione porta
		int porta;
		controll = recv(client_socket,(void*)&porta,sizeof(porta),0);
		if(controlla_errore(controll,&client_socket)){	
			FD_CLR(client_socket, &master);
			break;
		}
		flag = (long)cerca_client(buff);
		printf("	%s si è connesso.\n",buff);
		if(!flag){
			inserisci_client(buff,dim,porta,client_socket,client_addr.sin_addr.s_addr);
		}
		controll = send(client_socket,(void*)&flag,sizeof(flag),0);
		if(controlla_errore(controll,&client_socket)){
			FD_CLR(client_socket, &master);
			elimina_socket_client2(client_socket);
			break;
		}
		free(buff);
	}while(flag);
	if(client_socket > fdmax){
		fdmax = client_socket;
	}

}
/* INVIO ELENCO CLIENT */
void invia_client(int i){
	int num = num_client();
						
	controll = send(i,(void*)&num,sizeof(num),0);
	if(controll == -1){
		perror("Errore:");
		return;
	}
	struct elem* point = client;
	while(point){
		int dim = strlen(point->nome)+1;
		/* invio nome */
		controll = send(i,(void*)&dim,sizeof(dim),0);
		controll = send(i,(void*)point->nome,dim,0);
		/* invio stato */
		dim = point->state;
		controll = send(i,(void*)&dim,sizeof(dim),0);
		point = point->next;
		if(controll == -1){
			perror("Errore invio client");
			break;
		}
		
	}
}
/* CHECK PER AVVIO DEL GIOCO */
void richiesta_gioco(int i){
	uint32_t dim, command;
	//ricevo dimensione
	controll = recv(i,(void*)&dim,sizeof(dim),0);
	char* buff = (char*)malloc(dim);
	//ricevo username
	controll = recv(i,(void*)buff,dim,0);
	// client richiesto
	struct elem* point = cerca_client(buff);
	if(!point){// CASO USERNAME NON PRESENTE
		command = 0;
		controll = send(i,(void*)&command,sizeof(command),0);
		return;

	}else if(point->state){//CASO CLIENT OCCUPATO
		command = 1;
		controll = send(i,(void*)&command,sizeof(command),0);
		return;

	}else if(controll == -1 || controll == 0){
		command = 2;
		controll = send(i,(void*)&command,sizeof(command),0);
		printf("Errori a caso\n");
		return;

	}else {
		avvio_gioco(i,point);
	}
	
}
/* AVVIO DEL GIOCO */
void avvio_gioco(int i,struct elem* utente){
	/* i è l'utente  che ha effettuato la richiesta di gioco
		utente è il client che riceve la richiesta */

	uint32_t command = 3,dim ;
	controll = send(i,(void*)&command,sizeof(command),0);
	if(controll == 0 || controll == -1){
		perror("Errore invio(1), client richiedente disconnesso");
		//elimino i dati dell'utente disconnesso
		FD_CLR(i,&read_fds);
		FD_CLR(i,&master);
		elimina_socket_client2(i);
		close(i);
		return;
	}
	/* INVIO RICHIESTA PARTITA */
	// client richiedente
	struct elem* me = cerca_client2(i);
	dim = strlen(me->nome)+1;
	// invio nome del richiedente
	controll = send(utente->socket,(void*)&dim,sizeof(dim),0);
	if(controll == 0 || controll == -1){
		perror("Errore invio(2)");
		close(utente->socket);
		return;
	}
	controll = send(utente->socket,(void*)me->nome,dim,0);
	if(controll == 0 || controll == -1){
		perror("Errore invio(3)");
		close(utente->socket);
		return;
	}
	
	//richiesta di accettazione a giocare
	char risposta;
	controll = recv(utente->socket,(void*)&risposta,sizeof(risposta),0);
	if(controll == 0 || controll == -1){
		command = 15;
		perror("Errore ricezione(1), Client disconnesso durante la richiesta");
		//avviso l'utente richiedente che qualcosa è andato storto
		controll = send(i,(void*)&command,sizeof(command),0);
		//elimino i dati dell'utente disconnesso
		FD_CLR(utente->socket,&read_fds);
		FD_CLR(utente->socket,&master);
		elimina_socket_client2(utente->socket);
		close(utente->socket);
		return;
	}
	if(risposta == 'S' || risposta == 's'){
		command = 0;
		controll = send(i,(void*)&command,sizeof(command),0);
		if(controll == 0 || controll == -1){
			perror("Errore invio(4)");
			command = 15;
			controll = send(utente->socket,(void*)&command,sizeof(command),0);
			//elimino i dati dell'utente disconnesso
			FD_CLR(i,&read_fds);
			FD_CLR(i,&master);
			elimina_socket_client2(i);
			close(i);
			return;
		}
		// invio a chi ha accettato, i dati del richiedente
		controll = send(utente->socket,(void*)&me->porta,sizeof(me->porta),0);
		if(controll == 0 || controll == -1){
			perror("Errore invio(5)");
			//elimino i dati dell'utente disconnesso
			FD_CLR(utente->socket,&read_fds);
			FD_CLR(utente->socket,&master);
			elimina_socket_client2(utente->socket);
			close(utente->socket);
			printf("Client disconnesso\n");
			return;
		}
		
		controll = send(utente->socket,(void*)&me->my_ip,sizeof(me->my_ip),0);
		if(controll == 0 || controll == -1){
			perror("Errore invio(6)");
			//elimino i dati dell'utente disconnesso
			FD_CLR(utente->socket,&read_fds);
			FD_CLR(utente->socket,&master);
			elimina_socket_client2(utente->socket);
			close(utente->socket);
			printf("Client disconnesso\n");
			return;
		}
		//invio al richiedente i dati del richiesto
		controll = send(i,(void*)&utente->porta,sizeof(utente->porta),0);
		if(controll == 0 || controll == -1){
			perror("Errore invio(7)");
			command = 15;
			controll = send(utente->socket,(void*)&command,sizeof(command),0);
			//elimino i dati dell'utente disconnesso
			FD_CLR(i,&read_fds);
			FD_CLR(i,&master);
			elimina_socket_client2(i);
			close(i);
			printf("Client disconnesso\n");
			return;
		}
		controll = send(i,(void*)&utente->my_ip,sizeof(utente->my_ip),0);
		if(controll == 0 || controll == -1){
			perror("Errore invio(8)");
			//elimino i dati dell'utente disconnesso
			command = 15;
			controll = send(utente->socket,(void*)&command,sizeof(command),0);
			FD_CLR(i,&read_fds);
			FD_CLR(i,&master);
			elimina_socket_client2(i);
			close(i);
			printf("Client disconnesso\n");
			return;
		}
		/* invio acknoledgment di avvenuto handshake */
		command = 0;
		controll = send(utente->socket,(void*)&command,sizeof(command),0);
		utente->state = 1;
		me->state = 1;

		printf(" %s sta giocando con %s \n",me->nome,utente->nome);
		
	}else if(risposta == 'N' || risposta == 'n'){
		command = 1;
		controll = send(i,(void*)&command,sizeof(command),0);
		if(controll == 0 || controll == -1){
			perror("Errore ricezione(2)");
			//elimino i dati dell'utente disconnesso
			FD_CLR(i,&read_fds);
			FD_CLR(i,&master);
			elimina_socket_client2(i);
			close(i);
			printf("Client disconnesso\n");
			return;
		}
	}else{
		printf("Errori a caso\n");
	}



}
