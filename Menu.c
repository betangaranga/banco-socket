#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "generador_cuentas.h"


typedef struct _Datos {
        char nombre[50];
        char email[50];
        char cuenta[10];
        double saldo;
        char estatus;
        long id;
}Data;

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
typedef struct {
        int ID;
        int type;
        char cuenta[10];
        double OldSaldo;
        double NewSaldo;
        double Cantidad;
        char Estado;

}Log;
int main(int argc, char const *argv[]) {
        /* code */
        int opcion;
        long id;
        Data datos;
        Lote lote;
        int sockfd, portno;
        int n,i,j=0;
        FILE *r;
        long fileSize;
        char nombre_lote[50];

        Respuesta Answ;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        portno = atoi(argv[2]);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                printf("ERROR al abrir socket");
        server = gethostbyname(argv[1]);
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
        while(1) {
                printf("\t\t\t\tBIENVENIDO AL BANCO\t\t\t\t\t \n");
                printf("\n1 : Alta\n");
                printf("\n2 : Consulta\n");
                printf("\n3 : Actualizacion\n");
                printf("\n4 : Deposito\n");
                printf("\n5 : Retiro\n");
                printf("\n6 : Transferencia\n");
                printf("\n7 : Archivo por Lotes\n");
                printf("\n8 : Salir\n");
                scanf("%d",&opcion);
                n = write(sockfd,&opcion,sizeof(int));
                if (n < 0) {
                        printf("ERROR al indicar la opcion de opcion");
                }
                switch (opcion) {
                case 1:
                        printf("Ingrese Nombre:\n");
                        scanf("%s",datos.nombre);
                        fflush(stdin);
                        printf("Ingrese Email:\n");
                        scanf("%s",datos.email);
                        datos.estatus='A';
                        datos.saldo=0;
                        strcpy(datos.cuenta,randstring(9));
                        printf("TU NUMERO DE CUENTA GENERADO ES : %s\n",datos.cuenta );
                        n = write(sockfd,&datos,sizeof(Data));
                        bzero(datos.nombre,sizeof(datos.nombre));
                        bzero(datos.cuenta,sizeof(datos.cuenta));
                        bzero(datos.email,sizeof(datos.email));
                        if (n < 0) {
                                printf("ERROR al escribir en el socket");
                        }
                        n = read(sockfd,&Answ,sizeof(Respuesta));
                        if (n < 0) {
                                printf("ERROR al leer del socket");
                        }
                        printf("%s\n",Answ.mensaje);
                        printf("La nueva cuenta creada es %s\n Recuerdela bien!\n", Answ.Cuenta);

                        printf("Introduzca 's' para continuar\n");
                        while ( getchar() != 's') ;

                        break;
                case 2:
                        printf("Ingrese la cuenta a consultar:\n");
                        scanf("%s",&datos.cuenta);
                        n = write(sockfd,&datos.cuenta,sizeof(datos.cuenta));
                        if (n < 0) {
                                printf("ERROR al escribir en el socket");
                        }
                        Log EnviarLog2;
                        n = read(sockfd,&EnviarLog2,sizeof(Log));
                        if (n < 0) {
                                printf("ERROR al recuperar datos de consulta");
                        }
                        if(EnviarLog2.Estado=='Y')
                                printf("Resultados de su consulta\n\nCuenta:\t%s\nID:\t%d\nSaldo:\t%lf\n",EnviarLog2.cuenta,EnviarLog2.ID,EnviarLog2.NewSaldo);
                        else
                                printf("Lo sentimos la cuenta que ingreso no es valida. Verifique el numero de cuenta\n");
                        printf("\nIntroduzca 's' para continuar\n");
                        while ( getchar() != 's') ;
                        break;
                case 3:
                        printf("Ingrese  la cuenta a actualizar:\n");
                        scanf("%s",&datos.cuenta);

                        printf("Ingrese Nombre:\n");
                        scanf("%s",datos.nombre);
                        fflush(stdin);
                        printf("Ingrese Email:\n");
                        scanf("%s",datos.email);
                        datos.estatus='A';
                        datos.saldo=0;

                        n = write(sockfd,&datos,sizeof(Data));
                        n = read(sockfd,&Answ,sizeof(Respuesta));
                        if (n < 0) {
                                printf("ERROR al leer del socket");
                        }
                        printf("%s con cuenta %s\n",Answ.mensaje,Answ.Cuenta);
                        printf("Introduzca 's' para continuar");
                        while ( getchar() != 's') ;
                        break;
                case 4:
                        printf("Ingrese  la cuenta a la cual depositara:\n");
                        scanf("%s",&datos.cuenta);
                        printf("Cuanto depositaras?\n");
                        scanf("%lf",&datos.saldo);
                        write(sockfd,&datos.cuenta,sizeof(datos.cuenta));
                        write(sockfd,&datos.saldo,sizeof(datos.saldo));
                        n = read(sockfd,&Answ,sizeof(Respuesta));
                        if (n < 0) {
                                printf("ERROR al leer del socket");
                        }
                        printf("%s\n",Answ.mensaje);
                        printf("Realice consulta en cuenta %s\n", Answ.Cuenta);
                        printf("Introduzca 's' para continuar");
                        while ( getchar() != 's') ;
                        break;
                case 5:
                        printf("Ingrese la cuenta en la cual retirara :\n");
                        scanf("%s",&datos.cuenta);
                        printf("Cuanto depositaras?\n");
                        scanf("%lf",&datos.saldo);
                        write(sockfd,&datos.cuenta,sizeof(datos.cuenta));
                        write(sockfd,&datos.saldo,sizeof(datos.saldo));
                        n = read(sockfd,&Answ,sizeof(Respuesta));
                        if (n < 0) {
                                printf("ERROR al leer del socket");
                        }
                        printf("%s\n",Answ.mensaje);
                        printf("Realice consulta en cuenta %s\n", Answ.Cuenta);
                        printf("Introduzca 's' para continuar");
                        while ( getchar() != 's') ;
                        break;
                case 6:
                        printf("Ingrese la cuenta de Origen :\n");
                        scanf("%s",&lote.ctaOrigen);

                        printf("Ingrese la cuenta de Destino :\n");
                        scanf("%s",&lote.ctaDestino);
                        fflush(stdin);
                        printf("Monto a transferir\n");
                        fflush(stdin);

                        scanf("%lf",&lote.Cantidad);
                        fflush(stdin);

                        lote.type=4;
                        lote.ID=4;
                        printf("ORIGEN %s\n",lote.ctaOrigen);
                        printf("DESTINO %s\n",lote.ctaDestino);
                        write(sockfd,&lote,sizeof(Lote));
                        n = read(sockfd,&Answ,sizeof(Respuesta));
                        if (n < 0) {
                                printf("ERROR al leer del socket");
                        }
                        printf("%s\n",Answ.mensaje);
                        printf("Realice consulta en ambas cuentas\n");
                        printf("Introduzca 's' para continuar");
                        while ( getchar() != 's') ;
                        break;
                case 7:
                        printf("Ingrese el nombre del archivo por lotes a cargar\n");
                        scanf("%s",&nombre_lote);


                        r=fopen(nombre_lote,"rb");
                        if(!r) {
                                printf("El archivo no existe\n");
                        }
                        else{
                                fseek(r,0L,SEEK_END);
                                fileSize=ftell(r);
                                printf("fileSize ;%ld\n",fileSize );
                                for(i=0; i<(fileSize/sizeof(Lote)); i++) {
                                        j++;
                                }
                                Lote lotes[j];
                                write(sockfd,&j,sizeof(int));

                                write(sockfd,&fileSize,sizeof(long));

                                for(i=0; i<(fileSize/sizeof(Lote)); i++) {
                                        fseek(r,((i)*sizeof(Lote))+32,SEEK_SET);
                                        fread(&lotes[i],sizeof(Lote),1,r);
                                        rewind(r);
                                }

                                write(sockfd,lotes,sizeof(lotes));

                        }
                        fclose(r);


                        break;

                case 8:
                        close(sockfd);
                        exit(0);
                        break;
                }
                system("/usr/bin/clear");

        }
        close(sockfd);
        return 0;
}
