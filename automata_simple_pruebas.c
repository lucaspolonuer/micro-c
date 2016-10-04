#include <stdio.h>
#include <string.h>
#define NUMESTADOS 15
#define NUMCOLS 13
#define TAMLEX 32+1


int columna(char c){
	if ( isalpha(c) ) return 0;
	if ( isdigit(c) ) return 1;
	if ( c == '+' ) return 2;
	if ( c == '-' ) return 3;
	if ( c == '(' ) return 4;
	if ( c == ')' ) return 5;
	if ( c == ',' ) return 6;
	if ( c == ';' ) return 7;
	if ( c == ':' ) return 8;
	if ( c == '=' ) return 9;
	if ( c == EOF ) return 10;
	if ( isspace(c) ) return 11;
	return 12;
}

int verifica(char * s){
	int i = 0;
	while(s[i] != '\0'){
		if(columna(s[i]) == 12) return 0;
		i++;
	}
	return 1;
}

int estadoFinal(int e){
	if ( e == 0 || e == 1 || e == 3 || e == 11) return 0;
	return 1;
}

char * automata(char * s){
	static int tabla[NUMESTADOS][NUMCOLS] = {
		{  1, 3 , 5 , 6 , 7 , 8 , 9 , 10, 11, 14, 13, 0, 14 },
		{  1, 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99 , 99, 99, 99, 99 },
		{  4,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, 4 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
		{ 14, 14, 14, 14, 14, 14, 14, 14, 14, 12, 14, 14, 14 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
		{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 } };

	int estado = 0;
	char car = *s;

	do{
		if(car == '\0') car = ' '; //si llegamos al \0 lo hacemos pasar por espacio ( para no cambiar el automata del libro )
		estado = tabla[estado][columna(car)];
		s++;
		car = *s;
	}while(!estadoFinal(estado) && estado != 99);

	static char resultado[50];

	switch(estado){
		case 2:
			strcpy(resultado, "identificador");
			break;
		case 4:
			strcpy(resultado, "constante");
			break;
		case 5:
			strcpy(resultado, "suma");
			break;
		case 6:
			strcpy(resultado, "resta");
			break;
		case 7:
			strcpy(resultado, "parentesis izq");
			break;
		case 8:
			strcpy(resultado, "parentesis der");
			break;
		case 9:
			strcpy(resultado, "coma");
			break;
		case 10:
			strcpy(resultado, "punto y coma");
			break;
		case 12:
			strcpy(resultado, "asignacion");
			break;
		case 13:
			strcpy(resultado, "fin de texto");
			break;
		case 14:
			strcpy(resultado, "error lexico");
			break;

	}

	return resultado;

}

int main(int argc, char * argv[]){
	if(argc == 1){
		printf("Debe ingresar una cadena\n");
		return -1;
	}
	if(argc > 2){
		printf("Debe ingresar solamente un argumento\n");
		return -2;
	}
	if(!verifica(argv[1])){
		printf("La cadena ingresada contiene caracteres invalidos para el alfabeto\n");
		return -3;
	}
	
	printf("El resultado es : %s\n", automata(argv[1]));

	return 0;
}