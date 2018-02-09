void Alta(Data *datos, char cuenta[10],char nombre[50],char email[50],FILE *f,size_t tamano){
	
}
n = read(newsockfd,&datosRecibidos,sizeof(DataRecibida));
if (n < 0)
                error("ERROR de lectura del socket");
printf("VERSION 1:\n");
printf("\nNombre: %s",datos.nombre);
printf("\nCorreo: %s\n",datos.email);
printf("\nSaldo: %lf\n",datos.saldo);
printf("\nEstatus: %c\n",datos.estatus);
printf("\nCuenta: %s\n",datos.cuenta);
strcpy(datos.nombre,datosRecibidos.nombre);
strcpy(datos.email,datosRecibidos.email);
strcpy(datos.cuenta,datosRecibidos.cuenta);
datos.saldo=datosRecibidos.saldo;
datos.estatus=datosRecibidos.estatus;
printf("VERSION 2:\n");
printf("\nNombre: %s",datos.nombre);
printf("\nCorreo: %s\n",datos.email);
printf("\nSaldo: %lf\n",datos.saldo);
printf("\nEstatus: %c\n",datos.estatus);
printf("\nCuenta: %s\n",datos.cuenta);
f =fopen("DATA.bin","ab+");
if(fileSize==0) {
                fwrite(&header, sizeof(header),1,f);
}

lista=InsertarFinalLista(lista,datos.nombre,datos.email,datos.cuenta,datos.saldo,datos.estatus,datos.id);
fseek(f,0L,SEEK_END);
fileSize=ftell(f);
int id=fileSize/sizeof(datos);
datos.id=id;
printf("Mensaje recibido:\n");
printf("\nNombre: %s",datos.nombre);
printf("\nCorreo: %s\n",datos.email);
printf("\nSaldo: %lf\n",datos.saldo);
printf("\nEstatus: %c\n",datos.estatus);
printf("\nCuenta: %s\n",datos.cuenta);
printf("\nID: %ld\n",datos.id);

n= fwrite(&datos, sizeof(datos),1,f);
if(n<0) {
                error("ERROR AL ESCRIBIR\n");
}
printf("Guardando datos en el archivo de base");
strcpy(Answ.mensaje,"Registrado satisfactoriamente");
Answ.ID=datos.id;
n = write(newsockfd,&Answ,sizeof(Respuesta));
if (n < 0)
                error("ERROR de escritura al socket");
fclose(f);
close(newsockfd);
