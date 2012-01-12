/*
This file is part of Simple Fast Dictionary.

    Simple Fast Dictionary is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Simple Fast Dictionary is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Simple Fast Dictionary.  If not, see <http://www.gnu.org/licenses/>.

    Author: Alexander Andreyev (aka San АНДРЕЕВ) <sundreyev@gmail.com>
    (C) Copyright 2012 Alexander Andreyev

(Этот файл — часть Simple Fast Dictionary.

   Simple Fast Dictionary - свободная программа: вы можете перераспространять ее и/или
   изменять ее на условиях Меньшей стандартной общественной лицензии GNU в том виде,
   в каком она была опубликована Фондом свободного программного обеспечения;
   либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
   версии.

   Simple Fast Dictionary распространяется в надежде, что она будет полезной,
   но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
   или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Меньшей стандартной
   общественной лицензии GNU.

   Вы должны были получить копию Меньшей стандартной общественной лицензии GNU
   вместе с этой программой. Если это не так, см.
   <http://www.gnu.org/licenses/>.)

   Автор: Александр Андреев (aka San АНДРЕЕВ) <sundreyev@gmail.com>
   (C) Copyright 2012 Александр Андреев
*/

#include <string.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/* only visual ASCII symbols */
#define FIRST_VISUAL_SYMBOL_CODE 33
#define LAST_VISUAL_SYMBOL_CODE 91

#define PLAIN_DICT 0
#define STRUCT_DICT 1
#define ARRAY_DICT 2

#define THREADS_UNSAFE 0
#define THREADS_SAFE 1

#define ONE_STORAGE 0
#define DUPLICATED_STORAGE 1


struct memArea {
	void *mem;
	int size;
	int inUse;
	struct memArea *next;
};

struct structSymbol {
	struct structSymbol *neighbour;
	struct structSymbol *next;
	char c;
	int idx;
};

struct arraySymbol {
	short int idx;
	struct arraySymbol **letters;
};

struct dict {
	char **dictWords;
	void **letters;
	struct memArea *symbolsMem;
	struct memArea *lastSymbolMemBlock;
	struct memArea *lettersMem;
	struct memArea *lastLettersMemBlock;
	int arrSize;
	int strSize;
	intptr_t *results;
	int dictType;
	int threadSafety;
	int oneStorage;
	int numOfWords;
	int numOfSymbols;
	int numOfResults;
	int greatestLen;
	int lastAddedWordIdx;
	int memUsed;
	int actualNumOfWordPtrs;
	int actualNumOfSymbols;
	int actualNumOfLetters;
};


struct dict *initDict(int dictType, int threadSafety, int oneStorage, int numOfWords, int numOfSymbols, int numOfResults);
void putToDict(struct dict *dictionary, char *str);
int searchInDict(struct dict *dictionary, char *str, intptr_t *results, int numOfResults);
void resizeResults(struct dict *dictionary, int newSize);
void printDict(struct dict *dictionary);
void freeDict(struct dict *dictionary);

