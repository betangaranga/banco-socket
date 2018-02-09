#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
        int ID;
        int type;         //1.Consulta, 2.Deposito. 3.Retiro 4.Tranferencia
        char ctaOrigen[11];
        char ctaDestino[11];
        double Cantidad;
}Lote;
typedef struct {
        long fileSize;
        long dataSize;
        short offSetData;
        short headerSize;
        short regSize;
        char type[4];         //Puede ser Data o Lote
}Header;
int main(int argc, char const *argv[]) {
        Lote lotes;
        Header header;
        int cuentas;
        FILE *f;
        int opcion;
        int i;
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char str1[]="consulta";
        char str2[]="deposito";
        char str3[]="retiro";
        char str4[]="transferencia";
        char num[10];
        char min[10];
        char puntos[1]=":";

        long fileSize;
        /* code */
        printf("\t\t\t\tGENERADOR DE LOTES\t\t\t\t\t \n");
        printf("\n1 : Consulta\n");
        printf("\n2 : Deposito\n");
        printf("\n3 : Retiro\n");
        printf("\n4 : Transferencia\n");
        scanf("%d",&opcion);
        fflush(stdin);
        switch (opcion) {
        case 1:
                printf("Cuantas cuentas deseas consultar?\n");
                scanf("%d",&cuentas);
                sprintf(num, "%d", cuentas);
                sprintf(min,"%d",tm.tm_min);
                strcat(puntos,min);
                strcat(str1,num);
                strcat(str1,puntos);
                f=fopen(str1,"wb");


                for(i=1; i<cuentas+1; i++) {
                        if(i==1) {
                                header.fileSize=(cuentas*sizeof(lotes))+sizeof(header);
                                long dataSize=cuentas*sizeof(lotes);
                                short offSetData=0;
                                short headerSize=sizeof(headerSize);
                                short regSize=sizeof(lotes);
                                char type[4]="LOTE";

                                fwrite(&header,sizeof(header),1,f);
                        }
                        lotes.ID=i;

                        printf("Ingrese Cuenta %d\n",i);
                        scanf("%s",&lotes.ctaOrigen);
                        lotes.type=1;
                        lotes.Cantidad=0;
                        strcpy(lotes.ctaDestino,"NULL");
                        fwrite(&lotes,sizeof(lotes),1,f);
                }
                fseek(f,0L,SEEK_END);
                fileSize=ftell(f);
                printf("TAMANO DE CADA STRCUT %zu\n",sizeof(lotes) );
                printf("TAMANO DEL FILE %ld\n", fileSize);
                break;

        case 2:
                printf("A cuantas cuentas deseas depositar?\n");
                scanf("%d",&cuentas);
                sprintf(num, "%d", cuentas);
                sprintf(min,"%d",tm.tm_min);
                strcat(puntos,min);
                strcat(str2,num);
                strcat(str2,puntos);
                f=fopen(str2,"ab");


                for(i=1; i<cuentas+1; i++) {
                        if(i==1) {
                                header.fileSize=(cuentas*sizeof(lotes))+sizeof(header);
                                long dataSize=cuentas*sizeof(lotes);
                                short offSetData=0;
                                short headerSize=sizeof(headerSize);
                                short regSize=sizeof(lotes);
                                char type[4]="LOTE";

                                fwrite(&header,sizeof(header),1,f);
                        }
                        lotes.ID=i;

                        printf("Ingrese Cuenta %d\n",i);
                        scanf("%s",&lotes.ctaOrigen);
                        printf("Ingrese cantidad a depositar para %s\n",lotes.ctaOrigen );
                        scanf("%lf",&lotes.Cantidad);
                        lotes.type=2;
                        strcpy(lotes.ctaDestino,"NULL");
                        fwrite(&lotes,sizeof(lotes),1,f);
                }
                fseek(f,0L,SEEK_END);
                fileSize=ftell(f);
                printf("TAMANO DE CADA STRUCT %zu\n",sizeof(lotes) );
                printf("TAMANO DEL FILE %ld\n", fileSize);
                break;
        case 3:
                printf("De cuantas cuentas deseas retirar dinero?\n");
                scanf("%d",&cuentas);
                sprintf(num, "%d", cuentas);
                sprintf(min,"%d",tm.tm_min);
                strcat(puntos,min);
                strcat(str3,num);
                strcat(str3,puntos);
                f=fopen(str3,"ab");


                for(i=1; i<cuentas+1; i++) {
                        if(i==1) {
                                header.fileSize=(cuentas*sizeof(lotes))+sizeof(header);
                                long dataSize=cuentas*sizeof(lotes);
                                short offSetData=0;
                                short headerSize=sizeof(headerSize);
                                short regSize=sizeof(lotes);
                                char type[4]="LOTE";

                                fwrite(&header,sizeof(header),1,f);
                        }
                        lotes.ID=i;
                        printf("Ingrese Cuenta %d\n",i);
                        scanf("%s",&lotes.ctaOrigen);
                        printf("Ingrese cantidad a depositar para %s\n",lotes.ctaOrigen );
                        scanf("%lf",&lotes.Cantidad);
                        lotes.type=3;
                        strcpy(lotes.ctaDestino,"NULL");
                        fwrite(&lotes,sizeof(lotes),1,f);
                }
                fseek(f,0L,SEEK_END);
                fileSize=ftell(f);
                printf("TAMANO DE CADA STRCUT %zu\n",sizeof(lotes) );
                printf("TAMANO DEL FILE %ld\n", fileSize);
                break;
        case 4:
                printf("Cuantas transferencias deseas realizar?\n");
                scanf("%d",&cuentas);
                sprintf(num, "%d", cuentas);
                sprintf(min,"%d",tm.tm_min);
                strcat(puntos,min);
                strcat(str4,num);
                strcat(str4,puntos);
                f=fopen(str4,"ab");


                for(i=1; i<cuentas+1; i++) {
                        if(i==1) {
                                header.fileSize=(cuentas*sizeof(lotes))+sizeof(header);
                                long dataSize=cuentas*sizeof(lotes);
                                short offSetData=0;
                                short headerSize=sizeof(headerSize);
                                short regSize=sizeof(lotes);
                                char type[4]="LOTE";

                                fwrite(&header,sizeof(header),1,f);
                        }
                        lotes.ID=i;

                        printf("Ingrese Cuenta de Origen %d\n",i);
                        scanf("%s",&lotes.ctaOrigen);
                        printf("Ingrese Cuenta de Destino %d\n",i);
                        scanf("%s",&lotes.ctaDestino);
                        printf("Ingrese cantidad de la transaccion %d\n",i);
                        scanf("%lf",&lotes.Cantidad);
                        lotes.type=4;
                        fwrite(&lotes,sizeof(lotes),1,f);
                }
                fseek(f,0L,SEEK_END);
                fileSize=ftell(f);
                printf("TAMANO DE CADA STRCUT %zu\n",sizeof(lotes) );
                printf("TAMANO DEL FILE %ld\n", fileSize);

                break;









        }

        return 0;
}
