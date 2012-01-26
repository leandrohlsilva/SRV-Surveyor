/*
 * MovementsReader.h
 *
 *  Created on: Aug 22, 2011
 *      Author: leandro
 */

#ifndef MOVEMENTSREADER_H_
#define MOVEMENTSREADER_H_


#include "Movement.h"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>


class MovementsReader {
private:
	Movement *movements;
	int qtdMovements;
	int indiceAtual;

public:
	MovementsReader(char *nameFile);
	Movement *nextMovement();
	bool hasNextMovement();
	int getQtdMovements();
	void restart();

};


#endif /* MOVEMENTSREADER_H_ */
