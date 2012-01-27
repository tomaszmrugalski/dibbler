/*
    Posadis - A DNS Server
    Posadis time functions
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

#include "dibbler-config.h"

#include "postime.h"

#include <sys/types.h>
#if defined(WIN32)
#include <winsock2.h>
#else
#include <sys/time.h>
#endif

#include <stdio.h>

#ifndef HAVE_GETTIMEOFDAY
#include <sys/types.h>
#include <sys/timeb.h>
//typedef int timezone;
void gettimeofday(timeval *a, int *b) {
  struct timeb _gettimeofday_tmp;
  ftime(&_gettimeofday_tmp);
  a->tv_sec = _gettimeofday_tmp.time;
  a->tv_usec = _gettimeofday_tmp.millitm * 1000;
}
#endif

postime_t::postime_t() {
  sec = 0;
  msec = 0;
}

postime_t::postime_t(int msecs) {
  sec = msecs / 1000;
  msec = msecs % 1000;
}

bool postime_t::operator==(const postime_t& tm) {
  return (sec == tm.sec && msec == tm.msec);
}

bool postime_t::operator<=(const postime_t& tm) {
  return (sec < tm.sec ||
          (sec == tm.sec && msec <= tm.msec));
}

bool postime_t::operator<(const postime_t& tm) {
  return (sec < tm.sec ||
          (sec == tm.sec && msec < tm.msec));
}

bool postime_t::operator>=(const postime_t& tm) {
  return (sec > tm.sec ||
          (sec == tm.sec && msec >= tm.msec));
}

bool postime_t::operator>(const postime_t& tm) {
  return (sec > tm.sec ||
          (sec == tm.sec && msec > tm.msec));
}

bool postime_t::operator>(const timespec& tm) {
  return (sec > tm.tv_sec ||
          (sec == tm.tv_sec && msec > (tm.tv_nsec / 1000000)));
}

int postime_t::after(const postime_t &tm) {
  return (sec - tm.sec) * 1000 + (msec - tm.msec);
}


postime_t postime_t::operator+(int msecs) {
  postime_t ret = postime_t();
  long x = msec + msecs;
  ret.msec = x % 1000;
  ret.sec = sec + x / 1000;
  return ret;
}

postime_t& postime_t::operator+=(int msecs) {
  long x = msec + msecs;
  msec = x % 1000;
  sec = sec + x / 1000;
  return *this;
}


postime_t postime_t::operator-(const postime_t &tm) {
  postime_t res = postime_t();
  
  long x = msec - tm.msec;
  res.msec = x % 1000;
  res.sec += sec - tm.sec + x / 1000;

  return res;
}

postime_t postime_t::operator+(const postime_t &tm) {
  postime_t res = postime_t();
  long x;
  
  x = msec + tm.msec;
  res.msec = x % 1000;
  res.sec = sec + tm.sec + x / 1000;
  
  return res;
}

postime_t &postime_t::operator+=(const postime_t &tm) {
  long x;

  x = msec + tm.msec;
  msec = x % 1000;
  sec = sec + tm.sec + x / 1000;

  return *this;
}

postime_t& postime_t::operator=(const postime_t &tm) {
  sec = tm.sec;
  msec = tm.msec;
  return *this;
}

postime_t getcurtime() {
  timeval tmp;
  postime_t res;

  gettimeofday(&tmp, NULL);
  res.sec = tmp.tv_sec;
  res.msec = tmp.tv_usec / 1000;
  
  return res;
}


timespec postimespec(int timeout) {
  timespec end;
  timeval end2;
  gettimeofday(&end2, NULL);
  end.tv_sec = end2.tv_sec;
  end.tv_nsec = end2.tv_usec * 1000;
  end.tv_nsec += ((timeout % 1000) * 1000000);
  end.tv_sec += (timeout / 1000) + (end.tv_nsec / 1000000000);
  end.tv_nsec = end.tv_nsec % 1000000000;
  return end;
}
