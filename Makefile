battle_make:
	gcc ./server/battle_server.c ./server/battle_service_s.c -Wall -o battle_server
	gcc ./client/battle_client.c ./client/battle_service_c.c ./client/battle_gioco.c -Wall -o battle_client
