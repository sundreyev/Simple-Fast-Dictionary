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
*/

#include "dictSearch.h"

/* ======================================= */
/* ===== MEMORY MANAGEMENT FUNCTIONS ===== */
/* ======================================= */

char **getWordsMem(struct dict *dictionary) {
	int ptrSize = sizeof(char*);
	char **result;
	if (dictionary->actualNumOfWordPtrs > dictionary->lastAddedWordIdx) {
		return dictionary->dictWords;
	}
	else if (0 == dictionary->actualNumOfWordPtrs) {
		dictionary->actualNumOfWordPtrs = dictionary->numOfWords;
		result = (char**) calloc(1, dictionary->numOfWords * ptrSize);
	}
	else if (dictionary->actualNumOfWordPtrs == dictionary->lastAddedWordIdx) {
		dictionary->actualNumOfWordPtrs += dictionary->numOfWords;
		result = (char**) realloc((void*)dictionary->dictWords, dictionary->actualNumOfWordPtrs * ptrSize);
		memset((void*)(result + (dictionary->actualNumOfWordPtrs - dictionary->numOfWords)), '\0', dictionary->numOfWords * ptrSize);
	}
	dictionary->memUsed += dictionary->numOfWords * ptrSize;
	return result;
}

void *getSymbolMem(struct dict *dictionary) {
	int areaSize;
	void *result;
	struct memArea *mem;
	areaSize = dictionary->numOfSymbols * dictionary->strSize;
	if (NULL != dictionary->lastSymbolMemBlock) {
		mem = dictionary->lastSymbolMemBlock;
		if (mem->inUse >= mem->size) {
			mem->next = (struct memArea*) calloc(1, sizeof(struct memArea));
			mem = mem->next;
			mem->mem = calloc(1, areaSize);
			mem->size = areaSize;
			dictionary->lastSymbolMemBlock = mem;
			dictionary->memUsed += sizeof(struct memArea) + areaSize;
		}
	}
	else {
		mem = (struct memArea*) calloc(1, sizeof(struct memArea));
		mem->mem = calloc(1, areaSize);
		mem->size = areaSize;
		dictionary->symbolsMem = mem;
		dictionary->lastSymbolMemBlock = mem;
		dictionary->memUsed += sizeof(struct memArea) + areaSize;
	}
	result = mem->mem + mem->inUse;
	mem->inUse += dictionary->strSize;
	dictionary->actualNumOfSymbols++;
	return result;
}

void **getLettersMem(struct dict *dictionary) {
	void **result;
	struct memArea *mem;
	if (NULL != dictionary->lastLettersMemBlock) {
		mem = dictionary->lastLettersMemBlock;
		if (mem->inUse >= mem->size) {
			mem->next = (struct memArea*) calloc(1, sizeof(struct memArea));
			mem = mem->next;
			mem->mem = calloc(1, dictionary->arrSize);
			mem->size = dictionary->arrSize;
			dictionary->lastLettersMemBlock = mem;
			dictionary->memUsed += sizeof(struct memArea) + dictionary->arrSize;
		}
	}
	else {
		mem = (struct memArea*) calloc(1, sizeof(struct memArea));
		mem->mem = calloc(1, dictionary->arrSize);
		mem->size = dictionary->arrSize;
		dictionary->lettersMem = mem;
		dictionary->lastLettersMemBlock = mem;
		dictionary->memUsed += sizeof(struct memArea) + dictionary->arrSize;
	}
	result = mem->mem + mem->inUse;
	mem->inUse += dictionary->arrSize;
	dictionary->actualNumOfLetters++;
	return result;
}

void resizeResults(struct dict *dictionary, int newSize) {
	if (THREADS_UNSAFE == dictionary->threadSafety) {
		free(dictionary->results);
		dictionary->results = (intptr_t*) calloc(1, newSize * sizeof(intptr_t*));
		dictionary->numOfResults = newSize;
	}
}


/* =========================================== */
/* ===== DICTIONARY MANAGEMENT FUNCTIONS ===== */
/* =========================================== */

struct dict *initDict(int dictType, int threadSafety, int oneStorage, int numOfWords, int numOfSymbols, int numOfResults) {
	struct dict *dictionary = (struct dict*) calloc(1, sizeof(struct dict));
	dictionary->dictType = dictType;
	dictionary->threadSafety = threadSafety;
	dictionary->oneStorage = oneStorage;
	dictionary->greatestLen = 0;
	dictionary->lastAddedWordIdx = 0;
	dictionary->numOfWords = numOfWords;
	dictionary->numOfSymbols = numOfSymbols;
	dictionary->numOfResults = numOfResults;
	switch (dictionary->dictType) {
		case STRUCT_DICT:
			dictionary->strSize = sizeof(struct structSymbol);
			dictionary->arrSize = sizeof(struct structSymbol*) * (LAST_VISUAL_SYMBOL_CODE + 1);
			break;
		case ARRAY_DICT:
			dictionary->strSize = sizeof(struct arraySymbol);
			dictionary->arrSize = sizeof(struct arraySymbol*) * dictionary->numOfSymbols * (LAST_VISUAL_SYMBOL_CODE + 1);
			break;
		default: break;
	}
	dictionary->memUsed = sizeof(struct dict);
	if ((PLAIN_DICT == dictionary->dictType) || (DUPLICATED_STORAGE == dictionary->oneStorage)) {
		dictionary->dictWords = getWordsMem(dictionary);
		dictionary->memUsed += ((numOfWords + 1) * sizeof(char*));
	}
	if (THREADS_UNSAFE == threadSafety) {
		dictionary->results = (intptr_t*) calloc(1, numOfResults * sizeof(intptr_t*));
		dictionary->memUsed += (numOfResults * sizeof(intptr_t*));
	}
	if ((STRUCT_DICT == dictType) || (ARRAY_DICT == dictType)) {
		dictionary->letters = getLettersMem(dictionary);
	}
	return dictionary;
}

void freeDict(struct dict *dictionary) {
	int i = 0;
	struct memArea *mem, *tmpMem;

	if ((PLAIN_DICT == dictionary->dictType) || (DUPLICATED_STORAGE == dictionary->oneStorage)) {
		while ((NULL != dictionary->dictWords[i]) && (dictionary->actualNumOfWordPtrs >= i)) {
			free(dictionary->dictWords[i]);
			i++;
		}
		free(dictionary->dictWords);
	}

	if (THREADS_UNSAFE == dictionary->threadSafety) free(dictionary->results);

	if ((STRUCT_DICT == dictionary->dictType) || (ARRAY_DICT == dictionary->dictType)) {
		if (NULL != dictionary->symbolsMem) {
			mem = dictionary->symbolsMem;
			while (NULL != mem) {
				free(mem->mem);
				tmpMem = mem;
				mem = mem->next;
				free(tmpMem);
			}
		}
		if (NULL != dictionary->lettersMem) {
			mem = dictionary->lettersMem;
			while (NULL != mem) {
				free(mem->mem);
				tmpMem = mem;
				mem = mem->next;
				free(tmpMem);
			}
		}
	}

	free(dictionary);
	dictionary = NULL;
}



/* ===================================== */
/* ===== PUT WORDS TO DICTIONARIES ===== */
/* ===================================== */

/* ----- put word to index dictionary ----- */
void putToArrayDict(struct dict *dictionary, char *str) {
	int i = 0, len = strlen(str);
	struct arraySymbol *s, **letters;

	letters = (struct arraySymbol **) dictionary->letters;
	while (len > i) {
		if (NULL != letters[str[i] - FIRST_VISUAL_SYMBOL_CODE]) {
			/* some value for this letter exists, so use it */
			s = letters[str[i] - FIRST_VISUAL_SYMBOL_CODE];
		}
		else {
			/* there will be first word with this letter at the begining */
			s = (struct arraySymbol*) getSymbolMem(dictionary);
			s->idx = -1;
			letters[str[i] - FIRST_VISUAL_SYMBOL_CODE] = s;
		}
		if (NULL == s->letters) s->letters = (struct arraySymbol**) getLettersMem(dictionary);
		letters = s->letters;
		i++;
	}
	s = (struct arraySymbol *) getSymbolMem(dictionary);
	s->idx = dictionary->lastAddedWordIdx - 1;
	letters[LAST_VISUAL_SYMBOL_CODE] = s;
}


/* ----- put word to tree dictionary ----- */
void putToStructDict(struct dict *dictionary, char *str) {
	int i = 1, len = strlen(str);
	//int strSize = sizeof(struct structSymbol);
	struct structSymbol *s, *next, **letters;

	letters = (struct structSymbol **) dictionary->letters;
	if (NULL != letters[str[0] - FIRST_VISUAL_SYMBOL_CODE]) {
		/* some value for this letter exists, so use it */
		s = letters[str[0] - FIRST_VISUAL_SYMBOL_CODE];
	}
	else {
		/* there will be first word with this letter at the begining */
		s = (struct structSymbol*) getSymbolMem(dictionary);
		s->idx = -1;
		letters[str[0] - FIRST_VISUAL_SYMBOL_CODE] = s;
	}
	if (len > i) {
		while (len > i) {
			while (NULL != s) {
				if ('\0' == s->c) {
					s->c = str[i];
					s->neighbour = (struct structSymbol*) getSymbolMem(dictionary);
					if (len != (i+1)) {
						s->neighbour->idx = -1;
						s = s->neighbour;
					}
					else {
						s->neighbour->idx = dictionary->lastAddedWordIdx - 1;
					}
					break;
				}
				else if (str[i] == s->c) {
					if (len != (i+1)) {
						if (NULL == s->neighbour) {
							s->neighbour = (struct structSymbol*) getSymbolMem(dictionary);
							s->neighbour->idx = -1;
						}
						s = s->neighbour;
					}
					else {
						while (NULL != s) {
							if ('\0' == s->c) break;
							if (NULL == s->next) {
								s->next = (struct structSymbol*) getSymbolMem(dictionary);
								s->next->idx = dictionary->lastAddedWordIdx - 1;
								break;
							}
							s = s->next;
						}
					}
					break;
				}
				else {
					if (NULL == s->next) {
						s->next = (struct structSymbol*) getSymbolMem(dictionary);
						s->next->c = str[i];
						s->next->idx = -1;
						s->next->neighbour = (struct structSymbol*) getSymbolMem(dictionary);
						s = s->next->neighbour;
						if (len == (i+1)) {
							s->idx = dictionary->lastAddedWordIdx - 1;
						}
						else {
							s->idx = -1;
						}
						break;
					}
					else {
						s = s->next;
					}
				}
			}
			i++;
		}
	}
	else if (len == i) {
		if ('\0' == s->c) {
			s->idx = dictionary->lastAddedWordIdx - 1;
		}
		else {
			while (NULL != s->next) {
				s = s->next;
			}
			s->next = (struct structSymbol*) getSymbolMem(dictionary);
			s->next->idx = dictionary->lastAddedWordIdx - 1;
		}
	}
}


/* ----- common interface and plain dictionary 'put' function ----- */
void putToDict(struct dict *dictionary, char *str) {
	int len = strlen(str);
	if (len > dictionary->greatestLen) {
		dictionary->greatestLen = len;
	}

	dictionary->lastAddedWordIdx++;
	if ((PLAIN_DICT == dictionary->dictType) || (DUPLICATED_STORAGE == dictionary->oneStorage)) {
		dictionary->dictWords = getWordsMem(dictionary);
		if (NULL == dictionary->dictWords[dictionary->lastAddedWordIdx-1]) {
			dictionary->dictWords[dictionary->lastAddedWordIdx-1] = (char*) malloc(len+1);
			strncpy(dictionary->dictWords[dictionary->lastAddedWordIdx-1], str, len+1);
			dictionary->memUsed += len + 1;
		}
	}
	switch (dictionary->dictType) {
		case PLAIN_DICT: dictionary->actualNumOfSymbols += len; break;
		case STRUCT_DICT: putToStructDict(dictionary, str); break;
		case ARRAY_DICT: putToArrayDict(dictionary, str); break;
		default: break;;
	}
}



/* ======================================== */
/* ===== SEARCH WORDS IN DICTIONARIES ===== */
/* ======================================== */

/* ----- search word in index arrays dictionary ----- */
int searchInArrayDict(struct dict *dictionary, char *str, intptr_t *results, int numOfResults) {
	int i = 0, word = 0, k = 0;
	struct arraySymbol *s = NULL, **letters = (struct arraySymbol**) dictionary->letters;
	/* use internal results array for non-treads case */
	if (THREADS_UNSAFE == dictionary->threadSafety) memset((void*)dictionary->results, 0, dictionary->numOfResults);

	while ('\0' != str[i]) {
		if (('.' == str[i]) || (',' == str[i]) || (';' == str[i]) || ('\t' == str[i]) || ('\n' == str[i]) || (' ' == str[i])) {
			/* words delimiter in source string found - no word or word ended */
			if ((NULL != s) && (NULL != s->letters[LAST_VISUAL_SYMBOL_CODE])) {
				/* "termination symbol" in array, so there is a searched word */
				if (THREADS_UNSAFE == dictionary->threadSafety) {
					if (k < dictionary->numOfResults) dictionary->results[k] = s->letters[LAST_VISUAL_SYMBOL_CODE]->idx;
				}
				else {
					/* thread safe storage - each thread should have its own results array */
					if (k < numOfResults) results[k] = s->letters[LAST_VISUAL_SYMBOL_CODE]->idx;
				}
				k++;
				s = NULL;
			}
			word = 0;
		}
		else {
			if (0 == word) {
				s = letters[str[i] - FIRST_VISUAL_SYMBOL_CODE];
				word = 1;
			}
			else {
				if (NULL != s) {
					s = s->letters[str[i] - FIRST_VISUAL_SYMBOL_CODE];
				}
			}
		}
		i++;
	}

	return k;
}


/* ----- search word in plain dictionary ----- */
int searchInPlainDict(struct dict *dictionary, char *str, intptr_t *results, int numOfResults) {
	int i = 0, j = 0;
	int strLen = 0, dictLen = 0;
	char *token, *subtoken;
	char *srcStr = (char*) malloc(strlen(str) + 1);
	if (THREADS_UNSAFE == dictionary->threadSafety) memset((void*)dictionary->results, 0, dictionary->numOfResults);
	strncpy(srcStr, str, strlen(str) + 1);

	token = strtok(srcStr, " .,;\t\n");
	if (NULL != token) {
		while (dictionary->lastAddedWordIdx > j) {
			strLen = strlen(token);
			dictLen = strlen(dictionary->dictWords[j]);
			if ((0 == strncmp(token, dictionary->dictWords[j], dictLen)) && (strLen == dictLen)) {
				if (THREADS_UNSAFE == dictionary->threadSafety) {
					if (i < dictionary->numOfResults) dictionary->results[i] = j;
				}
				else {
					/* thread safe storage - each thread should have its own results array */
					if (i < numOfResults) results[i] = j;
				}
				i++;
				break;
			}
			j++;
		}
		j = 0;
		subtoken = strtok(NULL, " .,;\t\n");
		while (NULL != subtoken) {
			while (dictionary->lastAddedWordIdx > j) {
				strLen = strlen(subtoken);
				dictLen = strlen(dictionary->dictWords[j]);
				if ((0 == strncmp(subtoken, dictionary->dictWords[j], dictLen)) && (strLen == dictLen)) {
					if (THREADS_UNSAFE == dictionary->threadSafety) {
						if (i < dictionary->numOfResults) dictionary->results[i] = j;
					}
					else {
						/* thread safe storage - each thread should have its own results array */
						if (i < numOfResults) results[i] = j;
					}
					i++;
					break;
				}
				j++;
			}
			subtoken = strtok(NULL, " .,;\t\n");
			j = 0;
		}
	}

	free(srcStr);
	return i;
}


/* ----- search word in structures dictionary ----- */
int searchInStructDict(struct dict *dictionary, char *str, intptr_t *results, int numOfResults) {
	int i = 0, word = 0, k = 0;
	struct structSymbol *s = NULL, **letters = (struct structSymbol**) dictionary->letters;
	if (THREADS_UNSAFE == dictionary->threadSafety) memset((void*)dictionary->results, 0, dictionary->numOfResults);

	while ('\0' != str[i]) {
		if (('.' == str[i]) || (',' == str[i]) || (';' == str[i]) || ('\t' == str[i]) || ('\n' == str[i]) || (' ' == str[i])) {
			while (NULL != s) {
				if ('\0' == s->c) {
					if (THREADS_UNSAFE == dictionary->threadSafety) {
						if (k < dictionary->numOfResults) dictionary->results[k] = s->idx;
					}
					else {
						/* thread safe storage - each thread should have its own results array */
						if (k < numOfResults) results[k] = s->idx;
					}
					s = NULL;
					k++;
					break;
				}
				s = s->next;
			}
			// no word or word ended
			word = 0;
		}
		else {
			if (0 == word) {
				// no word and first non-space symbol found
				s = letters[str[i] - FIRST_VISUAL_SYMBOL_CODE];
				word = 1;
			}
			else {
				// word currently here
				while (NULL != s) {
					if (str[i] == s->c) {
						s = s->neighbour;
						break;
					}
					s = s->next;
				}
			}
		}
		i++;
	}

	return k;
}


/* ----- common interface 'search' function ----- */
int searchInDict(struct dict *dictionary, char *str, intptr_t *results, int numOfResults) {
	int result = 0;
	switch (dictionary->dictType) {
		case PLAIN_DICT: result = searchInPlainDict(dictionary, str, results, numOfResults); break;
		case STRUCT_DICT: result = searchInStructDict(dictionary, str, results, numOfResults); break;
		case ARRAY_DICT: result = searchInArrayDict(dictionary, str, results, numOfResults); break;
		default: break;
	}
	return result;
}



/* ======================================== */
/* ===== DUMP WORDS FROM DICTIONARIES ===== */
/* ======================================== */

/* recursive through index arrays
 * j - level of recursion
 * tmp - currently accumulated symbols which belong to one word
 */
void printArrayPlace(struct arraySymbol **letters, int j, char *tmp) {
	int i = 0, k = 0;
	struct arraySymbol *s;
	j++;
	while (LAST_VISUAL_SYMBOL_CODE >= i) {
		if (NULL != letters[i]) {
			if (i != LAST_VISUAL_SYMBOL_CODE) {
				// there are more symbols, go to the next level (== symbol position in the word) until terminating symbol occurs
				s = letters[i];
				tmp[j] = i + FIRST_VISUAL_SYMBOL_CODE;
				if (NULL != s->letters) printArrayPlace(s->letters, j, tmp);
			}
			else {
				// there is terminating symbol and word ended
				tmp[j] = '\0';
				while ('\0' != tmp[k]) {
					printf("%c", tmp[k]);
					k++;
				}
				printf(" [index=%d]\n", letters[i]->idx);
				k = 0;
			}
		}
		i++;
	}
	tmp[j] = '\0';
}
/* ----- indexed dictionary head dump function ----- */
void printArrayDict(struct dict *dictionary) {
	int i = 0, j = 0;
	struct arraySymbol *s, **letters = (struct arraySymbol**) dictionary->letters;
	char tmp[dictionary->greatestLen + 1];

	/* walking through first symbols and calling recursive function printArrayPlace() if there is a word with such first symbol */
	while (LAST_VISUAL_SYMBOL_CODE > i) {
		if (NULL != letters[i]) {
			s = letters[i];
			tmp[j] = i + FIRST_VISUAL_SYMBOL_CODE;
			if (NULL != s->letters) printArrayPlace(s->letters, j, tmp);
			printf("-------------------------------\n");
		}
		i++;
	}
}


/* ----- plain dictionary dump function ----- */
void printPlainDict(struct dict *dictionary) {
	int i = 0;
	while (i < dictionary->lastAddedWordIdx) {
		printf("%s [index=%d]\n", dictionary->dictWords[i], i);
		printf("-------------------------------\n");
		i++;
	}
}


/* recursive through struct neighbours
 * j - level of recursion
 * tmp - currently accumulated symbols which belong to one word
 */
void printStructPlace(struct structSymbol *s, int j, char *tmp) {
	int k = 0;
	j++;
	while (NULL != s) {
		if (NULL != s->neighbour) {
			tmp[j] = s->c;
			printStructPlace(s->neighbour, j, tmp);
		}
		else {
			tmp[j] = s->c;
			if ('\0' == s->c) {
				while ('\0' != tmp[k]) {
					printf("%c", tmp[k]);
					k++;
				}
				printf(" [index=%d]\n", s->idx);
				k = 0;
			}
		}
		s = s->next;
	}
	tmp[j] = '\0';
}
/* ----- struct dictionary head dump function ----- */
void printStructDict(struct dict *dictionary) {
	int i = 0, j = 0;
	struct structSymbol *s, **letters = (struct structSymbol**) dictionary->letters;
	char tmp[dictionary->greatestLen + 1];
	while (LAST_VISUAL_SYMBOL_CODE > i) {
		if (NULL != letters[i]) {
			s = letters[i];
			tmp[j] = i + FIRST_VISUAL_SYMBOL_CODE;
			if (NULL != s->neighbour) printStructPlace(s, j, tmp);
			printf("-------------------------------\n");
		}
		i++;
	}
}


/* ----- common interface 'dump dictionary' function ----- */
void printDict(struct dict *dictionary) {
	printf("-------------------------------\n");
	switch (dictionary->dictType) {
		case PLAIN_DICT: printPlainDict(dictionary); break;
		case STRUCT_DICT: printStructDict(dictionary); break;
		case ARRAY_DICT: printArrayDict(dictionary); break;
		default: break;;
	}
}

