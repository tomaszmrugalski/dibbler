/*
    Posadis - A DNS Server
    Bit manipulation
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

#ifndef __POSLIB_BITS_H
#define __POSLIB_BITS_H

/*! \file poslib/bits.h
 * \brief bitmask manipulation
 *
 * This source file provides some macros to easily handle bit masks, which
 * are basically just character arrays. These macros do not handle the creation
 * of bitmasks, so you'll need to do that yourself. To create a bitmask for
 * x bits, allocate a character array of (x + 7) / 8 bytes. And be sure to
 * memset() it to reset all bits in one! Also note the first bitmap index is
 * zero and the last one is x - 1.
 */

/*!
 * \brief set a bit in the bitmask
 *
 * Sets the bit value for the given bit in the bitmask.
 * \param a The bitmask, a character array
 * \param b Bit index in the bitmask
 */
#define bitset(a, b) ( (a)[b / 8] |= 1 << (7 - (b % 8)) )

/*!
 * \brief reset a bit in the bitmask
 *
 * Resets the bit value for the given bit in the bitmask.
 * \param a The bitmask, a character array
 * \param b Bit index in the bitmask
 */
#define bitreset(a, b) ( (a)[b / 8] &= ~(1 << (7 - (b % 8))) )

/*!
 * \brief sets the value of a bit in the bitmask
 *
 * Sets the value of a bit in the bitmask. If c is non-zero, the given bit is
 * set, otherwise, the given bit is reset. This can come in handy if you want
 * to do #bitsetval(a, b, #bitisset(c, d)).
 * \param a The bitmask, a character array
 * \param b Bit index in the bitmask
 * \param c Boolean value to set the bit to
 */
#define bitsetval(a, b, c) ( (c) ? bitset(a, b) : bitreset(a, b) )

/*!
 * \brief checks whether a bit is set
 *
 * This function checks whether a given bit in the bitmask is set. If the bit
 * is set, the function returns nonzero. Note that it _might_ return 1, but
 * that it usually doesn't so don't depend on that!
 * \param a The bitmask, a character array
 * \param b Bit index in the bitmask
 * \return Nonzero if the bit is, set, zero if not.
 */
#define bitisset(a, b) ( (a)[b / 8] & (1 << (7 - (b % 8))) )

#endif /* __POSLIB_BITS_H */
