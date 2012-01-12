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

/* "NOTES" chapter from malloc(3) manual */
char srcStr[] = "By default, Linux follows an optimistic memory allocation strategy.  This means that when malloc() returns non-NULL there is no  guarantee that the memory really is available.  In case it turns out that the system is out of memory, one or more processes will be killed by the OOM killer.  For more information, see the description of /proc/sys/vm/overcommit_memory and /proc/sys/vm/oom_adj  in  proc(5),  and the kernel source file Documentation/vm/overcommit-accounting.\nNormally,  malloc() allocates memory from the heap, and adjusts the size of the heap as required, using sbrk(2).  When allocating blocks of memory larger than MMAP_THRESHOLD bytes, the glibc malloc() implementation allocates the memory as a private anonymous mapping  using mmap(2).   MMAP_THRESHOLD  is 128 kB by default, but is adjustable using mallopt(3).  Allocations performed using mmap(2) are unaffected by the RLIMIT_DATA resource limit (see getrlimit(2)).\nThe UNIX 98 standard requires malloc(), calloc(), and realloc() to set errno to ENOMEM upon failure.  Glibc assumes that  this  is  done (and  the  glibc  versions  of these routines do this); if you use a private malloc implementation that does not set errno, then certain library routines may fail without having a reason in errno.\nCrashes in malloc(), calloc(), realloc(), or free() are almost always related to heap corruption, such as overflowing an allocated chunk or freeing the same pointer twice.\nRecent  versions  of  Linux  libc (later than 5.4.23) and glibc (2.x) include a malloc() implementation which is tunable via environment variables.  When MALLOC_CHECK_ is set, a special (less efficient) implementation is used which is designed to be tolerant against simple errors,  such as double calls of free() with the same argument, or overruns of a single byte (off-by-one bugs).  Not all such errors can be protected against, however, and memory leaks can result.  If MALLOC_CHECK_ is set to 0, any  detected  heap  corruption  is  silently ignored;  if set to 1, a diagnostic message is printed on stderr; if set to 2, abort(3) is called immediately; if set to 3, a diagnostic message is printed on stderr and the program is aborted.  Using a nonzero MALLOC_CHECK_ value can be useful because  otherwise  a  crash may happen much later, and the true cause for the problem is then very hard to track down.";

void fillDict(struct dict *dictionary) {
	putToDict(dictionary, "abort(3)");
	putToDict(dictionary, "stderr");
	putToDict(dictionary, "MALLOC_CHECK_");
	putToDict(dictionary, "free()");
	putToDict(dictionary, "ENOMEM");
	putToDict(dictionary, "errno");
	putToDict(dictionary, "realloc()");
	putToDict(dictionary, "calloc()");
	putToDict(dictionary, "getrlimit(2)");
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
	int i = 0, cycles = 100000;
	int numOfWords = 10, numOfSymbols = 10, numOfResults = 33, actualNumOfResults = 0;
	double fullTime = 0, enTime = 0, stTime = 0;
	struct timeval tv_st, tv_end;
	struct dict *plainDict, *structDict, *arrayDict;

	if (2 == argc) {
		cycles = atoi((char*) argv[1]);
	}
	else {
		printf("Parameter 'number of cycles' (positive integer) allowed.\n");
	}
	printf("Doing %d cycles of searching with each type of dictionary...\n", cycles);

	/* plain dict */
	plainDict = initDict(PLAIN_DICT, THREADS_UNSAFE, ONE_STORAGE, numOfWords, numOfSymbols, numOfResults);
	fillDict(plainDict);
	while (cycles > i) {
		gettimeofday(&tv_st, NULL);
		actualNumOfResults = searchInDict(plainDict, srcStr, NULL, 0);
		gettimeofday(&tv_end, NULL);
		stTime = (tv_st.tv_sec + (tv_st.tv_usec * 0.000001));
		enTime = (tv_end.tv_sec + (tv_end.tv_usec * 0.000001));
		fullTime += (enTime - stTime);
		i++;
	}
	printf("(%d, PLAIN): [%d] dict words found in string, time (secs) = [%f], mem used: %d bytes\n", plainDict->dictType, actualNumOfResults, fullTime, plainDict->memUsed);
	freeDict(plainDict);

	/* structures dict */
	fullTime = 0;
	i = 0;
	actualNumOfResults = 0;
	structDict = initDict(STRUCT_DICT, THREADS_UNSAFE, ONE_STORAGE, numOfWords, numOfSymbols, numOfResults);
	fillDict(structDict);
	while (cycles > i) {
		gettimeofday(&tv_st, NULL);
		actualNumOfResults = searchInDict(structDict, srcStr, NULL, 0);
		gettimeofday(&tv_end, NULL);
		stTime = (tv_st.tv_sec + (tv_st.tv_usec * 0.000001));
		enTime = (tv_end.tv_sec + (tv_end.tv_usec * 0.000001));
		fullTime += (enTime - stTime);
		i++;
	}
	printf("(%d, STRUCT): [%d] dict words found in string, time (secs) = [%f], mem used: %d bytes\n", structDict->dictType, actualNumOfResults, fullTime, structDict->memUsed);
	freeDict(structDict);

	/* arrays dict */
	fullTime = 0;
	i = 0;
	actualNumOfResults = 0;
	arrayDict = initDict(ARRAY_DICT, THREADS_UNSAFE, ONE_STORAGE, numOfWords, numOfSymbols, numOfResults);
	fillDict(arrayDict);
	while (cycles > i) {
		gettimeofday(&tv_st, NULL);
		actualNumOfResults = searchInDict(arrayDict, srcStr, NULL, 0);
		gettimeofday(&tv_end, NULL);
		stTime = (tv_st.tv_sec + (tv_st.tv_usec * 0.000001));
		enTime = (tv_end.tv_sec + (tv_end.tv_usec * 0.000001));
		fullTime += (enTime - stTime);
		i++;
	}
	printf("(%d, ARRAY): [%d] dict words found in string, time (secs) = [%f], mem used: %d bytes\n", arrayDict->dictType, actualNumOfResults, fullTime, arrayDict->memUsed);
	freeDict(arrayDict);

	return 0;
}
