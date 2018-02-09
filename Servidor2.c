#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

pthread_mutex_t Fmutex;
typedef struct _Datos {
								char nombre[50];
								char email[50];
								char cuenta[10];
								double saldo;
								char estatus;
								long id;
								struct _Datos *sig;
}Data;
typedef struct _DatosRecibidos {
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
								Data *lista;
}DataRecibida;
typedef struct {
								long fileSize;
								long dataSize;
								short offSetData;
								short headerSize;
								short regSize;
								char type[4]; //Puede ser Data o Lote
}Header;
typedef struct {
								int ID;
								int type; //1.Consulta, 2.Deposito. 3.Retiro 4.Tranferencia
								char ctaOrigen[11];
								char ctaDestino[11];
								double Cantidad;
								int newsockfd;
								int portno;
								char *hostname;
								int Operacion;
}Lotes;
typedef struct {
								int ID;
								int type; //1.Consulta, 2.Deposito. 3.Retiro 4.Tranferencia
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

typedef struct {
								int ID1;
								int type1;
								char cuenta1[10];
								double OldSaldo1;
								double NewSaldo1;
								double Cantidad1;
								char Estado1;
								int ID2;
								int type2;
								char cuenta2[10];
								double OldSaldo2;
								double NewSaldo2;
								double Cantidad2;
								char Estado2;

}LogTransferencia;

void error(const char *msg)
{
								perror(msg);
								exit(1);
}
Log Buscar(Data *datos, char cuenta[10])
{
								Log EnviarLog;
								char Hecho='Y';
								char Negativo='N';

								while (datos!=NULL && strcmp(datos->cuenta, cuenta) != 0)
								{
																datos = datos->sig;
								}

								if (datos != NULL && strcmp(datos->cuenta, cuenta)==0)
								{
																printf("CUENTA ENCONTRADA:\n");
																printf("\nNombre: %s",datos->nombre);
																printf("\nCorreo: %s\n",datos->email);
																printf("\nCuenta: %s\n",datos->cuenta);
																printf("\nSaldo: %lf\n",datos->saldo);
																printf("\nEstatus: %c\n",datos->estatus);
																printf("\nID: %ld\n",datos->id);
																EnviarLog.ID=datos->id;
																EnviarLog.type=2;
																strcpy(EnviarLog.cuenta,datos->cuenta);
																//EnviarLog.cuenta=datos->cuenta;
																EnviarLog.OldSaldo=datos->saldo;
																EnviarLog.NewSaldo=datos->saldo;
																EnviarLog.Cantidad=0;
																strcpy(&EnviarLog.Estado,&Hecho);

								}

								else{
																printf("CUENTA NO ENCONTRADA:\n");
																//printf("\nNombre: %s",datos->nombre);
																//printf("\nCorreo: %s\n",datos->email);
																//printf("\nCuenta: %s\n",datos->cuenta);
																//printf("\nSaldo: %lf\n",datos->saldo);
																//printf("\nEstatus: %c\n",datos->estatus);
																//printf("\nID: %ld\n",datos->id);
																EnviarLog.ID=0;
																printf("\nID: %d\n",EnviarLog.ID);
																EnviarLog.type=2;
																printf("\nID: %d\n",EnviarLog.type);
																strcpy(EnviarLog.cuenta,cuenta);
																printf("\nCuenta: %s\n",EnviarLog.cuenta);
																//EnviarLog.cuenta=cuenta;
																EnviarLog.OldSaldo=0;
																EnviarLog.NewSaldo=0;
																EnviarLog.Cantidad=0;
																strcpy(&EnviarLog.Estado,&Negativo);
								}
								return EnviarLog;
}
LogTransferencia Tranferencia(Data *datos, char cuentaOrigen[10],char cuentaDestino[10],double monto,FILE *f,size_t tamano){
								int n;
								LogTransferencia EnviarLog;
								char Hecho='Y';
								char Negativo='N';
								Data *aux1=datos;
								long idOrigen,idDestino;
								double saldoOrigen,saldoDestino;
								int bandera1,bandera2;
								while (datos!=NULL && strcmp(datos->cuenta, cuentaOrigen) != 0)
								{
																datos = datos->sig;
								}
								if (datos != NULL && strcmp(datos->cuenta, cuentaOrigen)==0)
								{
																idOrigen=datos->id;
																saldoOrigen=datos->saldo;
																bandera1=1;
								}
								else{
																printf("CUENTA DE ORIGEN NO EXISTE\n");
																EnviarLog.ID1=0;
																EnviarLog.type1=6;
																strcpy(EnviarLog.cuenta1,cuentaOrigen);
																EnviarLog.OldSaldo1=0;
																EnviarLog.NewSaldo1=0;
																EnviarLog.Cantidad1=monto;
																strcpy(&EnviarLog.Estado1,&Negativo);
																EnviarLog.ID2=0;
																EnviarLog.type2=6;
																strcpy(EnviarLog.cuenta2,cuentaDestino);
																EnviarLog.OldSaldo2=0;
																EnviarLog.NewSaldo2=0;
																EnviarLog.Cantidad2=monto;
																strcpy(&EnviarLog.Estado2,&Negativo);

								}
								while (aux1!=NULL && strcmp(aux1->cuenta, cuentaDestino) != 0)
								{
																aux1 = aux1->sig;
								}
								if (aux1 != NULL && strcmp(aux1->cuenta, cuentaDestino)==0)
								{
																idDestino=aux1->id;
																saldoDestino=aux1->saldo;
																bandera2=1;
								}
								else{
																printf("CUENTA DE DESTINO NO EXISTE\n");
																EnviarLog.ID1=0;
																EnviarLog.type1=6;
																strcpy(EnviarLog.cuenta1,cuentaOrigen);
																EnviarLog.OldSaldo1=0;
																EnviarLog.NewSaldo1=0;
																EnviarLog.Cantidad1=monto;
																strcpy(&EnviarLog.Estado1,&Negativo);
																EnviarLog.ID2=0;
																EnviarLog.type2=6;
																strcpy(EnviarLog.cuenta2,cuentaDestino);
																EnviarLog.OldSaldo2=0;
																EnviarLog.NewSaldo2=0;
																EnviarLog.Cantidad2=monto;
																strcpy(&EnviarLog.Estado2,&Negativo);

								}
								printf("ID CUENTA DE ORIGEN--%ld--\n",idOrigen);
								printf("ID CUENTA DE DESTINO--%ld--\n",idDestino);
								printf("SALDO CUENTA DE ORIGEN--%lf--\n",saldoOrigen);
								printf("SALDO CUENTA DE DESTINO--%lf--\n",saldoDestino);
								if(bandera1==1&&bandera2==1) {
																if(saldoOrigen<monto) {
																								printf("El saldo de la cuenta de origen es insuficiente para realizar la transferencia\n");
																}
																else{
																								EnviarLog.OldSaldo2=aux1->saldo;
																								EnviarLog.NewSaldo2=aux1->saldo+monto;
																								EnviarLog.OldSaldo1=datos->saldo;
																								EnviarLog.NewSaldo1=datos->saldo-monto;
																								datos->saldo=datos->saldo-monto;
																								aux1->saldo=aux1->saldo+monto;
																								f =fopen("DATA.bin","rb+");
																								fseek(f,((idOrigen)*tamano)+32,SEEK_SET);
																								n= fwrite(datos,tamano,1,f);
																								if(n<0) {
																																error("ALGO SALIO MAL AL ACTUALIZAR LA CUENTA DE ORIGEN");
																																EnviarLog.ID1=0;
																																EnviarLog.type1=6;
																																strcpy(EnviarLog.cuenta1,cuentaOrigen);
																																EnviarLog.OldSaldo1=0;
																																EnviarLog.NewSaldo1=0;
																																EnviarLog.Cantidad1=monto;
																																strcpy(&EnviarLog.Estado1,&Negativo);

																								}
																								else{
																																EnviarLog.ID1=0;
																																EnviarLog.type1=6;
																																strcpy(EnviarLog.cuenta1,cuentaOrigen);
																																EnviarLog.Cantidad1=monto;
																																strcpy(&EnviarLog.Estado1,&Hecho);

																								}
																								rewind(f);
																								fseek(f,((idDestino)*tamano)+32,SEEK_SET);
																								n= fwrite(aux1,tamano,1,f);
																								if(n<0) {
																																error("ALGO SALIO MAL AL ACTUALIZAR LA CUENTA DE DESTINO");
																																EnviarLog.ID2=0;
																																EnviarLog.type2=6;
																																strcpy(EnviarLog.cuenta2,cuentaDestino);
																																EnviarLog.OldSaldo2=0;
																																EnviarLog.NewSaldo2=0;
																																EnviarLog.Cantidad2=monto;
																																strcpy(&EnviarLog.Estado2,&Negativo);
																								}
																								else{
																																EnviarLog.ID2=0;
																																EnviarLog.type2=6;
																																strcpy(EnviarLog.cuenta2,cuentaDestino);
																																EnviarLog.Cantidad2=monto;
																																strcpy(&EnviarLog.Estado2,&Hecho);
																								}


																								fclose(f);
																								printf("NUEVO SALDO CUENTA DE ORIGEN--%lf--\n",datos->saldo);
																								printf("NUEVO SALDO CUENTA DE DESTINO--%lf--\n",aux1->saldo);


																}
								}


								return EnviarLog;

}
Log Deposito(Data *datos, char cuenta[10],double saldo,FILE *f,size_t tamano){
								int n;
								Log EnviarLog;
								char Hecho='Y';
								char Negativo='N';
								while (datos!=NULL && strcmp(datos->cuenta, cuenta) != 0)
								{
																datos = datos->sig;
								}
								if (datos != NULL && strcmp(datos->cuenta, cuenta)==0)
								{
																printf("CUENTA ENCONTRADA:\n");
																printf("Esto voy a avanzar en el archivo -%ld-:\n",((datos->id)*tamano)+32);
																printf("datos->id -%ld-:\n",datos->id);

																EnviarLog.OldSaldo=datos->saldo;
																EnviarLog.NewSaldo=datos->saldo+saldo;
																EnviarLog.Cantidad=saldo;
																datos->saldo=datos->saldo+saldo;
																printf("\nSaldo: %lf\n",datos->saldo);
																f =fopen("DATA.bin","rb+");
																fseek(f,((datos->id)*tamano)+32,SEEK_SET);
																n= fwrite(datos,tamano,1,f);
																if(n<0) {
																								error("ALGO SALIO MAL");
																}
																fclose(f);
																EnviarLog.ID=0;
																EnviarLog.type=4;
																strcpy(EnviarLog.cuenta,datos->cuenta);
																strcpy(&EnviarLog.Estado,&Hecho);

								}
								else{

																printf("Cuenta no ENCONTRADA\n");
																EnviarLog.ID=0;
																EnviarLog.type=4;
																strcpy(EnviarLog.cuenta,cuenta);
																EnviarLog.OldSaldo=0;
																EnviarLog.NewSaldo=0;
																EnviarLog.Cantidad=saldo;
																strcpy(&EnviarLog.Estado,&Negativo);
								}

								return EnviarLog;
}
Log Retiro(Data *datos, char cuenta[10],double saldo,FILE *f,size_t tamano){
								int n;
								Log EnviarLog;
								char Hecho='Y';
								char Negativo='N';
								while (datos!=NULL && strcmp(datos->cuenta, cuenta) != 0)
								{
																datos = datos->sig;
								}
								if (datos != NULL && strcmp(datos->cuenta, cuenta)==0)
								{
																printf("CUENTA ENCONTRADA:\n");
																printf("Esto voy a avanzar en el archivo -%ld-:\n",((datos->id)*tamano)+32);
																printf("datos->id -%ld-:\n",datos->id);
																if(datos->saldo<saldo) {
																								printf("No tienes saldo suficiente :D\n");
																								EnviarLog.ID=0;
																								EnviarLog.type=5;
																								strcpy(EnviarLog.cuenta,datos->cuenta);
																								EnviarLog.OldSaldo=datos->saldo;
																								EnviarLog.NewSaldo=datos->saldo;
																								EnviarLog.Cantidad=saldo;
																								strcpy(&EnviarLog.Estado,&Negativo);
																}
																else{


																								EnviarLog.OldSaldo=datos->saldo;
																								EnviarLog.NewSaldo=datos->saldo-saldo;
																								EnviarLog.Cantidad=saldo;
																								datos->saldo=datos->saldo-saldo;
																								printf("\nSaldo: %lf\n",datos->saldo);
																								f =fopen("DATA.bin","rb+");
																								fseek(f,((datos->id)*tamano)+32,SEEK_SET);
																								n= fwrite(datos,tamano,1,f);
																								if(n<0) {
																																error("ALGO SALIO MAL");
																								}
																								fclose(f);
																								EnviarLog.ID=0;
																								EnviarLog.type=5;
																								strcpy(EnviarLog.cuenta,datos->cuenta);
																								strcpy(&EnviarLog.Estado,&Hecho);
																}

								}

								else{
																printf("No se Encontro la cuenta\n");
																EnviarLog.ID=0;
																EnviarLog.type=5;
																strcpy(EnviarLog.cuenta,cuenta);
																EnviarLog.OldSaldo=0;
																EnviarLog.NewSaldo=0;
																EnviarLog.Cantidad=saldo;
																strcpy(&EnviarLog.Estado,&Negativo);

								}

								return EnviarLog;

}
int Contar(Data *lista){
								int c=0;
								while(lista!=NULL) {
																lista=lista->sig;
																c++;
								}
								return c;
}
Data* CrearNodo(char *nombre,char *email,char *cuenta,double saldo,char estatus,long id){

								Data *entrada;
								entrada=(Data*)malloc (sizeof(Data));
								strcpy(entrada->nombre,nombre);
								strcpy(entrada->email,email);
								strcpy(entrada->cuenta,cuenta);
								entrada->saldo=saldo;
								entrada->estatus=estatus;
								entrada->id=id;
								entrada->sig=NULL;
								return entrada;
}
Data* InsertarFinalLista(Data *lista,char *nombre,char *email,char *cuenta,double saldo,char estatus,long id){
								Data *nuevo;
								Data *aux = lista;
								nuevo=CrearNodo(nombre,email,cuenta,saldo,estatus,id);
								if(lista!=NULL) {
																while(aux->sig!=NULL)
																{
																								aux=aux->sig;
																}
																aux->sig=nuevo;
								}
								else
																lista=nuevo;
								return lista;
}
void VerLista(Data *datos){
								if(datos==NULL) {
																printf("Lista Vacia");
								}
								else{
																while(datos!=NULL) {
																								printf("\nNombre: %s",datos->nombre);
																								printf("\nCorreo: %s\n",datos->email);
																								printf("\nCuenta: %s\n",datos->cuenta);

																								printf("\nSaldo: %lf\n",datos->saldo);
																								printf("\nEstatus: %c\n",datos->estatus);
																								printf("\nID: %ld\n",datos->id);


																								datos=datos->sig;
																}
								}
}
Log Actualizar(Data *datos, char cuenta[10],char nombre[50],char email[50],FILE *f,size_t tamano){
								int n;
								Log EnviarLog;
								char Hecho='Y';
								char Negativo='N';
								while (datos!=NULL && strcmp(datos->cuenta, cuenta) != 0)
								{
																datos = datos->sig;
								}
								if (datos != NULL && strcmp(datos->cuenta, cuenta)==0)
								{
																printf("CUENTA ENCONTRADA:\n");
																printf("Esto voy a avanzar en el archivo -%ld-:\n",((datos->id)*tamano)+32);
																printf("datos->id -%ld-:\n",datos->id);
																strcpy(datos->nombre,nombre);
																strcpy(datos->email,email);


																f =fopen("DATA.bin","rb+");

																fseek(f,((datos->id)*tamano)+32,SEEK_SET);

																n= fwrite(datos,tamano,1,f);
																if(n<0) {
																								error("ALGO SALIO MAL");
																}

																fclose(f);

																EnviarLog.ID=0;
																EnviarLog.type=3;
																strcpy(EnviarLog.cuenta,datos->cuenta);
																EnviarLog.OldSaldo=datos->saldo;
																EnviarLog.NewSaldo=datos->saldo;
																EnviarLog.Cantidad=0;
																strcpy(&EnviarLog.Estado,&Hecho);

								}
								else{
																printf("NO HAY COINCIDENCIAS EN EL SISTEMA\n");
																EnviarLog.ID=0;
																//printf("\nID: %d\n",EnviarLog.ID);
																EnviarLog.type=3;
																//printf("\nID: %d\n",EnviarLog.type);
																strcpy(EnviarLog.cuenta,cuenta);
																//printf("\nCuenta: %s\n",EnviarLog.cuenta);
																//EnviarLog.cuenta=cuenta;
																EnviarLog.OldSaldo=0;
																EnviarLog.NewSaldo=0;
																EnviarLog.Cantidad=0;
																strcpy(&EnviarLog.Estado,&Negativo);

								}

								return EnviarLog;
}
Log Alta(Data datos, char nombre[50],char email[50],char cuenta[10],double saldo,char estatus,long id,FILE *f){
								int n;
								Log EnviarLog;
								char Hecho='Y';
								char Negativo='N';
								f =fopen("DATA.bin","ab+");
								n= fwrite(&datos, sizeof(datos),1,f);

								if(n<0) {
																error("ERROR AL ESCRIBIR\n");
																EnviarLog.ID=0;
																EnviarLog.type=1;
																strcpy(EnviarLog.cuenta,cuenta);
																EnviarLog.OldSaldo=saldo;
																EnviarLog.NewSaldo=saldo;
																EnviarLog.Cantidad=0;
																strcpy(&EnviarLog.Estado,&Negativo);
								}
								else{
																EnviarLog.ID=0;
																EnviarLog.type=1;
																strcpy(EnviarLog.cuenta,cuenta);
																EnviarLog.OldSaldo=saldo;
																EnviarLog.NewSaldo=saldo;
																EnviarLog.Cantidad=0;
																strcpy(&EnviarLog.Estado,&Hecho);

								}
								fclose(f);
								return EnviarLog;

}
void *Servidor_Hilo_Lotes(void *parametros){
								DataRecibida *userdata = (DataRecibida *)parametros;
								DataRecibida datosRecibidos;
								datosRecibidos= *userdata;
								int j,i;
								FILE *f;
								Log EnviarLog2;
								LogTransferencia RLog;

								read(datosRecibidos.newsockfd,&j,sizeof(int));
								printf("LLEGO LOOP %d\n",j );
								Lote lote[j];
								read(datosRecibidos.newsockfd,lote,sizeof(lote));
								for(i=0; i<j; i++) {
									switch (lote[i].type) {
										case 1:
										Buscar(datosRecibidos.lista,lote[i].ctaOrigen);
										break;
										case 2:
										EnviarLog2=Deposito(datosRecibidos.lista,lote[i].ctaOrigen,lote[i].Cantidad,f,sizeof(datosRecibidos));
										break;
										case 3:
										EnviarLog2=Retiro(datosRecibidos.lista,lote[i].ctaOrigen,lote[i].Cantidad,f,sizeof(datosRecibidos));
										break;
										case 4:

										RLog=Tranferencia(datosRecibidos.lista,lote[i].ctaOrigen,lote[i].ctaDestino,lote[i].Cantidad,f,sizeof(datosRecibidos));
										break;
									}

								}
								close(datosRecibidos.newsockfd);

								return (void *)datosRecibidos.lista;

}

void *Servidor_Hilo_Alta(void *parametros){
								DataRecibida *userdata = (DataRecibida *)parametros;
								DataRecibida datosRecibidos;
								datosRecibidos= *userdata;
								FILE *f;
								unsigned long int fileSize;
								int n;
								Respuesta Answ;
								Data datos;
								Header header;
								Log EnviarLog2;
								printf("%d\n",datosRecibidos.newsockfd);
								//	int nodo;


								n = read(datosRecibidos.newsockfd,&datosRecibidos,sizeof(DataRecibida));
								strcpy(datos.nombre,datosRecibidos.nombre);
								strcpy(datos.email,datosRecibidos.email);
								strcpy(datos.cuenta,datosRecibidos.cuenta);
								datos.saldo=datosRecibidos.saldo;
								datos.estatus=datosRecibidos.estatus;

								f =fopen("DATA.bin","rb+");

								fseek(f,0L,SEEK_END);
								fileSize=ftell(f);
								fclose(f);

								long id=fileSize/sizeof(datos);
								datos.id=id;
								printf("ID :%ld\n",datos.id);
								EnviarLog2=Alta(datos,datosRecibidos.nombre,datosRecibidos.email,datosRecibidos.cuenta,datosRecibidos.saldo, datosRecibidos.estatus,datos.id, f);
								datosRecibidos.lista=InsertarFinalLista(datosRecibidos.lista,datosRecibidos.nombre,datosRecibidos.email,datosRecibidos.cuenta,datosRecibidos.saldo, datosRecibidos.estatus,id);

								printf("ID:%d\n",EnviarLog2.ID);
								printf("Type:%d\n",EnviarLog2.type);
								printf("Cuenta:%s\n",EnviarLog2.cuenta );
								printf("OldSaldo:%lf\n",EnviarLog2.OldSaldo );
								printf("NewSaldo:%lf\n",EnviarLog2.NewSaldo );
								printf("Cantidad:%lf\n",EnviarLog2.Cantidad );

								printf("El estado de la transaccion es: %c\n",EnviarLog2.Estado);
								printf("sizeof:%ld\n",sizeof(Log));
								n = write(datosRecibidos.newsockfd,&EnviarLog2,sizeof(Log));
								if (n < 0) {
																error("ERROR de escritura al socket");
								}



								strcpy(Answ.mensaje,"Registrado satisfactoriamente");
								bzero(Answ.Cuenta,sizeof(char)*10);
								strcpy(Answ.Cuenta,EnviarLog2.cuenta);
								n = write(datosRecibidos.newsockfd,&Answ,sizeof(Respuesta));
								if (n < 0)
																error("ERROR de escritura al socket");
								close(datosRecibidos.newsockfd);
								return (void *)datosRecibidos.lista;
}

void *Servidor_Hilo_Consulta(void *parametros){
								DataRecibida *userdata = (DataRecibida *)parametros;
								DataRecibida datosRecibidos;
								datosRecibidos= *userdata;
								int n;
								printf("ESTA ES LA LISTA COMPLETA\n");
								VerLista(datosRecibidos.lista);
								n = read(datosRecibidos.newsockfd,&datosRecibidos.cuenta,sizeof(char)*10);
								if (n < 0)
																error("ERROR de lectura del socket");
								printf("LA CUENTA QUE LLEGO ES:%s\n",datosRecibidos.cuenta);
								Log EnviarLog2;
								EnviarLog2=Buscar(datosRecibidos.lista,datosRecibidos.cuenta);
								printf("ID:%d\n",EnviarLog2.ID);
								printf("Type:%d\n",EnviarLog2.type);
								printf("Cuenta:%s\n",EnviarLog2.cuenta );
								printf("OldSaldo:%lf\n",EnviarLog2.OldSaldo );
								printf("NewSaldo:%lf\n",EnviarLog2.NewSaldo );
								printf("Cantidad:%lf\n",EnviarLog2.Cantidad );

								printf("El estado de la transaccion es: %c\n",EnviarLog2.Estado);
								printf("sizeof:%ld\n",sizeof(Log));
								n = write(datosRecibidos.newsockfd,&EnviarLog2,sizeof(Log));
								if (n < 0) {
																error("ERROR al enviar datos de consulta");
								}
								printf("Segun ya envie la info\n");
								close(datosRecibidos.newsockfd);
}

void *Servidor_Hilo_Actualizar(void *parametros){
								DataRecibida *userdata = (DataRecibida *)parametros;
								DataRecibida datosRecibidos;
								datosRecibidos= *userdata;
								int n;
								Respuesta Answ;
								FILE *f;
								n = read(datosRecibidos.newsockfd,&datosRecibidos,sizeof(DataRecibida));
								if (n < 0)
																error("ERROR de lectura del socket");
								printf("\nNombre: %s",datosRecibidos.nombre);
								printf("\nCorreo: %s\n",datosRecibidos.email);

								printf("\nCuenta: %s\n",datosRecibidos.cuenta);
								Log EnviarLog2;
								EnviarLog2=Actualizar(datosRecibidos.lista,datosRecibidos.cuenta,datosRecibidos.nombre,datosRecibidos.email,f,sizeof(datosRecibidos));
								printf("ID:%d\n",EnviarLog2.ID);
								printf("Type:%d\n",EnviarLog2.type);
								printf("Cuenta:%s\n",EnviarLog2.cuenta );
								printf("OldSaldo:%lf\n",EnviarLog2.OldSaldo );
								printf("NewSaldo:%lf\n",EnviarLog2.NewSaldo );
								printf("Cantidad:%lf\n",EnviarLog2.Cantidad );

								printf("El estado de la transaccion es: %c\n",EnviarLog2.Estado);
								printf("sizeof:%ld\n",sizeof(Log));
								n = write(datosRecibidos.newsockfd,&EnviarLog2,sizeof(Log));
								if (n < 0) {
																error("ERROR de escritura al socket");
								}

								strcpy(Answ.mensaje,"Cliente Actualizado");
								bzero(Answ.Cuenta,sizeof(char)*10);
								strcpy(Answ.Cuenta,EnviarLog2.cuenta);
								n = write(datosRecibidos.newsockfd,&Answ,sizeof(Respuesta));
								if (n < 0)
																error("ERROR de escritura al socket");
								close(datosRecibidos.newsockfd);
								return (void *)datosRecibidos.lista;
}


void *Servidor_Hilo_Deposito(void *parametros){
								DataRecibida *userdata = (DataRecibida *)parametros;
								DataRecibida datosRecibidos;
								datosRecibidos= *userdata;
								int n;
								Respuesta Answ;
								FILE *f;
								Log EnviarLog2;
								read(datosRecibidos.newsockfd,&datosRecibidos.cuenta,sizeof(datosRecibidos.cuenta));
								read(datosRecibidos.newsockfd,&datosRecibidos.saldo,sizeof(datosRecibidos.saldo));

								EnviarLog2=Deposito(datosRecibidos.lista,datosRecibidos.cuenta,datosRecibidos.saldo,f,sizeof(datosRecibidos));

								printf("ID:%d\n",EnviarLog2.ID);
								printf("Type:%d\n",EnviarLog2.type);
								printf("Cuenta:%s\n",EnviarLog2.cuenta );
								printf("OldSaldo:%lf\n",EnviarLog2.OldSaldo );
								printf("NewSaldo:%lf\n",EnviarLog2.NewSaldo );
								printf("Cantidad:%lf\n",EnviarLog2.Cantidad );

								printf("El estado de la transaccion es: %c\n",EnviarLog2.Estado);
								printf("sizeof:%ld\n",sizeof(Log));
								n = write(datosRecibidos.newsockfd,&EnviarLog2,sizeof(Log));
								if (n < 0) {
																error("ERROR de escritura al socket");
								}
								strcpy(Answ.mensaje,"Deposito exitoso");
								bzero(Answ.Cuenta,sizeof(char)*10);
								strcpy(Answ.Cuenta,EnviarLog2.cuenta);
								n = write(datosRecibidos.newsockfd,&Answ,sizeof(Respuesta));
								if (n < 0)
																error("ERROR de escritura al socket");

								close(datosRecibidos.newsockfd);

								return (void *)datosRecibidos.lista;
}
void *Servidor_Hilo_Retiro(void *parametros){
								DataRecibida *userdata = (DataRecibida *)parametros;
								DataRecibida datosRecibidos;
								datosRecibidos= *userdata;
								int n;
								Respuesta Answ;
								FILE *f;
								Log EnviarLog2;
								read(datosRecibidos.newsockfd,&datosRecibidos.cuenta,sizeof(datosRecibidos.cuenta));
								read(datosRecibidos.newsockfd,&datosRecibidos.saldo,sizeof(datosRecibidos.saldo));
								EnviarLog2=Retiro(datosRecibidos.lista,datosRecibidos.cuenta,datosRecibidos.saldo,f,sizeof(datosRecibidos));

								printf("ID:%d\n",EnviarLog2.ID);
								printf("Type:%d\n",EnviarLog2.type);
								printf("Cuenta:%s\n",EnviarLog2.cuenta );
								printf("OldSaldo:%lf\n",EnviarLog2.OldSaldo );
								printf("NewSaldo:%lf\n",EnviarLog2.NewSaldo );
								printf("Cantidad:%lf\n",EnviarLog2.Cantidad );

								printf("El estado de la transaccion es: %c\n",EnviarLog2.Estado);
								printf("sizeof:%ld\n",sizeof(Log));
								n = write(datosRecibidos.newsockfd,&EnviarLog2,sizeof(Log));
								if (n < 0) {
																error("ERROR de escritura al socket");
								}
								strcpy(Answ.mensaje,"Retiro exitoso");
								bzero(Answ.Cuenta,sizeof(char)*10);
								strcpy(Answ.Cuenta,EnviarLog2.cuenta);
								n = write(datosRecibidos.newsockfd,&Answ,sizeof(Respuesta));
								if (n < 0)
																error("ERROR de escritura al socket");
								close(datosRecibidos.newsockfd);

								return (void *)datosRecibidos.lista;
}

void *Servidor_Hilo_Transferencia(void *parametros){
								DataRecibida *userdata = (DataRecibida *)parametros;
								DataRecibida datosRecibidos;
								datosRecibidos= *userdata;
								int n;
								Respuesta Answ;
								FILE *f;
								Lotes lotes;
								Log EnviarLog2;
								LogTransferencia RLog;
								read(datosRecibidos.newsockfd,&lotes,sizeof(Lotes));
								printf("La cuenta de origen que llego es: (%s)\n y la de destino(%s) \n",lotes.ctaOrigen,lotes.ctaDestino);
								printf("Cantidad-%lf-\n",lotes.Cantidad );
								RLog=Tranferencia(datosRecibidos.lista,lotes.ctaOrigen,lotes.ctaDestino,lotes.Cantidad,f,sizeof(datosRecibidos));
								EnviarLog2.ID=RLog.ID1;
								EnviarLog2.type=RLog.type1;
								strcpy(EnviarLog2.cuenta,RLog.cuenta1);
								EnviarLog2.OldSaldo=RLog.OldSaldo1;
								EnviarLog2.NewSaldo=RLog.NewSaldo1;
								EnviarLog2.Cantidad=RLog.Cantidad1;
								strcpy(&EnviarLog2.Estado,&RLog.Estado1);
								printf("ID:%d\n",EnviarLog2.ID);
								printf("Type:%d\n",EnviarLog2.type);
								printf("Cuenta:%s\n",EnviarLog2.cuenta );
								printf("OldSaldo:%lf\n",EnviarLog2.OldSaldo );
								printf("NewSaldo:%lf\n",EnviarLog2.NewSaldo );
								printf("Cantidad:%lf\n",EnviarLog2.Cantidad );

								printf("El estado de la transaccion es: %c\n",EnviarLog2.Estado);
								printf("sizeof:%ld\n",sizeof(Log));

								//Envio el primer mensaje
								n = write(datosRecibidos.newsockfd,&EnviarLog2,sizeof(Log));
								if (n < 0) {
																error("ERROR de escritura al socket");
								}

								//Leo la info de la cuenta destino
								EnviarLog2.ID=RLog.ID2;
								EnviarLog2.type=RLog.type2;
								strcpy(EnviarLog2.cuenta,RLog.cuenta2);
								EnviarLog2.OldSaldo=RLog.OldSaldo2;
								EnviarLog2.NewSaldo=RLog.NewSaldo2;
								EnviarLog2.Cantidad=RLog.Cantidad2;
								strcpy(&EnviarLog2.Estado,&RLog.Estado2);
								printf("ID:%d\n",EnviarLog2.ID);
								printf("Type:%d\n",EnviarLog2.type);
								printf("Cuenta:%s\n",EnviarLog2.cuenta );
								printf("OldSaldo:%lf\n",EnviarLog2.OldSaldo );
								printf("NewSaldo:%lf\n",EnviarLog2.NewSaldo );
								printf("Cantidad:%lf\n",EnviarLog2.Cantidad );

								printf("El estado de la transaccion es: %c\n",EnviarLog2.Estado);
								printf("sizeof:%ld\n",sizeof(Log));

								//Envio el segundo mensaje
								n = write(datosRecibidos.newsockfd,&EnviarLog2,sizeof(Log));
								if (n < 0) {
																error("ERROR de escritura al socket");
								}
								strcpy(Answ.mensaje,"Tranferencia exitosa");
								bzero(Answ.Cuenta,sizeof(char)*10);
								strcpy(Answ.Cuenta,EnviarLog2.cuenta);
								n = write(datosRecibidos.newsockfd,&Answ,sizeof(Respuesta));
								if (n < 0)
																error("ERROR de escritura al socket");
								close(datosRecibidos.newsockfd);
								return (void *)datosRecibidos.lista;
}
int main(int argc, char *argv[])
{
								int sockfd, newsockfd;
								int portno;
								FILE *f;
								int aux2;
								unsigned long int fileSize;
								socklen_t clilen;
								char buffer[256];
								int Operacion;
								struct sockaddr_in serv_addr, cli_addr;
								int n;
								Respuesta Answ;
								Data datos;
								Header header;

								//	int nodo;
								Data *lista=NULL;
								void *listaaux;
								DataRecibida datosRecibidos;
								Lotes lotes;
								f =fopen("DATA.bin","a+b");
								fseek(f,0L,SEEK_END);
								fileSize=ftell(f);
								int _id=fileSize/sizeof(datos);

								header.headerSize=sizeof(header);
								header.regSize=sizeof(datos);
								header.offSetData=0;
								header.fileSize=fileSize;
								header.dataSize=header.fileSize-header.headerSize;
								strcpy(header.type,"NULL");
								if (fileSize==0) {
																fwrite(&header,sizeof(header),1,f);
								}
								for(int i=1; i<_id+1; i++) {
																fseek(f,((i-1)*sizeof(datos))+header.headerSize,SEEK_SET);
																fread(&datos,sizeof(datos),1,f);
																lista=InsertarFinalLista(lista,datos.nombre,datos.email,datos.cuenta,datos.saldo,datos.estatus,datos.id);
																rewind(f);
								}
								fclose(f);
								//		Busca(lista);

								//METER DATOS DE HEADER EN FSEEKS!

								if (argc < 2) {
																fprintf(stderr,"ERROR, no se ha proveido puerto\n");
																exit(1);
								}
								sockfd = socket(AF_INET, SOCK_STREAM, 0);
								if (sockfd < 0)
																error("ERROR al abrir socket");
								bzero((char *) &serv_addr, sizeof(serv_addr));
								portno = atoi(argv[1]);
								serv_addr.sin_family = AF_INET;
								serv_addr.sin_addr.s_addr = INADDR_ANY;
								serv_addr.sin_port = htons(portno);
								if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
																error("ERROR en enlazar");
								listen(sockfd,5);

								while(1) {
																//La llamada al sistema accept() causa que el procese se bloquee hasta que un cliente se
																//conecte con el servidor.
																clilen = sizeof(cli_addr);
																newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
																if (newsockfd < 0)
																								error("ERROR en aceptar coneccines");
																//Se lee el tipo de operacion.
																n = read(newsockfd,&Operacion,sizeof(int));
																if (n < 0)
																								error("ERROR de lectura del socket");
																switch (Operacion) {
																case 1:
																								datosRecibidos.lista=lista;
																								datosRecibidos.Operacion=Operacion;
																								datosRecibidos.newsockfd=newsockfd;
																								printf("%d\n",newsockfd);
																								pthread_t hiloAlta;
																								pthread_create(&hiloAlta, NULL, Servidor_Hilo_Alta, &datosRecibidos);
																								pthread_join(hiloAlta,&listaaux);
																								lista=(Data *)listaaux;

																								break;
																case 2:
																								datosRecibidos.lista=lista;
																								datosRecibidos.Operacion=Operacion;
																								datosRecibidos.newsockfd=newsockfd;
																								pthread_t hiloConsulta;
																								pthread_create(&hiloConsulta, NULL, Servidor_Hilo_Consulta, &datosRecibidos);
																								pthread_join(hiloConsulta,NULL);

																								break;
																case 3:
																								datosRecibidos.lista=lista;
																								datosRecibidos.Operacion=Operacion;
																								datosRecibidos.newsockfd=newsockfd;
																								pthread_t hiloActualizar;
																								pthread_create(&hiloActualizar, NULL, Servidor_Hilo_Actualizar, &datosRecibidos);
																								pthread_join(hiloActualizar,&listaaux);
																								lista=(Data *)listaaux;
																								break;
																case 4:
																								datosRecibidos.lista=lista;
																								datosRecibidos.Operacion=Operacion;
																								datosRecibidos.newsockfd=newsockfd;
																								pthread_t hiloDeposito;
																								pthread_create(&hiloDeposito, NULL, Servidor_Hilo_Deposito, &datosRecibidos);
																								pthread_join(hiloDeposito,&listaaux);
																								lista=(Data *)listaaux;
																								break;
																case 5:
																								datosRecibidos.lista=lista;
																								datosRecibidos.Operacion=Operacion;
																								datosRecibidos.newsockfd=newsockfd;
																								pthread_t hiloRetiro;
																								pthread_create(&hiloRetiro, NULL, Servidor_Hilo_Retiro, &datosRecibidos);
																								pthread_join(hiloRetiro,&listaaux);
																								lista=(Data *)listaaux;
																								break;

																case 6:
																								datosRecibidos.lista=lista;
																								datosRecibidos.Operacion=Operacion;
																								datosRecibidos.newsockfd=newsockfd;
																								pthread_t hiloTransferencia;
																								pthread_create(&hiloTransferencia, NULL, Servidor_Hilo_Transferencia, &datosRecibidos);
																								pthread_join(hiloTransferencia,&listaaux);
																								lista=(Data *)listaaux;
																								break;
																case 7:
																								datosRecibidos.lista=lista;
																								datosRecibidos.Operacion=Operacion;
																								datosRecibidos.newsockfd=newsockfd;
																								pthread_t hiloLotes;
																								pthread_create(&hiloLotes, NULL, Servidor_Hilo_Lotes, &datosRecibidos);
																								pthread_join(hiloLotes,&listaaux);
																								lista=(Data *)listaaux;
																								break;

																case 8:
																								close(newsockfd);
																								break;
																default:
																								close(newsockfd);
																								break;
																}
								}
								close(sockfd);
								return 0;
}
