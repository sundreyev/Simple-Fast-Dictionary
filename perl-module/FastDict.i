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

    Author: Alexander Andreyev (aka San АНДРЕЕВ, http://linuxportal.ru) <sundreyev@gmail.com>
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

   Автор: Александр Андреев (aka San АНДРЕЕВ, http://linuxportal.ru) <sundreyev@gmail.com>
   (C) Copyright 2012 Александр Андреев
*/

%module FastDict

/* Following is from Swig-1.3 doc - http://www.swig.org/Doc1.3/Perl5.html#Perl5_nn33 */
%typemap(in) char ** {
	AV *tempav;
	I32 len;
	int i;
	SV  **tv;

	if (!SvROK($input))
		croak("$input is not an array.");
	if (SvTYPE(SvRV($input)) != SVt_PVAV)
		croak("$input is not an array.");
	tempav = (AV*)SvRV($input);
	len = av_len(tempav);
	$1 = (char **) malloc((len+2)*sizeof(char *));
	for (i = 0; i <= len; i++) {
		tv = av_fetch(tempav, i, 0);
		$1[i] = (char *) SvPV(*tv,PL_na);
	}
	$1[i] = 0;
};

/* This cleans up our char ** array after the function call */
%typemap(freearg) char ** {
	free($1);
}

/* Creates a new Perl array and places a NULL-terminated char ** into it */
%typemap(out) char ** {
	AV *myav;
	SV **svs;
	int i = 0,len = 0;
	/* Figure out how many elements we have */
	while ($1[len])
		len++;
	svs = (SV **) malloc(len*sizeof(SV *));
	for (i = 0; i < len ; i++) {
		svs[i] = sv_newmortal();
		sv_setpv((SV*)svs[i],$1[i]);
	};
	myav = av_make(len,svs);
	free(svs);
	$result = newRV_noinc((SV*)myav);
	sv_2mortal($result);
	argvi++;
}

%include stdint.i

/* Following is my conversion for intptr_t* arrays - for results array */
%typemap(in) intptr_t * {
	/* Currently no updates of results array is allowed from Perl code */
	$1 = NULL;
}
%typemap(freearg) intptr_t * {
	free($1);
}
%typemap(out) intptr_t * {
	AV *intArray;
	SV **scalarVal;
	int arrLen = $1[0], i = 0;
	scalarVal = (SV**) malloc((arrLen + 1) * sizeof(SV*));
	for (i = 0; i <= arrLen; i++) {
		scalarVal[i] = sv_newmortal();
		sv_setiv(scalarVal[i], $1[i]);
	}
	intArray = av_make(arrLen + 1, scalarVal);
	free(scalarVal);
	$result = newRV_noinc((SV*)intArray);
	sv_2mortal($result);
	argvi++;
}


%{
	/* Includes the header in the wrapper code */
	#include "../dictSearch.h"
%}
 
/* Parse the header file to generate wrappers */
%include "../dictSearch.h"
