/*
    Posadis - A DNS Server
    Randomize functions
    Copyright (C) 2002  Meilof Veeningen <meilof@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef WIN32
#include <pthread.h>
#endif
#include <time.h>
#include <stdlib.h>
#ifdef __BORLANDC__
#include <limits.h>
#endif

#include "random.h"

char randomstate[256];

class _random_system {
 public:
  _random_system() {
#ifndef _WIN32
    initstate(time(NULL), randomstate, sizeof(randomstate));
    setstate(randomstate);
    srandom(time(NULL));
#else
    srand(time(NULL));
#endif
  }
  ~_random_system() {
  }
} __random_system;

int posrandom() {
#ifdef _WIN32
  int ret = rand();
#else
  int ret = random();
#endif
  return ret;
}

int possr_prev = 0;

int possimplerandom() {
  int t;
  t = (possr_prev*3877 + 29573) % 139968;
  possr_prev = t;
  return t;
}
