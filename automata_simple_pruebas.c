#include <stdio.h>
#define NUMESTADOS 15
#define NUMCOLS 13
#define TAMLEX 32+1

int verifica(char * cad){
	int i = 0;
	while(cad[i] != '\0'){
		if(cad[i] != 'a' && cad[i] != 'b') return 0;
		i++;
	}
	return 1;
}

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

int automata(char * s){
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

	while(car != '\0'){
		estado = tabla[estado][columna(car)];
		s++;
		car = *s;
	}

	return estado;

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
	/*
	if(!verifica(argv[1])){
		printf("La cadena tiene caracteres que no pertenecen al alfabeto\n");
		return -3;
	}
	*/
	int res = automata(argv[1]);
	
	printf("%d\n", res );

	return 0;
}