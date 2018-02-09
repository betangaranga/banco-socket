//Recibe de parametros 1.Puerto como servidor
//2.IP del servidor 2 3.Puerto como cliente a servidor 2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

typedef struct _Datos {
        char nombre[50];
        char email[50];
        char cuenta[10];
        double saldo;
        char estatus;
        long id;
        //Datos necesarios para comunicacion con el servidor 2
        int newsockfd;
        int portno;
        char *hostname;
        int Operacion;
}Data;
typedef struct {
								int ID;
								int type; //1.Consulta, 2.Deposito. 3.Retiro 4.Tranferencia
								char ctaOrigen[11];
								char ctaDestino[11];
								double Cantidad;
                int Operacion;
                int newsockfd;
                int portno;
                char *hostname;

}Lotes;
typedef struct {
        int ID;
        int type;         //1.Consulta, 2.Deposito. 3.Retiro 4.Tranferencia
        char ctaOrigen[11];
        char ctaDestino[11];
        double Cantidad;
}Lote;
typedef struct {
	char Cuenta[10];
	char mensaje[30];
}Respuesta;

typedef struct{
	int ID;
	int type;
	char cuenta[10];
	double OldSaldo;
	double NewSaldo;
	double Cantidad;
	char Estado;

}Log;
void error(const char *msg)
{
        perror(msg);
        exit(1);
}
void *Servidor_Hilo_Consulta(void *parametros){
        Data *userdata = (Data *)parametros;
        Data datos;
        datos= *userdata;
        int n;
        FILE *f;
        int ch=0;
        int lines=0;
        f =fopen("BACK.txt","r");
        if (!f){
          return 1;
              }
        while(!feof(f))
              {
                ch = fgetc(f);
                if(ch == '\n')
                {
                  lines++;
                }
              }

          		fclose(f);

        //char buffer[256];
        n = read(datos.newsockfd,&datos.cuenta,sizeof(datos.cuenta));

        int sockfd, portno;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        portno = datos.portno;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                printf("ERROR al abrir socket");
        server = gethostbyname(datos.hostname);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no existe host\n");
                exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);
        //Se conecta con el servidor
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                perror("ERROR al conectar");

        n = write(sockfd,&datos.Operacion,sizeof(int));
        if (n < 0) {
                printf("ERROR al indicar la operacion ");
        }
        write(sockfd,&datos.cuenta,sizeof(datos.cuenta));
        if (n < 0) {
                printf("ERROR al la cuenta");
        }
        Log EnviarLog2;
        n = read(sockfd,&EnviarLog2,sizeof(Log));
        if (n < 0) {
                printf("ERROR recuperar datos de consulta");
        }
        n = write(datos.newsockfd,&EnviarLog2,sizeof(Log));
        if (n < 0) {
                printf("ERROR enviar datos de consulta");
        }

          f =fopen("BACK.txt","a+");
          fprintf(f, "\n%d\t\t", lines);
    			fprintf(f, "%d\t\t", EnviarLog2.type);
    			fprintf(f, "%s\t\t", EnviarLog2.cuenta);
    			fprintf(f, "%lf\t\t", EnviarLog2.OldSaldo);
    			fprintf(f, "%lf\t\t",  EnviarLog2.NewSaldo );
    			fprintf(f, "%lf\t\t", EnviarLog2.Cantidad);
          fprintf(f, "%c", EnviarLog2.Estado);
          fclose(f);
        close(sockfd);
}

void *Servidor_Hilo_Alta(void *parametros){
        Data *userdata = (Data *)parametros;
        Data datos;
        datos= *userdata;
        int n;
        bzero(datos.nombre,25);
        char buffer[256];
        FILE *f;
        int ch=0;
        int lines=0;
        f =fopen("BACK.txt","r");
        if (!f){
        f =fopen("BACK.txt","a+");

              }
        while(!feof(f))
              {
                ch = fgetc(f);
                if(ch == '\n')
                {
                  lines++;
                }
              }

              fclose(f);
        n = read(datos.newsockfd,&datos,sizeof(Data));
        if (n < 0)
                error("ERROR de lectura del socket");
        printf("Cliente canalizado, esperando a que finalice actividad para escribir en log.txt:\n");
        printf("Guardando datos en el archivo de base\n");
        //Se realiza la conexion con el servidor 2
        int sockfd, portno;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        portno = datos.portno;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                printf("ERROR al abrir socket");
        server = gethostbyname(datos.hostname);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no existe host\n");
                exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);
        //Se conecta con el servidor
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                perror("ERROR al conectar");
        n = write(sockfd,&datos.Operacion,sizeof(int));
        if (n < 0) {
                printf("ERROR al indicar la operacion ");
        }
        n = write(sockfd,&datos,sizeof(Data));
        if (n < 0) {
                printf("ERROR al escribir en el socket");
        }
        //Se lee del socket la confirmacion
        Log RecibidoLog;
        n = read(sockfd,&RecibidoLog,sizeof(Log));
        if (n < 0) {
                printf("ERROR al leer del socket");
        }
      printf("Llegue antes de imprimir lo que recibio\n");
      printf("ID:%d\n",RecibidoLog.ID);
      printf("Type:%d\n",RecibidoLog.type);
      printf("Cuenta:%s\n",RecibidoLog.cuenta );
      printf("OldSaldo:%lf\n",RecibidoLog.OldSaldo );
      printf("NewSaldo:%lf\n",RecibidoLog.NewSaldo );
      printf("Cantidad:%lf\n",RecibidoLog.Cantidad );
      printf("El estado de la transaccion es: %c\n",RecibidoLog.Estado);
      printf("sizeof:%ld\n",sizeof(Log));
      printf("Ya debi haber impreso el estado\n");

      f =fopen("BACK.txt","a+");
      fprintf(f, "\n%d\t\t", lines);
      fprintf(f, "%d\t\t", RecibidoLog.type);
      fprintf(f, "%s\t\t", RecibidoLog.cuenta);
      fprintf(f, "%lf\t\t", RecibidoLog.OldSaldo);
      fprintf(f, "%lf\t\t",  RecibidoLog.NewSaldo );
      fprintf(f, "%lf\t\t", RecibidoLog.Cantidad);
      fprintf(f, "%c", RecibidoLog.Estado);
      fclose(f);

        Respuesta Answ;
        n = read(sockfd,&Answ,sizeof(Respuesta));
        if (n < 0) {
                printf("ERROR al leer del socket");
        }
        printf("%s\n",Answ.mensaje);
        //Se escribe en el socket la confirmacion
        n = write(datos.newsockfd,&Answ,sizeof(Respuesta));
        if (n < 0)
                error("ERROR de escritura al socket");
        close(sockfd);

}
void *Servidor_Hilo_Actualizar(void *parametros){
        Data *userdata = (Data *)parametros;
        Data datos;
        datos= *userdata;
        int n;
        FILE *f;
        int ch=0;
        int lines=0;
        f =fopen("BACK.txt","r");
        if (!f){
          f =fopen("BACK.txt","a+");
              }
        while(!feof(f))
              {
                ch = fgetc(f);
                if(ch == '\n')
                {
                  lines++;
                }
              }

              fclose(f);
        bzero(datos.nombre,25);
        char buffer[256];
        n = read(datos.newsockfd,&datos,sizeof(Data));
        if (n < 0)
                error("ERROR de lectura del socket");
        printf("Cliente canalizado, esperando a que finalice actividad para escribir en log.txt:\n");
        printf("Guardando datos en el archivo de base\n");
        //Se realiza la conexion con el servidor 2
        int sockfd, portno;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        portno = datos.portno;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                printf("ERROR al abrir socket");
        server = gethostbyname(datos.hostname);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no existe host\n");
                exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);
        //Se conecta con el servidor
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                perror("ERROR al conectar");
        n = write(sockfd,&datos.Operacion,sizeof(int));
        if (n < 0) {
                printf("ERROR al indicar la operacion ");
        }
        n = write(sockfd,&datos,sizeof(Data));
        if (n < 0) {
                printf("ERROR al escribir en el socket");
        }

        //Se lee del socket la confirmacion
        Log RecibidoLog;
        n = read(sockfd,&RecibidoLog,sizeof(Log));
        if (n < 0) {
                printf("ERROR al leer del socket");
        }
      printf("Llegue antes de imprimir lo que recibio\n");
      printf("ID:%d\n",RecibidoLog.ID);
      printf("Type:%d\n",RecibidoLog.type);
      printf("Cuenta:%s\n",RecibidoLog.cuenta );
      printf("OldSaldo:%lf\n",RecibidoLog.OldSaldo );
      printf("NewSaldo:%lf\n",RecibidoLog.NewSaldo );
      printf("Cantidad:%lf\n",RecibidoLog.Cantidad );
      printf("El estado de la transaccion es: %c\n",RecibidoLog.Estado);
      printf("sizeof:%ld\n",sizeof(Log));
      printf("Ya debi haber impreso el estado\n");

      f =fopen("BACK.txt","a+");
      fprintf(f, "\n%d\t\t", lines);
      fprintf(f, "%d\t\t", RecibidoLog.type);
      fprintf(f, "%s\t\t", RecibidoLog.cuenta);
      fprintf(f, "%lf\t\t", RecibidoLog.OldSaldo);
      fprintf(f, "%lf\t\t",  RecibidoLog.NewSaldo );
      fprintf(f, "%lf\t\t", RecibidoLog.Cantidad);
      fprintf(f, "%c", RecibidoLog.Estado);
      fclose(f);

        Respuesta Answ;
        n = read(sockfd,&Answ,sizeof(Respuesta));
        if (n < 0) {
                printf("ERROR al leer del socket");
        }
        printf("%s\n",Answ.mensaje);
        //Se escribe en el socket la confirmacion
        n = write(datos.newsockfd,&Answ,sizeof(Respuesta));
        if (n < 0)
                error("ERROR de escritura al socket");

        close(sockfd);
}
void *Servidor_Hilo_Lotes(void *parametros){
        int i=0,j;
        long fileSize;
        Lotes *userdata=(Lotes *)parametros;
        Lotes lotes;

        lotes=*userdata;
        read(lotes.newsockfd,&j,sizeof(int));
        printf("LLEGO LOOP %d\n",j );

        read(lotes.newsockfd,&fileSize,sizeof(long));
        printf("LLEGO FS %ld\n",fileSize );

        Lote lote[j];
        read(lotes.newsockfd,lote,sizeof(lote));
        for(i=0; i<j; i++) {
                printf(" Cuenta: %s\n",lote[i].ctaOrigen);
                printf(" Cantidad: %lf\n",lote[i].Cantidad);
                printf(" Cuenta Dest: %s\n",lote[i].ctaDestino);
                printf(" Type: %d\n",lote[i].type);
                printf(" ID: %d\n",lote[i].ID);
              }
              int sockfd, portno;
              struct sockaddr_in serv_addr;
              struct hostent *server;
              portno = lotes.portno;
              sockfd = socket(AF_INET, SOCK_STREAM, 0);
              if (sockfd < 0)
                      printf("ERROR al abrir socket");
              server = gethostbyname(lotes.hostname);
              if (server == NULL) {
                      fprintf(stderr,"ERROR, no existe host\n");
                      exit(0);
              }
              bzero((char *) &serv_addr, sizeof(serv_addr));
              serv_addr.sin_family = AF_INET;
              bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
              serv_addr.sin_port = htons(portno);
      //Se conecta con el servidor
              if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                      perror("ERROR al conectar");
            int  n = write(sockfd,&lotes.Operacion,sizeof(int));
              if (n < 0) {
                      printf("ERROR al indicar la operacion ");
              }
              write(sockfd,&j,sizeof(int));

              write(sockfd,lote,sizeof(lote));
              close(sockfd);
}
void *Servidor_Hilo_Transferencia(void *parametros){
Lotes *userdata=(Lotes *)parametros;
Lotes lotes;
lotes=*userdata;
int n;
FILE *f;
int ch=0;
int lines=0;
f =fopen("BACK.txt","r");
if (!f){
  f =fopen("BACK.txt","a+");
      }
while(!feof(f))
      {
        ch = fgetc(f);
        if(ch == '\n')
        {
          lines++;
        }
      }

      fclose(f);
n = read(lotes.newsockfd,&lotes,sizeof(Lotes));



printf("LLEGO %s: \n",lotes.ctaOrigen);
printf("LLEGO %s: \n",lotes.ctaDestino);
printf("LLEGO %lf : \n",lotes.Cantidad);



if (n < 0){
        error("ERROR de lectura del socket");
      }
printf("Cliente canalizado, esperando a que finalice actividad para escribir en log.txt:\n");
//Se realiza la conexion con el servidor 2
int sockfd, portno;
struct sockaddr_in serv_addr;
struct hostent *server;
portno = lotes.portno;
sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0)
        printf("ERROR al abrir socket");
server = gethostbyname(lotes.hostname);
if (server == NULL) {
        fprintf(stderr,"ERROR, no existe host\n");
        exit(0);
}
bzero((char *) &serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
serv_addr.sin_port = htons(portno);
//Se conecta con el servidor
if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        perror("ERROR al conectar");
n = write(sockfd,&lotes.Operacion,sizeof(int));
if (n < 0) {
        printf("ERROR al indicar la operacion ");
}

n = write(sockfd,&lotes,sizeof(Lotes));
if (n < 0) {
        printf("ERROR al escribir en el socket");
}
printf("ENVIE %s: \n",lotes.ctaOrigen);
printf("ENVIE %s: \n",lotes.ctaDestino);
printf("ENVIE %lf : \n",lotes.Cantidad);

//Se lee del socket la confirmacion
Log RecibidoLog;
n = read(sockfd,&RecibidoLog,sizeof(Log));
if (n < 0) {
        printf("ERROR al leer del socket");
}
printf("Imprimo la primera estructura\n");
printf("ID:%d\n",RecibidoLog.ID);
printf("Type:%d\n",RecibidoLog.type);
printf("Cuenta:%s\n",RecibidoLog.cuenta );
printf("OldSaldo:%lf\n",RecibidoLog.OldSaldo );
printf("NewSaldo:%lf\n",RecibidoLog.NewSaldo );
printf("Cantidad:%lf\n",RecibidoLog.Cantidad );
printf("El estado de la transaccion es: %c\n",RecibidoLog.Estado);
printf("sizeof:%ld\n",sizeof(Log));
printf("Ya debi haber impreso toda la primera estructura\n");

f =fopen("BACK.txt","a+");
fprintf(f, "\n%d\t\t", lines);
fprintf(f, "%d\t\t", RecibidoLog.type);
fprintf(f, "%s\t\t", RecibidoLog.cuenta);
fprintf(f, "%lf\t\t", RecibidoLog.OldSaldo);
fprintf(f, "%lf\t\t",  RecibidoLog.NewSaldo );
fprintf(f, "%lf\t\t", RecibidoLog.Cantidad);
fprintf(f, "%c", RecibidoLog.Estado);
fclose(f);
//Se elee la segunda estructura
n = read(sockfd,&RecibidoLog,sizeof(Log));
if (n < 0) {
        printf("ERROR al leer del socket");
}
printf("Imprimo la segunda estructura\n");
printf("ID:%d\n",RecibidoLog.ID);
printf("Type:%d\n",RecibidoLog.type);
printf("Cuenta:%s\n",RecibidoLog.cuenta );
printf("OldSaldo:%lf\n",RecibidoLog.OldSaldo );
printf("NewSaldo:%lf\n",RecibidoLog.NewSaldo );
printf("Cantidad:%lf\n",RecibidoLog.Cantidad );
printf("El estado de la transaccion es: %c\n",RecibidoLog.Estado);
printf("sizeof:%ld\n",sizeof(Log));
printf("Ya debi haber impreso toda la segunda estructura\n");


      f =fopen("BACK.txt","a+");
      fprintf(f, "\n%d\t\t", lines+1);
      fprintf(f, "%d\t\t", RecibidoLog.type);
      fprintf(f, "%s\t\t", RecibidoLog.cuenta);
      fprintf(f, "%lf\t\t", RecibidoLog.OldSaldo);
      fprintf(f, "%lf\t\t",  RecibidoLog.NewSaldo );
      fprintf(f, "%lf\t\t", RecibidoLog.Cantidad);
      fprintf(f, "%c", RecibidoLog.Estado);
      fclose(f);
		Respuesta Answ;
        n = read(sockfd,&Answ,sizeof(Respuesta));
        if (n < 0) {
                printf("ERROR al leer del socket");
        }
        printf("%s\n",Answ.mensaje);
        //Se escribe en el socket la confirmacion
        n = write(lotes.newsockfd,&Answ,sizeof(Respuesta));
        if (n < 0)
                error("ERROR de escritura al socket");
close(sockfd);

}
void *Servidor_Hilo_Deposito(void *parametros){
        Data *userdata = (Data *)parametros;
        Data datos;
        datos= *userdata;
        int n;
        FILE *f;
        int ch=0;
        int lines=0;
        f =fopen("BACK.txt","r");
        if (!f){
          f =fopen("BACK.txt","a+");
              }
        while(!feof(f))
              {
                ch = fgetc(f);
                if(ch == '\n')
                {
                  lines++;
                }
              }

              fclose(f);
        char buffer[256];
        n = read(datos.newsockfd,&datos.cuenta,sizeof(datos.cuenta));
        n = read(datos.newsockfd,&datos.saldo,sizeof(double));
        if (n < 0)
                error("ERROR de lectura del socket");
        printf("Cliente canalizado, esperando a que finalice actividad para escribir en log.txt:\n");
        //Se realiza la conexion con el servidor 2
        int sockfd, portno;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        portno = datos.portno;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                printf("ERROR al abrir socket");
        server = gethostbyname(datos.hostname);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no existe host\n");
                exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);
        //Se conecta con el servidor
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                perror("ERROR al conectar");
        n = write(sockfd,&datos.Operacion,sizeof(int));
        if (n < 0) {
                printf("ERROR al indicar la operacion ");
        }
        n = write(sockfd,&datos.cuenta,sizeof(datos.cuenta));
        n = write(sockfd,&datos.saldo,sizeof(double));

        if (n < 0) {
                printf("ERROR al escribir en el socket");
        }

        //Se lee del socket la confirmacion
        Log RecibidoLog;
        n = read(sockfd,&RecibidoLog,sizeof(Log));
        if (n < 0) {
                printf("ERROR al leer del socket");
        }
      printf("Llegue antes de imprimir lo que recibio\n");
      printf("ID:%d\n",RecibidoLog.ID);
      printf("Type:%d\n",RecibidoLog.type);
      printf("Cuenta:%s\n",RecibidoLog.cuenta );
      printf("OldSaldo:%lf\n",RecibidoLog.OldSaldo );
      printf("NewSaldo:%lf\n",RecibidoLog.NewSaldo );
      printf("Cantidad:%lf\n",RecibidoLog.Cantidad );
      printf("El estado de la transaccion es: %c\n",RecibidoLog.Estado);
      printf("sizeof:%ld\n",sizeof(Log));
      printf("Ya debi haber impreso el estado\n");


            f =fopen("BACK.txt","a+");
            fprintf(f, "\n%d\t\t", lines);
            fprintf(f, "%d\t\t", RecibidoLog.type);
            fprintf(f, "%s\t\t", RecibidoLog.cuenta);
            fprintf(f, "%lf\t\t", RecibidoLog.OldSaldo);
            fprintf(f, "%lf\t\t",  RecibidoLog.NewSaldo );
            fprintf(f, "%lf\t\t", RecibidoLog.Cantidad);
            fprintf(f, "%c", RecibidoLog.Estado);
            fclose(f);
        Respuesta Answ;
        n = read(sockfd,&Answ,sizeof(Respuesta));
        if (n < 0) {
                printf("ERROR al leer del socket");
        }
        printf("%s\n",Answ.mensaje);
        //Se escribe en el socket la confirmacion
        n = write(datos.newsockfd,&Answ,sizeof(Respuesta));
        if (n < 0)
                error("ERROR de escritura al socket");
        close(sockfd);
}

void *Servidor_Hilo_Retiro(void *parametros){
        Data *userdata = (Data *)parametros;
        Data datos;
        datos= *userdata;
        int n;
        FILE *f;
        int ch=0;
        int lines=0;
        f =fopen("BACK.txt","r");
        if (!f){
          f =fopen("BACK.txt","a+");
              }
        while(!feof(f))
              {
                ch = fgetc(f);
                if(ch == '\n')
                {
                  lines++;
                }
              }

              fclose(f);
        char buffer[256];
        n = read(datos.newsockfd,&datos.cuenta,sizeof(datos.cuenta));
        n = read(datos.newsockfd,&datos.saldo,sizeof(double));
        if (n < 0)
                error("ERROR de lectura del socket");
        printf("Cliente canalizado, esperando a que finalice actividad para escribir en log.txt:\n");
        //Se realiza la conexion con el servidor 2
        int sockfd, portno;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        portno = datos.portno;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                printf("ERROR al abrir socket");
        server = gethostbyname(datos.hostname);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no existe host\n");
                exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);
        //Se conecta con el servidor
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                perror("ERROR al conectar");
        n = write(sockfd,&datos.Operacion,sizeof(int));
        if (n < 0) {
                printf("ERROR al indicar la operacion ");
        }
        n = write(sockfd,&datos.cuenta,sizeof(datos.cuenta));
        n = write(sockfd,&datos.saldo,sizeof(double));

        if (n < 0) {
                printf("ERROR al escribir en el socket");
        }

        //Se lee del socket la confirmacion
        Log RecibidoLog;
        n = read(sockfd,&RecibidoLog,sizeof(Log));
        if (n < 0) {
                printf("ERROR al leer del socket");
        }
      printf("Llegue antes de imprimir lo que recibio\n");
      printf("ID:%d\n",RecibidoLog.ID);
      printf("Type:%d\n",RecibidoLog.type);
      printf("Cuenta:%s\n",RecibidoLog.cuenta );
      printf("OldSaldo:%lf\n",RecibidoLog.OldSaldo );
      printf("NewSaldo:%lf\n",RecibidoLog.NewSaldo );
      printf("Cantidad:%lf\n",RecibidoLog.Cantidad );
      printf("El estado de la transaccion es: %c\n",RecibidoLog.Estado);
      printf("sizeof:%ld\n",sizeof(Log));
      printf("Ya debi haber impreso el estado\n");

      f =fopen("BACK.txt","a+");
      fprintf(f, "\n%d\t\t", lines);
      fprintf(f, "%d\t\t", RecibidoLog.type);
      fprintf(f, "%s\t\t", RecibidoLog.cuenta);
      fprintf(f, "%lf\t\t", RecibidoLog.OldSaldo);
      fprintf(f, "%lf\t\t",  RecibidoLog.NewSaldo );
      fprintf(f, "%lf\t\t", RecibidoLog.Cantidad);
      fprintf(f, "%c", RecibidoLog.Estado);
      fclose(f);
       Respuesta Answ;
        n = read(sockfd,&Answ,sizeof(Respuesta));
        if (n < 0) {
                printf("ERROR al leer del socket");
        }
        printf("%s\n",Answ.mensaje);
        //Se escribe en el socket la confirmacion
        n = write(datos.newsockfd,&Answ,sizeof(Respuesta));
        if (n < 0)
                error("ERROR de escritura al socket");
        close(sockfd);
}



int main(int argc, char *argv[])
{
        //Descriptores de archivos. Variavles que almacenan los valores
        //retornados por la llamada al sistema de sockets y aceptacin de llamadas.
        int sockfd, newsockfd;
        //Almacena el nmero de puerto sobre el cual el servidor aceptar conecciones.
        int portno;
        //Almacena el tamao de la direccin del cliente.
        //Es necesaria para aceptar llamadas de sistema.
        socklen_t clilen;
        char buffer[256];
        Data datos;
        Lotes lotes;
        int Operacion;
        //El servidor lee caracteres desde la coneccin de soccets en este buffer.
        struct sockaddr_in serv_addr, cli_addr;
        int n,Salir=0;
        if (argc < 2) {
                fprintf(stderr,"ERROR, no se ha proveido puerto\n");
                exit(1);
        }
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                error("ERROR al abrir socket");
        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);
        datos.portno=atoi(argv[3]);
        datos.hostname=argv[2];
        lotes.portno=atoi(argv[3]);
        lotes.hostname=argv[2];
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
        //Enlaza un socket con una direccin.
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                error("ERROR en enlazar");
        //Listen() permite al proceso escuchar sobre un socket por una coneccin.
        //El primer argumento es el descriptor del socket.
        //El segundo es el tamao de la cola de registro, es decir es el nmero de conecciones que
        //puede aceptar mientras el proceso esta manejando una coneccin en particular.
        //Este debera ser establecido en 5, el tamao mximo permitido por la mayora de los sistemas.
        listen(sockfd,5);
        while(1) {
                //La llamada al sistema accept() causa que el procese se bloquee hasta que un cliente se
                //conecte con el servidor.
                printf("Esperando  clientes\n");
                clilen = sizeof(cli_addr);
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0)
                        error("ERROR en aceptar coneccines");
                else
                        Salir=0;
                //Se lee el tipo de operacion.
                while(Salir==0) {
                        n = read(newsockfd,&Operacion,sizeof(int));
                        if (n < 0)
                                error("ERROR de lectura del socket");
                        switch (Operacion) {
                        case 1:
                                datos.Operacion=Operacion;
                                datos.newsockfd=newsockfd;
                                pthread_t hiloAlta;
                                pthread_create(&hiloAlta, NULL, Servidor_Hilo_Alta, &datos);
                                pthread_join(hiloAlta,NULL);
                                break;
                        case 2:
                                datos.Operacion=Operacion;
                                datos.newsockfd=newsockfd;
                                pthread_t hiloConsulta;
                                pthread_create(&hiloConsulta, NULL, Servidor_Hilo_Consulta, &datos);
                                pthread_join(hiloConsulta,NULL);
                                break;
                        case 3:
                                datos.Operacion=Operacion;
                                datos.newsockfd=newsockfd;
                                pthread_t hiloActualizar;
                                pthread_create(&hiloActualizar, NULL, Servidor_Hilo_Actualizar, &datos);
                                pthread_join(hiloActualizar,NULL);
                                break;
                        case 4:
                                datos.Operacion=Operacion;
                                datos.newsockfd=newsockfd;
                                pthread_t hiloDeposito;
                                pthread_create(&hiloDeposito, NULL, Servidor_Hilo_Deposito, &datos);
                                pthread_join(hiloDeposito,NULL);
                                break;
                        case 5:
                                datos.Operacion=Operacion;
                                datos.newsockfd=newsockfd;
                                pthread_t hiloRetiro;
                                pthread_create(&hiloRetiro, NULL, Servidor_Hilo_Retiro, &datos);
                                pthread_join(hiloRetiro,NULL);
                                break;
                        case 6:
                                lotes.Operacion=Operacion;
                                lotes.newsockfd=newsockfd;
                                pthread_t hiloTransferencia;
                                pthread_create(&hiloTransferencia, NULL, Servidor_Hilo_Transferencia, &lotes);
                                pthread_join(hiloTransferencia,NULL);
                        break;
                        case 7:
                              lotes.Operacion=Operacion;
                              lotes.newsockfd=newsockfd;
                              pthread_t hiloLote;
                              pthread_create(&hiloLote, NULL, Servidor_Hilo_Lotes, &lotes);
                              pthread_join(hiloLote,NULL);
                              break;


                        case 8:
                                printf("Adios popo\n");
                                Salir=1;
                                break;
                        }
                }
        }

        return 0;
}
