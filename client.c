/* TCP CLIENTE */
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PUERTO 55556
#define SIZE 255

int s;              /*VARIABLE DE CONEXION*/
char answer[SIZE];  /*SE GUARDA RESPUESTA DEL SERVIDOR*/

// MENSAJE Y RESPUESTA A SOLICITUD
void send_req(char opc){
    write(s, &opc, 1);      /*SE ENVIA OPCION ELEGIDA*/
    read(s, answer, SIZE);  /*SE LEE RESPUESTA DEL SERVIDOR*/

    printf("\nRespuesta: %s\n", answer);

    for(int i=0; i<SIZE; i++) answer[i] = '\0'; // CLEAN BUF
}

// MUESTRA HISTORIAL DE CONEXIONES AL SERVIDOR
void att_opc_4(void){ // OPCION 4
    FILE *fd;
    char line[36];

    fd = fopen("history.txt", "r");

    printf("\n");
    while(!feof(fd)){
        fgets(line,36,fd);
        puts(line);
    }

    fclose(fd);
}

// CAPTURA UNA CADENA QUE SERA PROCESADA POR EL SERVIDOR
void att_opc_3ab(void){ // OPCION 3. a) b)
    char word[SIZE];
    int length;

    printf("\nDigite una palabra: ");
    scanf("%s", word);
    length = strlen(word);

    write(s, word, length); // ENVIA CADENA

    read(s, word, length); // RECIBE CADENA

    printf("\n La cadena ahora es: %s", word);
}

// CAPTURA UNA LISTA DE N NUMEROS PARA SER SUMADOR POR EL SERVIDOR
void att_opc_3c(void){ // OPCION 3. c)
    int num[SIZE], n=0, suma=0;
    int length;

    for(int i=0; i<SIZE; i++) num[i] = 0; // CLEAN BUF

    printf("\nCuantos numeros desea ingresar?\n>: ");
    scanf("%d", &n);

    printf("\nn = %i", n);

    printf("\nDigite la lista de numeros: ");
    for(int i=0; i<n; i++) scanf("%d", &num[i]);

    write(s, &n, sizeof(int));    // ENVIA EL VALOR DE N
    write(s, &num, sizeof(int)*n);  // ENVIAN LOS N NUMEROS

    read(s, &suma, sizeof(int));  // RECIBE LA SUMA DE LOS N NUMEROS

    printf("\nLa suma de los (n = %i) numeros es: %i", n, suma);
}

void att_opc_1(void){
    char word[SIZE];
    int length;

    printf("\nDigite el nombre del archivo: ");
    scanf("%s", word);
    length = strlen(word);

    write(s, word, length); // ENVIA CADENA
    write(s, &length, sizeof(int)); // ENVIA NUMERO DE LEMENTOS

    read(s, answer, SIZE); // RECIBE CADENA

    printf("\nRespuesta: %s\n", answer);
    
    for(int i=0; i<SIZE; i++) answer[i] = '\0'; // CLEAN BUF
}

void att_opc_2(void){
    char line[SIZE*2];
    FILE *fd;

    read(s, answer, SIZE); // RECIBE CADENA

    printf("\nRespuesta: %s\n", answer);
    
    for(int i=0; i<SIZE; i++) answer[i] = '\0'; // CLEAN BUF

    fd = fopen("file_list", "r");

    printf("\n");
    while (!feof(fd)){
        fgets(line, SIZE*2, fd);
        puts(line);
    } 

    fclose(fd);
}

int main(void){
    char buff[SIZE], word[SIZE], aux[SIZE];
    int len=0;
    struct sockaddr_in server_info;
    struct hostent *host;
    FILE *ap;

    char opc;

    s=socket(AF_INET, SOCK_STREAM, 0);
    host=gethostbyname("127.0.0.1"); /* esta dirección debe ser del servidor a conectarse o solo escribe localhost sin comillas, (localhost)*/

    bcopy(host->h_addr, &server_info.sin_addr, host->h_length);

    server_info.sin_family=AF_INET;
    server_info.sin_port=htons(PUERTO);

    connect(s, (struct sockaddr *)&server_info, sizeof(server_info));

    /*MENU PETICIONES AL SERVIDOR*/
    printf("\nMENU DE OPCIONES [SOLICITUDES AL SERVIDOR]");
    printf("\n1. ENVIAR UN ARCHIVO\n2. LISTAR ARCHIVOS ENVIADOS\n3. OPERACIONES\n");
    printf("\ta. ENVIAR UNA CADENA DE MAYUSCULAS A MINUSCULAS.\n\tb. ENVIAR UNA CADENA DE MINUSCULAS A MAYUSCULAS.");
    printf("\n\tc. SUMAR N NUMEROS\n4. HISTORIAL DE CONEXIONES\n>: "); scanf("%c", &opc);

    switch (opc){ 
    case '1': /*SOLICITUD DE LA OPCION 1.*/
        send_req(opc);
        att_opc_1();
        break;
    
    case '2': /*SOLICITUD DE LA OPCION 2.*/
        send_req(opc);
        att_opc_2();
        break;

    case '3': /*SOLICITUD DE LA OPCION 3.*/
        printf("\nDebe elegir una opcion del submenu de (3): a, b, c.\n");
        break;
    
    case 'a': /*SOLICITUD DE LA OPCION 3. A)*/
        send_req(opc);
        att_opc_3ab();
        break;
    
    case 'b': /*SOLICITUD DE LA OPCION 3. B)*/
        send_req(opc);
        att_opc_3ab();
        break;

    case 'c': /*SOLICITUD DE LA OPCION 3. C)*/
        send_req(opc);
        att_opc_3c();
        break;
    
    case '4': /*SOLICITUD DE LA OPCION 4*/
        send_req(opc);
        att_opc_4();
        break;
    
    default:
        printf("\nEsa opcion no es valida...\n");
        break;
    }

    close(s);
    return 0;
}