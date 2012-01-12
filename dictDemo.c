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

#include "dictSearch.h"

/* "NOTES" chapter from malloc(3) manual */
char srcStr[] = "By default, Linux follows an optimistic memory allocation strategy.  This means that when malloc() returns non-NULL there is no  guarantee that the memory really is available.  In case it turns out that the system is out of memory, one or more processes will be killed by the OOM killer.  For more information, see the description of /proc/sys/vm/overcommit_memory and /proc/sys/vm/oom_adj  in  proc(5),  and the kernel source file Documentation/vm/overcommit-accounting.\nNormally,  malloc() allocates memory from the heap, and adjusts the size of the heap as required, using sbrk(2).  When allocating blocks of memory larger than MMAP_THRESHOLD bytes, the glibc malloc() implementation allocates the memory as a private anonymous mapping  using mmap(2).   MMAP_THRESHOLD  is 128 kB by default, but is adjustable using mallopt(3).  Allocations performed using mmap(2) are unaffected by the RLIMIT_DATA resource limit (see getrlimit(2)).\nThe UNIX 98 standard requires malloc(), calloc(), and realloc() to set errno to ENOMEM upon failure.  Glibc assumes that  this  is  done (and  the  glibc  versions  of these routines do this); if you use a private malloc implementation that does not set errno, then certain library routines may fail without having a reason in errno.\nCrashes in malloc(), calloc(), realloc(), or free() are almost always related to heap corruption, such as overflowing an allocated chunk or freeing the same pointer twice.\nRecent  versions  of  Linux  libc (later than 5.4.23) and glibc (2.x) include a malloc() implementation which is tunable via environment variables.  When MALLOC_CHECK_ is set, a special (less efficient) implementation is used which is designed to be tolerant against simple errors,  such as double calls of free() with the same argument, or overruns of a single byte (off-by-one bugs).  Not all such errors can be protected against, however, and memory leaks can result.  If MALLOC_CHECK_ is set to 0, any  detected  heap  corruption  is  silently ignored;  if set to 1, a diagnostic message is printed on stderr; if set to 2, abort(3) is called immediately; if set to 3, a diagnostic message is printed on stderr and the program is aborted.  Using a nonzero MALLOC_CHECK_ value can be useful because  otherwise  a  crash may happen much later, and the true cause for the problem is then very hard to track down.";

void fillDict(struct dict *dictionary) {
	putToDict(dictionary, "getrlimit(2)");
	putToDict(dictionary, "abort(3)");
	putToDict(dictionary, "stderr");
	putToDict(dictionary, "MALLOC_CHECK_");
	putToDict(dictionary, "free()");
	putToDict(dictionary, "ENOMEM");
	putToDict(dictionary, "errno");
	putToDict(dictionary, "realloc()");
	putToDict(dictionary, "calloc()");
	putToDict(dictionary, "RLIMIT_DATA");
	putToDict(dictionary, "mmap(2)");
	putToDict(dictionary, "MMAP_THRESHOLD");
	putToDict(dictionary, "sbrk(2)");
	putToDict(dictionary, "Documentation/vm/overcommit-accounting");
	putToDict(dictionary, "proc(5)");
	putToDict(dictionary, "/proc/sys/vm/oom_adj");
	putToDict(dictionary, "/proc/sys/vm/overcommit_memory");
	putToDict(dictionary, "mallopt(3)");
	putToDict(dictionary, "malloc()");
}

int main(int argc, char *argv[]) {
	int i = 0, dictType = STRUCT_DICT, threadSafety = THREADS_UNSAFE, oneStorage = DUPLICATED_STORAGE, numOfWords = 10, numOfSymbols = 10, numOfResults = 32;
	int actualNumOfResults = 0;
	struct dict *myDict;

	if (2 == argc) {
		dictType = atoi((char*) argv[1]);
		if ((PLAIN_DICT > dictType) || (ARRAY_DICT < dictType)) {
			printf("Dictionary type should be %d, %d or %d. Exit.\n", PLAIN_DICT, STRUCT_DICT, ARRAY_DICT);
			exit(0);
		}
	}
	else {
		printf("Parameter 'dictionary type' allowed - %d (for plain), %d (for struct) or %d (for array).\n", PLAIN_DICT, STRUCT_DICT, ARRAY_DICT);
	}
	printf("dictType: ");
	switch (dictType) {
		case PLAIN_DICT: printf("PLAIN\n\n"); break;
		case STRUCT_DICT: printf("STRUCT\n\n"); break;
		case ARRAY_DICT: printf("ARRAY\n\n");
	}
	myDict = initDict(dictType, threadSafety, oneStorage, numOfWords, numOfSymbols, numOfResults);
	fillDict(myDict);
	printDict(myDict);
	actualNumOfResults = searchInDict(myDict, srcStr, NULL, 0);
	printf("\n[%d] dict words found in string, mem used: %d bytes, act. symbols: %d\n", actualNumOfResults, myDict->memUsed, myDict->actualNumOfSymbols);
	printf("max [%d] entries were requested (in order of appearance):\n", numOfResults);
	printf("----------------------------------------------------------------------------------\n");
	while ((numOfResults > i) && (0 <= myDict->results[i])) {
		printf("\t(appeared as %2d)\t%s [index=%d]\n", i, myDict->dictWords[myDict->results[i]], myDict->results[i]);
		printf("----------------------------------------------------------------------------------\n");
		i++;
	}
	freeDict(myDict);
	return 0;
}



