/*
    Posadis - A DNS Server
    Exception class
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

#include "exception.h"

#include "sysstring.h"
#include <stdarg.h>
#include <stdio.h>

PException::PException() {
  message = strdup("");
}

PException::PException(const char *_message) {
  message = strdup(_message);
}

PException::PException(const char *_message, PException &p) {
  message = (char *)malloc(strlen(_message) + strlen(p.message) + 1);
  strcpy(message, _message);
  strcat(message, p.message);
}

PException::PException(const PException& p) {
  message = strdup(p.message);
}

PException::PException(bool formatted, const char *_message, ...) {
  va_list list;
  char buff[1024];

  va_start(list, _message);
  vsnprintf(buff, sizeof(buff), _message, list);
  va_end(list);

  message = strdup(buff);
}

PException& PException::operator=(const PException& p) {
  if (this != &p) {
    free(message);
    if (p.message) message = strdup(p.message); else message = NULL;
  }
  return *this;
}

PException::~PException() {
  if (message) free(message);
}
