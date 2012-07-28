/*
    Posadis - A DNS Server
    Include file for randomize functions
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

#ifndef __POSLIB_RANDOM_H
#define __POSLIB_RANDOM_H

/*! \file poslib/random.h
 * \brief returns random numbers
 *
 * This source file hosts the posrandom() function for generating random
 * numbers.
 */

/*!
 * \brief return strong random number
 *
 * This function returns a strongly random number that should be unpredictable.
 */
int posrandom();

/*!
 * \brief return simple random number
 *
 * This function returns a simple random number, and therefore, it is much
 * faster than posrandom().
 */

int possimplerandom();

#endif /* __POSLIB_RANDOM_H */
