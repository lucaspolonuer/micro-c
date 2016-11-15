#include <stdio.h>
#include <string.h>

#define NUMESTADOS 15
#define NUMCOLS 13
#define TAMLEX 32+1

/*	---------- Compilador de lenguaje Micro	----------

	Realizado por GRUPO 1, Integrantes :
	- Polonuer Lucas
	- Dell 'Arciprete Lucas
	- Avila Lucas
	- Baruzzini Brenda
	Para la materia Sintaxis y Semántica de los Lenguajes
	a cargo del Profesor -Ricardo Guillermo Barca-
	en el curso K2002 de UTN FRBA durante el año 2016.

	---------- Compilador de lenguaje Micro	----------

	Basado en el libro de la cátedra, escrito 
	por Jorge Daniel Muchnik.

	---------- Compilador de lenguaje Micro	----------
	
	Como compilar : gcc -ansi compilador.c -o compilador
		"compilador" puede variar a gusto.
		
	Modo de uso : Luego de compilarlo, llamar al archivo
	"compilador" o cual sea su nombre y pasarle como 
	primer y único parámetro el nombre del archivo que
	contiene el programa escrito en lenguaje micro.

	---------- Compilador de lenguaje Micro	----------
*/

FILE * in; /* Archivo con el programa en micro */
FILE * out; /* Archivo de salida en codigo de maquina virtual */

typedef enum {
	INICIO, FIN, LEER, ESCRIBIR, ID, CONSTANTE, PARENIZQUIERDO, PARENDERECHO, PUNTOYCOMA, COMA, ASIGNACION, SUMA, RESTA, FDT, ERRORLEXICO
} TOKEN;

typedef struct {
	char identificador[TAMLEX];
	TOKEN token; 
} RegTS;
/* Los registros 0,1,2,3 de la TS son palabras reservadas, a partir de 4 estan los identificadores */
RegTS TS[1000] = { {"inicio", INICIO}, {"fin", FIN}, {"leer", LEER}, {"escribir", ESCRIBIR} };

typedef struct{
	TOKEN clase;
	char nombre[TAMLEX];
	int valor;
	} REG_EXPRESION;

char buffer[TAMLEX]; /* en donde se guarda lexema por lexema que vamos leyendo */
int tamanioTS = 4; /* variable global que guarda el tamaño de la tabla de simbolos actual */
int flagToken = 0; /* variable global que indica si proximoToken debe devolver realmente el proximo o devolver el actual porque no se termino de procesar */
TOKEN tokenActual;

/* PROTOTIPOS DE FUNCIONES */

TOKEN Scanner(); /* el scanner */
int columna(char c);
int estadoFinal(int e);

void Objetivo(void); /* los PAS */
void Programa(void);
void ListaSentencias(void);
void Sentencia(void);
void ListaIdentificadores(void);
void Identificador(REG_EXPRESION * presul);
void ListaExpresiones(void);
void Expresion(REG_EXPRESION * presul);
void Primaria(REG_EXPRESION * presul);
void OperadorAditivo(char * op);

REG_EXPRESION ProcesarId(void); /*Rutinas semanticas */
REG_EXPRESION ProcesarCte(void);
char * ProcesarOp(void);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der);
void Leer(REG_EXPRESION in);
void Escribir(REG_EXPRESION out);
void Terminar(void);
REG_EXPRESION GenInfijo(REG_EXPRESION e1, char * op, REG_EXPRESION e2);

void Match(TOKEN t); /* func auxiliares */
TOKEN ProximoToken();
void ErrorLexico();
void ErrorSintactico();
void Generar(char * co, char * a, char * b, char * c);

void Colocar(char * id); /* Manejo de TS */
int Chequear(char * s);
void BuscarTipoDeId(char * id, TOKEN * t);

int main(int argc, char * argv[]){
	if ( argc == 1 ) {
		printf("Debe ingresar el nombre del archivo fuente (en lenguaje Micro) en la linea de comandos\n"); 
		return -1; 
	}
	if ( argc != 3 ) {
		printf("Numero incorrecto de argumentos\n"); 
		return -2; 
	}
	if ( argv[1][strlen(argv[1])-1] != 'm' || argv[1][strlen(argv[1])-2] != '.' ) {
		printf("Nombre incorrecto del Archivo Fuente\n"); 
		return -3; 
	}

	if ( (in = fopen(argv[1], "r")) == NULL ) {
		printf("No se pudo abrir archivo fuente\n"); 
		return -4;
	}
	if ( (out = fopen(argv[2], "w")) == NULL ) {
		fclose(in);
		printf("No se pudo abrir archivo de salida\n"); 
		return -5;
	}

	Objetivo();
	fclose(in);
	fclose(out);
	return 0;
}


/* PAS - Procedimientos de Analisis Sintactico*/

void Objetivo(void){
	/* <objetivo> -> <programa> FDT #terminar*/
		Programa();
		Match(FDT);
	Terminar(); /* sentencia de finalizacion del programa */
}

void Programa(void){
	/* <programa> -> INICIO <listaSentencias> FIN */
		Match(INICIO);
		ListaSentencias();
		Match(FIN);
}

void ListaSentencias(void){
	/* <listaSentencias> -> <sentencia> {<sentencia>} */
	Sentencia();
	while ( 1 ) { /*se repite hasta que retorna al no encontrar sentencia {<sentencia>} */
		switch ( ProximoToken() ) {
			case ID : case LEER : case ESCRIBIR :
			Sentencia();
		break;
		default : return; /*si no es sentencia termina la funcion*/
		}
	}
}

void Sentencia(void) {
	TOKEN tok = ProximoToken();
	REG_EXPRESION izq, der; /* Lo que va a la izquierda y a la derecha de una asignacion */

	switch ( tok ) {
		case ID : /* <sentencia> -> ID := <expresion> #asignar ; */
			Identificador(&izq);
			Match(ASIGNACION);
			Expresion(&der);
			Asignar(izq, der); /* genera instrucción de asignacion */
			Match(PUNTOYCOMA);
	break;
		case LEER : /* <sentencia> -> LEER ( <listaIdentificadores> ) */
			Match(LEER);
			Match(PARENIZQUIERDO);
			ListaIdentificadores();
			Match(PARENDERECHO);
			Match(PUNTOYCOMA);
	break;
		case ESCRIBIR : /* <sentencia> -> ESCRIBIR ( <listaExpresiones> ) */
			Match(ESCRIBIR);
			Match(PARENIZQUIERDO);
			ListaExpresiones();
			Match(PARENDERECHO);
			Match(PUNTOYCOMA);
	break;
	default : return;
	}
}

void ListaIdentificadores(void) {
	/* <listaIdentificadores> -> <identificador> #leer_id {COMA <identificador> #leer_id} */
	TOKEN t;
	REG_EXPRESION reg;
	Identificador(&reg);
	Leer(reg);
	for ( t = ProximoToken(); t == COMA; t = ProximoToken() ) {
		Match(COMA);
		Identificador(&reg);
		Leer(reg);
	}
}

void Identificador(REG_EXPRESION * presul) {
	/* <identificador> -> ID #procesar_id */
		Match(ID);

	/* Guardamos el id en la TS (Colocar verifica que no este) y devolvemos en el parametro pasado por referencia el id leido */	
	*presul = ProcesarId();
}

void ListaExpresiones(void) {
	/* <listaExpresiones> -> <expresion> #escribir_exp {COMA <expresion> #escribir_exp} */
	TOKEN t;
	REG_EXPRESION expresion; /* En expresion queda una constante o una variable o una variable temporal Temp */
	Expresion(&expresion); 
	Escribir(expresion);
	for ( t = ProximoToken(); t == COMA; t = ProximoToken() ) {
		Match(COMA);
		Expresion(&expresion);
		Escribir(expresion);
	}
}

void Expresion(REG_EXPRESION * presul){
	/* <expresion> -> <primaria> { <operadorAditivo> <primaria> #gen_infijo } */
	REG_EXPRESION operandoIzq, operandoDer;
	char op[TAMLEX];
	TOKEN t;
	Primaria(&operandoIzq);
	for ( t = ProximoToken(); t == SUMA || t == RESTA; t = ProximoToken() ) {
		OperadorAditivo(op);
		Primaria(&operandoDer);
		operandoIzq = GenInfijo(operandoIzq, op, operandoDer);
	}
	*presul = operandoIzq;
	/* guardo en expresion lo que haya quedado en operandoIzq, ya sea una variable si habia un solo operando
	 o la variable temporal de codigo de maquina virtual (Temp&num) si habia mas de un operando y se llamo a GenInfijo*/
}

void Primaria(REG_EXPRESION * presul) {
	TOKEN tok = ProximoToken();
	switch ( tok ) {
			case ID : /* <primaria> -> <identificador> */
				Identificador(presul); 
		break;
			case CONSTANTE : /* <primaria> -> CONSTANTE #procesar_cte */
				Match(CONSTANTE); 

				/* Guardo en presul la constante leida */
				*presul = ProcesarCte();
		break;
			case PARENIZQUIERDO : /* <primaria> -> PARENIZQUIERDO <expresion> PARENDERECHO */
				Match(PARENIZQUIERDO); 
				Expresion(presul);
				Match(PARENDERECHO); 
		break;
		default : return;
	}
}

void OperadorAditivo(char * op) {
	/* <operadorAditivo> -> SUMA #procesar_op | RESTA #procesar_op */
	TOKEN t = ProximoToken();
	if ( t == SUMA || t == RESTA ) {
		Match(t);

		/* Guardo en s el operador leido */
			strcpy(op, ProcesarOp());
	} else
		ErrorSintactico();
}


/* MANEJO DE TS */

void BuscarTipoDeId(char * id, TOKEN * t){
	/* Guarda en t el tipo de token que es id, si lo encuentra en la TS (para ver si es palabra reservada o es identificador) */
	int i = 0;
	while ( i < tamanioTS ) {
		if ( !strcmp(id, TS[i].identificador) ) {
			*t = TS[i].token;
		}
		i++;
	}
}

int Chequear(char * id){
	/* Devuelve 1 si id esta en la TS */
	int i = 0;
	while ( i < tamanioTS ) {
		if ( !strcmp(id, TS[i].identificador) ) return 1;
		i++;
	}
	return 0;
}

void Colocar(char * id){
	/* Agrega un identificador a la TS si no esta todavia y genera la instruccion de declaracion*/
	if(Chequear(id)) {
		return;
	}

	int i = tamanioTS;
	if ( i < 999 ) {
		strcpy(TS[i].identificador, id );
		TS[i].token = ID;
		tamanioTS++;
	}
	Generar("Declara", id, "Entera", "");
}

/* RUTINAS SEMANTICAS */

REG_EXPRESION ProcesarId(void){
	Colocar(buffer);
	REG_EXPRESION reg;
	reg.clase = ID;
	strcpy(reg.nombre, buffer);
	return reg;
}

REG_EXPRESION ProcesarCte(void){
	REG_EXPRESION reg;
	reg.clase = CONSTANTE;
	strcpy(reg.nombre, buffer);
	return reg;
}

char * ProcesarOp(void){
	return buffer; 
}

void ErrorLexico() {
	printf("\n----------ERROR LEXICO----------\n");
}

void ErrorSintactico() {
	printf("\n----------ERROR SINTACTICO----------\n");
}

void Asignar(REG_EXPRESION izq, REG_EXPRESION der){
	/* Genera la instruccion para la asignacion */
	Generar("Almacena", der.nombre, izq.nombre, "");
}

void Leer(REG_EXPRESION in) {
	/* Genera la instruccion para leer */
	Generar("Lee", in.nombre, "Entera", "");
}
void Escribir(REG_EXPRESION out) {
	/* Genera la instruccion para escribir */
	Generar("Escribe", out.nombre, "Entera", "");
}

void Terminar(void) {
	/* Genera la instruccion para terminar la ejecucion del programa */
	Generar("Detiene", "", "", "");
}

REG_EXPRESION GenInfijo(REG_EXPRESION e1, char * op, REG_EXPRESION e2){
	/* Genera la instruccion para una operacion infija y construye un registro semantico con el
	resultado */
	REG_EXPRESION reg;
	static unsigned int numTemp = 1;
	char cadTemp[TAMLEX] ="Temp&";
	char cadNum[TAMLEX];
	char cadOp[TAMLEX];
	if ( op[0] == '-' ) strcpy(cadOp, "Restar");
	if ( op[0] == '+' ) strcpy(cadOp, "Sumar");
	sprintf(cadNum, "%d", numTemp);
	numTemp++;
	strcat(cadTemp, cadNum);
	Colocar(cadTemp);
	Generar(cadOp, e1.nombre, e2.nombre, cadTemp);
	strcpy(reg.nombre, cadTemp);
	return reg;
}

/* FUNCIONES AUXILIARES */

TOKEN ProximoToken() {
	if ( !flagToken ) {
		tokenActual = Scanner();
		if ( tokenActual == ERRORLEXICO ) ErrorLexico();
		flagToken = 1;
		/* determinamos si el token es realmente ID o es INICIO, FIN, LEER, ESCRIBIR 
		   segun como lo encontremos en la TS */
		if ( tokenActual == ID ) {
			BuscarTipoDeId(buffer, &tokenActual);
		}
	}
	return tokenActual;
}

void Match(TOKEN t) {
	if (t != ProximoToken()) ErrorSintactico();
	flagToken = 0;
}

void Generar(char * co, char * a, char * b, char * c) {
	/* Produce la salida de la instruccion para la MV por el archivo out */
	char str_result[200];
	if(strlen(c) > 0)
		sprintf(str_result, "%s %s%c %s%c %s\n", co, a, ',', b, ',', c);
	else if(strlen(b) > 0)
		sprintf(str_result, "%s %s%c %s\n", co, a, ',', b);
	else
		sprintf(str_result, "%s %s\n", co, a);
	fputs(str_result, out);
}

/* SCANNER */

int estadoFinal(int e){
	if ( e == 0 || e == 1 || e == 3 || e == 11 || e == 14 ) return 0;
	return 1;
}

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

TOKEN Scanner(){
	int tabla[NUMESTADOS][NUMCOLS] = {
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

	int car;
	int col;
	int estado = 0;
	int i = 0;

	do {
		car = fgetc(in);
		col = columna(car);
		estado = tabla[estado][col];
		if ( col != 11 ) { /*si es espacio no lo agrega al buffer*/
			buffer[i] = car;
			i++;
		}
	}
	while ( !estadoFinal(estado) && (estado != 14) );

	buffer[i] = '\0'; /*completé la cadena*/

	switch ( estado )
	{
		case 2 : 
			if ( col != 11 ){ /*si el caracter espureo no es blanco…*/
				ungetc(car, in); /* lo retorna al flujo */
				buffer[i-1] = '\0';
			}
			return ID;
		case 4 : 
			if ( col != 11 ) {
				ungetc(car, in);
				buffer[i-1] = '\0';
			}
			return CONSTANTE;
		case 5 : return SUMA;
		case 6 : return RESTA;
		case 7 : return PARENIZQUIERDO;
		case 8 : return PARENDERECHO;
		case 9 : return COMA;
		case 10 : return PUNTOYCOMA;
		case 12 : return ASIGNACION;
		case 13 : return FDT;
		case 14 : return ERRORLEXICO;
	}

	return 0;
}