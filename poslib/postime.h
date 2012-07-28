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

#ifndef __POSLIB_POSTIME_H
#define __POSLIB_POSTIME_H

#ifndef WIN32
#include <pthread.h>
#endif
#include "types.h"

#ifdef WIN32
#include <time.h>
struct timespec
  {
    time_t tv_sec;            /* Seconds.  */
    long int tv_nsec;           /* Nanoseconds.  */
  };
#endif


/*! \file poslib/postime.h
 * \brief Posadis time functions
 *
 * The functions in this source file, as well as the postime_t class defined
 * here, provide easy access to times in a system-independent way.
 */

/*!
 * \brief Time with millisecond precision
 *
 * The postime_t class offers system-independent time handling with
 * millisecond precision. Using its member functions and operators, one can
 * manipulate times in an inituive way.
 */
class postime_t {
  public:

    /*!
     * \brief postime_t constructor
     *
     * This constructor sets the number of seconds and the number of
     * milliseconds to zero.
     */
    postime_t();

    /*!
     * \brief constructor with a number of milliseconds
     *
     * This constructor takes a number of milliseconds. If this number is
     * greater than 1000, the number is split in a number of seconds and
     * a number of milliseconds. This function is mainly useful for offsets.
     * \param msecs Number of milliseconds
     * \sa postime_t()
     */
    postime_t(int msecs);

    /*!
     * \brief time assignment
     *
     * This assignment operator copies the given time to the current time.
     * \param t The time to assign
     * \return The assigned time
     */
    postime_t& operator=(const postime_t &t);

    /*!
     * \brief number of seconds
     *
     * This is the number of seconds of the time. For absolute times, this
     * is the number of seconds since the UNIX epoch as returned by the
     * gettimeofday() function.
     */
    long sec;

    /*!
     * \brief number of milliseconds
     *
     * This is the number of milliseconds. This value can range from 0 to
     * 999. If you assign a value greater than 999 to this, results are
     * undefined.
     */
    long msec;

    /*!
     * \brief equality check
     *
     * This operator checks whether two times are equal, that is, both the
     * number of seconds and the number of milliseconds are the same
     * \param t The time to compare with
     */
    bool operator==(const postime_t &t);

    /*!
     * \brief less than or equal
     *
     * This operator checks whether this time is before or the same as the
     * argument, t.
     * \param t The time to compare with
     */
    bool operator<=(const postime_t &t);

    /*!
     * \brief less than
     *
     * This operator checks whether this time is before the argument, t.
     * \param t The time to compare with
     */
    bool operator<(const postime_t &t);

    /*!
     * \brief greater than or equal

     *
     * This operator checks whether this time is after or the same as the
     * argument, t.
     * \param t The time to compare with
     */
    bool operator>=(const postime_t &t);

    /*!

     * \brief greater than
     *
     * This operator checks whether the time is after the argument, t.
     * \param t The time to compare with
     */
    bool operator>(const postime_t &t);

    /*!
     * \brief greater than timespec
     *
     * This operator checks whether the time is after the argument, t.
     * t is a timespec, as returned by the postimespec() function, and as
     * required by pthread_cond_timedwait function.
     * \param t The time to compare with
     * \sa postimespec()
     */
    bool operator>(const timespec &t);


    /*!
     * \brief Number of milliseconds after t
     *
     * This function returns the amount of time between this time and the time
     * specified by t. If this time is after t, this value is positive, if
     * this time is before t, the return value is negative.
     * \param t The time to compare with
     * \return The number of milliseconds the time is after t
     */
    int after(const postime_t &t);

    /*!
     * \brief time substraction
     *
     * This function substracts t from the given time and returns the result.
     * \param t The time to substract
     * \return The result of the substraction
     */
    postime_t operator-(const postime_t &t);

    /*!
     * \brief time addition
     *
     * This function adds t to the given time and returns the result. This is
     * probably only useful if at least one of the given times is an offset.
     * \param t The time to add
     * \return The result of the addition
     */
    postime_t operator+(const postime_t &t);

    /*!
     * \brief time millisecond addition
     *
     * This function adds the given amount of milliseconds to the time and
     * returns the result.
     * \param t The number of milliseconds to add
     * \return The result of the addition
     */
    postime_t operator+(int t);

    /*!
     * \brief time addition
     *
     * This function adds t to the given time and returns the result. This is
     * probably only useful if at least one of the given times is an offset.
     * \param t The time to add
     * \return This
     */
    postime_t& operator+=(const postime_t &t);

    /*!
     * \brief time millisecond addition
     *
     * This function adds the given amount of milliseconds to the time and
     * returns the result.
     * \param t The number of milliseconds to add
     * \return This
     */
    postime_t& operator+=(int t);
};

/*!
 * \brief get the current time
 *
 * Retrieves the current time with millisecond precision and stores it in a
 * postime_t structure.
 * \return The current time
 */
postime_t getcurtime();

/*!
 * \brief timespec for given millisecond timeout
 *
 * Returns a timespec structure that can be used, among other things, for the
 * pthread_cond_timedwait function (which is what it was designed for).
 * \param timeout Number of milliseconds from now the timespec is set to
 * \return A timespec structure containing an absolute time.
 */
timespec postimespec(int timeout);

#endif /* __POSLIB_POSTIME_H */
