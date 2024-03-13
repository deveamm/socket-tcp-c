/* TCP SERVER */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PUERTO 55556
#define SIZE 255

/*-----FUNCION--LISTA------*/
#define TBUF       2048
#define DirPadre  ".."
 
struct dirent **archivos;
struct stat atrib;
int total, k;
char buffer[1024];
uid_t UserID;

char  Dirinicial[TBUF] = ".";
/*-------------------------*/

int cs, s; /*VARIBALE DE CONEXION CONCLINTE Y ESCUCHA*/
char succes[] = {"Solicitud aceptada..."};

// CAMBIA MAYUSCULAS POR MINUSCULAS Y VICEVERSA
void switch_sizecase(void){
    char word[SIZE], c;
    int length;
    read(cs, word, SIZE); // SE RECIBE LA CADENA

    length = strlen(word);
    word[(length+1)] = '\0';

    for(int i=0; i<length; i++){
        c = word[i];

        if(word[i] > 64 && word[i] < 91){ 
            c += 32;
        
        }else if(word[i] > 96 && word[i] < 123) 
            c -= 32;
        
        word[i] = c;
    }

    write(cs, word, length); // SE ENVIA LA CADENA

    // SE LIMPIA EL BUFFER DE LA CADENA
    for(int i=0; i<length; i++) word[i] = '\0';
}

// SUMA UN ARREGLO DE N NUMEROS
void sum_arg(void){
    int num[SIZE], suma=0, length=0;

    for(int i=0; i<SIZE; i++) num[i] = 0; // CLEAN BUF

    read(cs, &length, sizeof(int));     // SE RECIBEN LOS N NUMEROS
    read(cs, &num, sizeof(int)*length); // SE RECIBEN LOS N NUMEROS

    for(int i=0; i<length; i++) suma += num[i];

    write(cs, &suma, sizeof(int));      // SE ENVIA LA SUMA
}

// RECIVE EL NOMBRE DE UN ARCHIVO Y LO ORDENA EN UN DIRECTORIO
void recv_file(void){
    char word[SIZE], ext[6], ruta[SIZE], aux[SIZE];
    char *msg = {"Archivo guardado y ordenado."};
    int i=0, length=0;
    FILE *fd;

    read(cs, word, SIZE);
    read(cs, &length, sizeof(int));

    word[length] = '\0';

    while(word[i] != '.'){ i++; }

    for(int j=i, k=0; j<length; j++, k++)
        ext[k] = word[j];

    ext[(length-i)] = '\0';

    if(strcmp(".txt", ext) == 0){
        
        strcpy(ruta, "texto/");
        strcat(ruta, word);

        fd = fopen((const char*)ruta, "w");

    }else if(strcmp(".c", ext) == 0){

        strcpy(ruta, "programas/");
        strcat(ruta, word);

        fd = fopen((const char*)ruta, "w");

    }else if(strcmp(".jpg", ext) == 0){

        strcpy(ruta, "imagenes/");
        strcat(ruta, word);

        fd = fopen((const char*)ruta, "w");

    }else if(strcmp(".png", ext) == 0){

        strcpy(ruta, "imagenes/");
        strcat(ruta, word);

        fd = fopen((const char*)ruta, "w");

    }else if(strcmp(".gif", ext) == 0){
    
        strcpy(ruta, "imagenes/");
        strcat(ruta, word);

        fd = fopen((const char*)ruta, "w");
    
    }else{

        strcpy(ruta, "otros/");
        strcat(ruta, word);

        fd = fopen((const char*)ruta, "w");
    }

    fclose(fd);
    
    length = strlen(msg);
    write(cs, msg, length);
}

// ENVIA UN LISTADO DE LOS ARCHIVOS EXISTENTES
void list_files(void){
    char *msg = {"Se listan archivos."};
    int k, length=0;;
    FILE * fd;

    fd = fopen("file_list", "w");

    if ( NULL == getcwd(Dirinicial,TBUF)){
        perror("Error");
    }

    fprintf(fd,"\nDirectorio de trabajo = %s\n",Dirinicial);
  
    total = scandir (Dirinicial, &archivos, NULL, alphasort);
    fprintf(fd,"\nNumero de archivos DIRECTORIO ACTUAL= %d\n",total-2);

    for(k=0;k<total;k++){
  	    sprintf(buffer,"%s/%s",Dirinicial,archivos[k]->d_name);  
	    stat(buffer,&atrib);
	    fprintf(fd,"\t%s \t\t%0o \t\t%ld  \n",archivos[k]->d_name, atrib.st_mode, atrib.st_size);
    }

    fclose(fd);

    length = strlen(msg);
    write(cs, msg, length);
}

struct Fecha{ // CONTIENE LOS DATOS DE FECHA Y HORA DE CONEXION DEL CLIENTE
    int gettime;    // DIA
    int getmin;     // MES
    int getmon;     // AÑO
    int getday;     // HORA
    int getyear;    // MINUTOS
    int getseg;     // SEGUNDOS
}data_conec;

// IMPRIMIR DATOS DE CONECCION DEL CLIENTE
void print_conec_client(void){
    printf("Fecha %i /%i /%i ", 
        data_conec.getday, 
            data_conec.getmon, 
                data_conec.getyear);
    printf("Hora %i:%i:%i.\n",
        data_conec.gettime,
            data_conec.getmin,
                data_conec.getseg);
}

int main(void){
    struct sockaddr_in local, peer_addr;    /*DATOS DE CONECCION SERVER/CLIENT*/                                                                                                     
    socklen_t peer_addr_size;               /*TAMAÑO DE LA DIRECCION IP DEL CLIENTE*/                           
    FILE *fd;                               /*FILE DESCRIPTOR*/
    time_t hora;                            /*VARIABLE AUXILIAR PARA OBTENER EL LOCAL TIME*/
    char str[15], opc;
    int length;
    length = strlen(succes);

    s=socket(AF_INET, SOCK_STREAM, 0);

    local.sin_family=AF_INET;
    local.sin_port=htons(PUERTO);
    local.sin_addr.s_addr=INADDR_ANY;

    bind(s, (struct sockaddr *)&local, sizeof(local));

    listen (s, 5);

    // SE CREA ARCHIVO DE HISTORIAL DE CONEXIONES
    fd=fopen("history.txt", "w");
    fprintf(fd, "HISTORIAL DE CONEXIONES AL SERVIDOR\n");
    fclose(fd);

    while (1) {
        peer_addr_size = sizeof(struct sockaddr_in);
        cs=accept(s, (struct sockaddr *)&peer_addr, &peer_addr_size);

        if(cs == -1){

            perror("errno");
            exit(1);

        }else{
            // DIRECCION IP DEL CLIENTE
            struct in_addr client_ip = peer_addr.sin_addr;
            inet_ntop(AF_INET, &client_ip, str, sizeof(char)*15);

            printf("\nConexion establecida: Client IP %s. ", str);       

            time(&hora);
            struct tm *tiempo = localtime(&hora);

            data_conec.getday = tiempo->tm_mday;    // GET DIA
            data_conec.getmon = tiempo->tm_mon;     // GET MES
            data_conec.getyear = tiempo->tm_year;   // GET AÑO
            data_conec.getyear += 1900;
            data_conec.gettime = tiempo->tm_hour;   // GET HORA
            data_conec.getmin = tiempo->tm_min;     // GET MINUTOS
            data_conec.getseg = tiempo->tm_sec;     // GET SEGUNDOS

            print_conec_client(); // SE IMPRIMEN LOS DATOS DE LA CONEXION

            fd=fopen("history.txt", "r+");
            
            char c;
            while((c = getc(fd)) != EOF){ continue;}
            if(c == EOF)
                fprintf((fd), "IP %s, %i/%i/%i, %i:%i:%i.\n", str, 
                data_conec.getday, data_conec.getmon, data_conec.getyear,  // FECHA
                data_conec.gettime, data_conec.getmin, data_conec.getseg); // HORA

            fclose(fd);
        }

        printf("\nEsperando mensaje de solicitud...\n");
        read(cs, &opc, 1);

        switch (opc){
        case '1':
            write(cs, succes, length);
            recv_file();
            opc = '\0';
            break;
        
        case '2':
            write(cs, succes, length);
            list_files();
            opc = '\0';
            break;
        
        case 'a':
            write(cs, succes, length);
            switch_sizecase();
            opc = '\0';
            break;

        case 'b':
            write(cs, succes, length);
            switch_sizecase();
            opc = '\0';
            break;
        
        case 'c':
            write(cs, succes, length);
            sum_arg();
            opc = '\0';
            break;
        
        case '4':
            write(cs, succes, length);
            opc = '\0';
            break;

        default: /*ANYTHING*/
            break;
        }
        
        close(cs);
    }

    return 0;
}