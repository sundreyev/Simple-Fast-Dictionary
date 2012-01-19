#!/usr/bin/perl

#This file is part of Simple Fast Dictionary.
#
#    Simple Fast Dictionary is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Simple Fast Dictionary is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with Simple Fast Dictionary.  If not, see <http://www.gnu.org/licenses/>.
#
#    Author: Alexander Andreyev (aka San АНДРЕЕВ, http://linuxportal.ru) <sundreyev@gmail.com>
#    (C) Copyright 2012 Alexander Andreyev
#
#(Этот файл — часть Simple Fast Dictionary.
#
#   Simple Fast Dictionary - свободная программа: вы можете перераспространять ее и/или
#   изменять ее на условиях Меньшей стандартной общественной лицензии GNU в том виде,
#   в каком она была опубликована Фондом свободного программного обеспечения;
#   либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
#   версии.
#
#   Simple Fast Dictionary распространяется в надежде, что она будет полезной,
#   но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
#   или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Меньшей стандартной
#   общественной лицензии GNU.
#
#   Вы должны были получить копию Меньшей стандартной общественной лицензии GNU
#   вместе с этой программой. Если это не так, см.
#   <http://www.gnu.org/licenses/>.)
#
#   Автор: Александр Андреев (aka San АНДРЕЕВ, http://linuxportal.ru) <sundreyev@gmail.com>
#   (C) Copyright 2012 Александр Андреев

use Benchmark;
use FastDict;

local $|=1;

####################################################################################################
# C library example and benchmark
####################################################################################################

print("\n= C ======================================\n");
my ($actualNumOfResults, $cycles, $dictWords, $i, $inputStr, $memUsed, $myDict, $numOfResults, $results);

$actualNumOfResults = 0;
$cycles = 10000;
$i = 0;
$inputStr = "By default, Linux follows an optimistic memory allocation strategy.  This means that when malloc() returns non-NULL there is no  guarantee that the memory really is available.  In case it turns out that the system is out of memory, one or more processes will be killed by the OOM killer.  For more information, see the description of /proc/sys/vm/overcommit_memory and /proc/sys/vm/oom_adj  in  proc(5),  and the kernel source file Documentation/vm/overcommit-accounting.\nNormally,  malloc() allocates memory from the heap, and adjusts the size of the heap as required, using sbrk(2).  When allocating blocks of memory larger than MMAP_THRESHOLD bytes, the glibc malloc() implementation allocates the memory as a private anonymous mapping  using mmap(2).   MMAP_THRESHOLD  is 128 kB by default, but is adjustable using mallopt(3).  Allocations performed using mmap(2) are unaffected by the RLIMIT_DATA resource limit (see getrlimit(2)).\nThe UNIX 98 standard requires malloc(), calloc(), and realloc() to set errno to ENOMEM upon failure.  Glibc assumes that  this  is  done (and  the  glibc  versions  of these routines do this); if you use a private malloc implementation that does not set errno, then certain library routines may fail without having a reason in errno.\nCrashes in malloc(), calloc(), realloc(), or free() are almost always related to heap corruption, such as overflowing an allocated chunk or freeing the same pointer twice.\nRecent  versions  of  Linux  libc (later than 5.4.23) and glibc (2.x) include a malloc() implementation which is tunable via environment variables.  When MALLOC_CHECK_ is set, a special (less efficient) implementation is used which is designed to be tolerant against simple errors,  such as double calls of free() with the same argument, or overruns of a single byte (off-by-one bugs).  Not all such errors can be protected against, however, and memory leaks can result.  If MALLOC_CHECK_ is set to 0, any  detected  heap  corruption  is  silently ignored;  if set to 1, a diagnostic message is printed on stderr; if set to 2, abort(3) is called immediately; if set to 3, a diagnostic message is printed on stderr and the program is aborted.  Using a nonzero MALLOC_CHECK_ value can be useful because  otherwise  a  crash may happen much later, and the true cause for the problem is then very hard to track down.";
$memUsed = 0;
$numOfResults = 32;

$myDict = FastDict::initDict(
	$FastDict::STRUCT_DICT,
	$FastDict::THREADS_UNSAFE,
	$FastDict::DUPLICATED_STORAGE,
	10,
	10,
	$numOfResults
);

FastDict::putToDict($myDict, "getrlimit(2)");
FastDict::putToDict($myDict, "abort(3)");
FastDict::putToDict($myDict, "stderr");
FastDict::putToDict($myDict, "MALLOC_CHECK_");
FastDict::putToDict($myDict, "free()");
FastDict::putToDict($myDict, "ENOMEM");
FastDict::putToDict($myDict, "errno");
FastDict::putToDict($myDict, "realloc()");
FastDict::putToDict($myDict, "calloc()");
FastDict::putToDict($myDict, "getrlimit(2)");
FastDict::putToDict($myDict, "RLIMIT_DATA");
FastDict::putToDict($myDict, "mmap(2)");
FastDict::putToDict($myDict, "MMAP_THRESHOLD");
FastDict::putToDict($myDict, "sbrk(2)");
FastDict::putToDict($myDict, "Documentation/vm/overcommit-accounting");
FastDict::putToDict($myDict, "proc(5)");
FastDict::putToDict($myDict, "/proc/sys/vm/oom_adj");
FastDict::putToDict($myDict, "/proc/sys/vm/overcommit_memory");
FastDict::putToDict($myDict, "mallopt(3)");
FastDict::putToDict($myDict, "malloc()");

print("Searching... ");
$cTimeSt = Benchmark->new;
while ($cycles > $i) {
	$actualNumOfResults = FastDict::searchInDict($myDict, $inputStr, 0);
	$i++;
}
$cTimeEn = Benchmark->new;
$cTime = timediff($cTimeEn, $cTimeSt);
print("done.\n");

$i = 0;
$dictWords = swig_dictWords_get $myDict;
$results = swig_results_get $myDict;
print("dictType: STRUCT\n[$actualNumOfResults] dict words found in string, mem used: ".(swig_memUsed_get $myDict)." bytes, act. symbols: ".(swig_actualNumOfSymbols_get $myDict)."\n");
print("max [".@$results[0]."] entries were requested (in order of appearance):\n");
print("----------------------------------------------------------------------------------\n");
while ((@$results[0] > $i) && (0 <= @$results[$i+1])) {
	print("\t(appeared as $i) \t".@$dictWords[@$results[$i+1]]." [index=".@$results[$i+1]."]\n");
	print("----------------------------------------------------------------------------------\n");
	$i++;
}

FastDict::freeDict($myDict);


####################################################################################################
# "true" Perl example and Benchmark
####################################################################################################

print("\n= PERL ======================================\n");
my (%dictionary, $index, @inputWords, $key, @results, $value);
%dictionary = (
	"getrlimit(2)"								=> '0',
	"abort(3)"									=> '1',
	"stderr"									=> '2',
	"MALLOC_CHECK_"								=> '3',
	"free()"									=> '4',
	"ENOMEM"									=> '5',
	"errno"										=> '6',
	"realloc()"									=> '7',
	"calloc()"									=> '8',
	"RLIMIT_DATA"								=> '9',
	"mmap(2)"									=> '10',
	"MMAP_THRESHOLD"							=> '11',
	"sbrk(2)"									=> '12',
	"Documentation/vm/overcommit-accounting"	=> '13',
	"proc(5)"									=> '14',
	"/proc/sys/vm/oom_adj"						=> '15',
	"/proc/sys/vm/overcommit_memory"			=> '16',
	"mallopt(3)"								=> '17',
	"malloc()"									=> '18'
);

print("Searching... ");
$i = 0;
$perlTimeSt = Benchmark->new;
while ($cycles > $i) {
	@inputWords = split(/[\.\,\;\t\n\s]/, $inputStr);
	$#results = -1;
	while (($key, $value) = each @inputWords) {
		if ($index = $dictionary{$value}) {
			push(@results, $index);
		}
	}
	$i++;
}
$perlTimeEn = Benchmark->new;
$perlTime = timediff($perlTimeEn, $perlTimeSt);
print("done.\n");

%dictionary = reverse %dictionary;
print("\n[".@results."] dict words found in string\n");
print("max [".@$results[0]."] entries were requested (in order of appearance):\n");
print("----------------------------------------------------------------------------------\n");
$i = 0;
foreach $value (@results) {
	if (@$results[0] > $i) {
		print("\t(appeared as $i) \t".$dictionary{$value}." [index=$value]\n");
		print("----------------------------------------------------------------------------------\n");
	}
	$i++;
}

print("\n= Times ======================================\n");
print("Number of cycles: [$cycles]\n");
print("C code took:\n\t".timestr($cTime, 'noc', '5.5f')."\n");
print("Perl code took:\n\t".timestr($perlTime, 'noc', '5.5f')."\n");
