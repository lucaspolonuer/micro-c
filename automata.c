#include <stdio.h>

int verifica(char * cad){
	int i = 0;
	while(cad[i] != '\0'){
		if(cad[i] != 'a' && cad[i] != 'b') return 0;
		i++;
	}
	return 1;
}

int columna(char c){
	if(c == 'a') return 0;
	if(c == 'b') return 1;
}

int automata(char * s){
	static int tabla[6][2] = {{1,5}, {5,2}, {3,1}, {5,4}, {5,5}, {3,5}};
	int estado = 0;
	char car = *s;
	while(car != '\0'){
		estado = tabla[estado][columna(car)];
		s++;
		car = *s;
	}
	if(estado == 2 || estado == 4) return 1;
	return 0;
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
		printf("La cadena tiene caracteres que no pertenecen al alfabeto\n");
		return -3;
	}
	if(automata(argv[1]))
		printf("La cadena es aceptada por ende pertenece al lenguaje\n");
	else
		printf("La cadena NO es aceptada por ende NO pertenece al lenguaje\n");
	return 0;
}