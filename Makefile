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

CC=gcc
ECHO=echo
SWIG=swig
CFLAGS=-O2
#CFLAGS=-ggdb
LDFLAGS=-L.
SOFLAGS=-shared -fPIC
LIBFILE=libDictSearch.so
DEMOFILE=dictDemo
COMPAREFILE=dictsComparison
PERLMOD_DIR=perl-module

.PHONY: perl-module

all:	demo compare perl-module
lib:	$(LIBFILE)
demo:	$(DEMOFILE)
compare: $(COMPAREFILE)

$(LIBFILE): dictSearch.c dictSearch.h
	$(CC) $(CFLAGS) $(SOFLAGS) -o $@ $<

$(DEMOFILE): dictDemo.c dictSearch.h $(LIBFILE)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lDictSearch

$(COMPAREFILE): dictsComparison.c dictSearch.h $(LIBFILE)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lDictSearch

perl-module: $(PERLMOD_DIR)/FastDict.i
	$(SWIG) -perl $(PERLMOD_DIR)/FastDict.i
	$(CC) -c `perl -MConfig -e 'print join(" ", @Config{qw(ccflags optimize cccdlflags)}, "-I$$Config{archlib}/CORE")'` -I./.. dictSearch.c
	$(CC) -c `perl -MConfig -e 'print join(" ", @Config{qw(ccflags optimize cccdlflags)}, "-I$$Config{archlib}/CORE")'` -I./.. -o $(PERLMOD_DIR)/FastDict_wrap.o $(PERLMOD_DIR)/FastDict_wrap.c
	$(CC) `perl -MConfig -e 'print $$Config{lddlflags}'` dictSearch.o $(PERLMOD_DIR)/FastDict_wrap.o -o $(PERLMOD_DIR)/FastDict.so

clean:
	rm -f $(LIBFILE) $(DEMOFILE) $(COMPAREFILE) *.o $(PERLMOD_DIR)/{FastDict.pm,FastDict.so,FastDict_wrap.*}

help:
	@$(ECHO) "This is short help about building library and programs. More information is in files Description-RU.pdf (russian) and Description-EN.pdf (english)."
	@$(ECHO) "	$$ make - builds all at once"
	@$(ECHO) "	$$ make perl-module - builds Perl module based on library"
	@$(ECHO) "	$$ make lib - builds only library"
	@$(ECHO) "	$$ make demo - builds library and demo program"
	@$(ECHO) "	$$ make compare - builds library and comparison program"
	@$(ECHO) "	$$ make clean - erases all generated files"
