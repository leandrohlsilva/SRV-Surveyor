/*
 * MovementsReader.cpp
 *
 *  Created on: Aug 22, 2011
 *      Author: Leandro Lourenco
 *
 *  Protocolo do arquivo de movimentos:
 *  linhas com o caractere '#' sao comentarios, entao devem ser desconsideradas
 *  as demais linhas terao a seguinte formatacao:
 *  %d %d %d\n
 *  O primeiro numero eh o motor esquerdo, o segundo a forca
 *  do motor direito e o terceiro eh o tempo de movimento.
 *  todos os parametros sao de 0 a 255
 */
#include <iostream>
#include "MovementsReader.h"
#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

MovementsReader::MovementsReader(char *nameFile) {

	FILE *file = fopen(nameFile, "r");

	if (file != NULL)
	{
		MovementsReader::qtdMovements = 0;
		movements = (Movement*) malloc(sizeof(Movement));


		while ( !feof(file) )
		{

			char m[2];
			float dur;
			fscanf(file, "%s %f\n", &m, &dur);
			Movement mov (m[0], dur);

			movements = (Movement*) realloc(movements, (qtdMovements + 1)*sizeof(Movement));
			movements[qtdMovements++] = mov;

		}
		fclose(file);
		MovementsReader::indiceAtual = 0;
	}

	else {
		cout << "Unable to open " << nameFile;
	}

}


Movement* MovementsReader::nextMovement() {
	if (hasNextMovement())
		return &movements[indiceAtual++];
	return NULL;
}
bool MovementsReader::hasNextMovement() {
	return indiceAtual < getQtdMovements();
}

int MovementsReader::getQtdMovements() {
	return MovementsReader::qtdMovements;
}

void MovementsReader::restart() {
	indiceAtual = 0;
}
