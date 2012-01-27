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

#ifndef __POSLIB_EXCEPTION_H
#define __POSLIB_EXCEPTION_H

/*! \file poslib/exception.h
 * \brief Poslib exception mechanism
 *
 * This file hosts the PException class, the class all Poslib functions use
 * to notify the caller of an error.
 */

/*!
 * \brief Poslib exception class
 *
 * This is the class all Poslib functions will use when they want to notify the
 * caller in case of an error. It has only one member, #message, which is a
 * free-form message describing the error. It can be constructed in various
 * ways: by specifying a message, a format string with arguments, and a
 * combination of a message and an other exception.
 */
class PException {
 public:
  /*!
   * \brief default constructor
   *
   * This default constructor for the exception class sets the #message to "".
   */
  PException();
  /*!
   * \brief copy constructor
   *
   * This destructor makes a copy of another PException.
   * \param p The PException to copy
   */
  PException(const PException& p);
  
  /*!
   * \brief constructor with message
   *
   * This constructor takes the message to store in the PException. The message
   * parameter will be copied to the PException using a strdup().
   * \param _message The message for the exception
   */
  PException(const char *_message);
  /*!
   * \brief constructor with message and another exception
   *
   * This constructor takes a message, and another PException. The #message
   * variable will consists of the _message parameter and the message of the
   * p parameter. This can be really convenient if you want to re-throw an
   * exception, for example:
   * \code
   * try {
   *   domainname x = "www.acdam.net";
   *   (...)
   * } catch (PException p) {
   *   throw PException("Process failed: ", p);
   * }
   * \endcode
   * \param _message Error message
   * \param p Exception whose message is appended to the error message
   */
  PException(const char *_message, PException &p);
  /*!
   * \brief constructor with format string
   *
   * This constructor can be used for formatting an error message. The first
   * boolean parameter is only there so that this can be distinguished from
   * other constructors; it has no actual meaning currently (we are thinking
   * what meaning we could give it though - so stay tuned :) ).
   *
   * This code internally calls vsnprintf, so you don't need to bother about
   * buffer overflows. Do note that the message will be trimmed of at 1024
   * characters if it becomes too long.
   * \param formatted Ignored.
   * \param message The format string message
   * \param ... Arguments for the format string
   */
  PException(bool formatted, const char *_message, ...);
  /*!
   * \brief destructor
   *
   * This destructor frees all memory associated with the exception object.
   */
  ~PException();
  /*!
   * \brief assignment operator
   *
   * This function assigns the message from the PException p to this object.
   * \param p The PException to copy
   * \return This exception object
   */
  PException& operator=(const PException& p);
  /*!
   * \brief the exception error message
   *
   * The error message of the exception. This is just one piece of free-form
   * text. You can always assume this value is non-NULL.
   */
  char *message;
};

#endif /* __POSLIB_EXCEPTION_H */
