#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

int contador;

//Estructura acceso excluyente (Nombre mutex se puede cambiar)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *AtenderCliente (void *socket)
{
	int sock_conn;
	int *s;
	s= (int *) socket;
	sock_conn= *s;
	
	char peticion[512];
	char respuesta[512];
	int ret;
	int terminar =0;
	while (terminar ==0)
	{
		
	ret=read(sock_conn,peticion, sizeof(peticion));
	printf ("Recibido\n");
		
	peticion[ret]='\0';
		
	printf ("Peticion: %s\n",peticion);
		
		
	char *p = strtok( peticion, "/");
	int codigo =  atoi (p);
		
	char nombre[20];
	// Añadir la negación del código 4
	if ((codigo !=0)&&(codigo!=4))
	{
		p = strtok( NULL, "/");
			
		strcpy (nombre, p);
			
		printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
	}
		
	if (codigo ==0) 
		terminar=1;
	else if (codigo ==4)
		sprintf (respuesta,"%d",contador);
	else if (codigo ==1) //piden la longitd del nombre
		sprintf (respuesta,"%d",strlen (nombre));
	else if (codigo ==2)
		if((nombre[0]=='M') || (nombre[0]=='S'))
		strcpy (respuesta,"SI");
		else
			strcpy (respuesta,"NO");
		else 
		{
			p = strtok( NULL, "/");
			float altura =  atof (p);
			if (altura > 1.70)
				sprintf (respuesta, "%s: eres alto",nombre);
			else
				sprintf (respuesta, "%s: eresbajo",nombre);
		}
			
		if (codigo !=0)
		{
				
			printf ("Respuesta: %s\n", respuesta);
			write (sock_conn,respuesta, strlen(respuesta));
		}
		if ((codigo ==1)||(codigo==2)|| (codigo==3))
		{	
			//Aquí se añade la exclusión contínua
			pthread_mutex_lock( &mutex ); //Programa no puede interrumpir
			contador = contador +1;
			pthread_mutex_unlock( &mutex); //Programa ya puede interrumpir			
}
			
}
	close(sock_conn); 
	
}


int main(int argc, char *argv[])
{
	
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	contador =0;
	int i;
	int sockets[100];
	pthread_t thread;
	i=0;
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		
		sockets[i] =sock_conn;
		
		pthread_create (&thread, NULL, AtenderCliente,&sockets[i]);
		i=i+1;
		
	}

	
}
