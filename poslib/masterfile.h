/*
    Posadis - A DNS Server
    Master file reading functionality
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

#ifndef __POSLIB_MASTERFILE_H
#define __POSLIB_MASTERFILE_H

#include <stdio.h>
#include "dnsmessage.h"
#include "domainfn.h"

/*! \file poslib/masterfile.h
 * \brief Master file reading routines
 *
 * In this file, you will find everything you need to start reading and
 * interpreting master files today!
 */

/*!
 * \brief error callback function
 *
 * This user callback function will be called by #read_master_file if a
 * non-terminal error has occured in the master file (e.g. one RR could
 * not be read). It can be handled by the application to display the error
 * message.
 * \sa #read_master_file
 * \param user_dat User data supplied to #read_master_file
 * \param fname File name of the master file
 * \param linenum Current line number
 * \param message The error message
 */
typedef void(*error_callback)(void *user_dat, const char *fname, int linenum, const char *message);

/*!
 * \brief literal RR callback function
 *
 * Callback function getting literal RR data back from the reader. This can
 * be used by editors to preserve the way the user spelled an RR, for example
 * by using '2h' instead of 7200. The implication of this is that there is
 * no guarantee the actual RR data has the right syntax. If however this is
 * not the case, the error callback is called. If the data contains data
 * relative to a non-znroot origin, it is converted to make it relative to the
 * zone root.
 * \sa #read_master_file
 * \param user_dat User data supplied to #read_master_file
 * \param dom The domain name
 * \param ttl TTL value
 * \param type The RR type
 * \param rrdata The RR data
 * \param origin The zone root
 */ 
typedef void(*rr_literal_callback)(void *user_dat, const char *dom, const char *ttl, const char *type, const char *rrdata, domainname origin);

/*!
 * \brief compiled RR callback function
 *
 * Callback function getting a compiled resource record.
 * \sa #read_master_file
 * \param user_dat User data supplied to #read_master_file
 * \param rr The RR
 */
typedef void(*rr_callback)(void *user_dat, DnsRR *rr);

/*!
 * \brief comment callback function
 *
 * Callback function for line comments, that is lines beginning with a ';'
 * sign. These can be used to embed configuration options in master files.
 * \sa #read_master_file
 * \param user_dat User data supplied to #read_master_file
 * \param comment The comment (with the initial ';' chomped off)
 */
typedef void(*comment_callback)(void *user_dat, const char *comment);

#define POSLIB_MF_AUTOPROBE 1 /**< ignore given znroot and guess from file name */
#define POSLIB_MF_NOSOA     2 /**< Do not require SOA record */

/*!
 * \brief read master files
 *
 * This function will read the given master file for you, calling the
 * supplied callbacks when nessecary. This function contains a fairly simple
 * lexical analyzer which supports most standard master file features, but
 * not the '$include' functionality. If the function detects a syntax error,
 * it will call the error callback function, but it will continue reading
 * the file.
 * \param file Master file to open
 * \param znroot Root domain of zone
 * \param userdat User data supplied to callbacks
 * \param err Error callback function called when an error occurs
 * \param rr_cb Callback called for each RR, passing binary data (optional)
 * \param rrl_cb Callback called for each RR, passing literal data (optional)
 * \param comm_cb Callback for comments (optional)
 * \param flags One of POSLIB_MF_AUTOPROBE, POSLIB_MF_NOSOA
 */
void read_master_file(const char *file, domainname &znroot, void *userdat,
                      error_callback err, rr_callback rr_cb, rr_literal_callback rrl_cb,
                      comment_callback comm_cb, int flags);

/*!
 * \brief guess zone name from file name
 *
 * This function will do an educated guess on the zone name for a given file.
 * For example, a file called 'db.acdam.net' will probably be of the 'acdam.net'
 * zone. Supported are the 'db.' prefix and the '.prm' postfix.
 * \param file file name
 * \return Guess for zone name
 */ 
domainname guess_zone_name(const char *file);

/*!
 * \brief try and open a file for reading
 *
 * This function tests whether the given filename is a directory, and if it
 * isn't, it will try opening it and return a C-style FILE* pointer.
 * \param file file name
 * \return FILE* pointer, or \p NULL on error
 */
FILE *try_fopen_r(const char *file);

/*!
 * \brief try and open a file
 *
 * This function tests whether the given filename is a directory, and if it
 * isn't, it will try opening it and return a C-style FILE* pointer.
 * \param file file name
 * \return FILE* pointer, or \p NULL on error
 */
FILE *try_fopen(const char *file, const char *mode);

/*!
 * \brief check for existence of file
 *
 * This function tests whether the given file exists and is a common file.
 * \param file file name
 * \return \p true if the file exists, \p false otherwise
 */
bool file_exists(const char *file);

#endif /* __POSLIB_MASTERFILE_H */
