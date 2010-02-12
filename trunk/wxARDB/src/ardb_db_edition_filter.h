/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2007 Sylvain Gogel
 *  meshee.knight@gmail.com
 *
 *  Official project page: http://code.google.com/p/ardb/
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef ARDB_DB_EDITION_FILTER_H_
#define ARDB_DB_EDITION_FILTER_H_

#include <wx/wx.h>
#include "database.h"


wxArrayString ardb_db_ef_get_inuse_editions();
wxArrayString ardb_db_ef_get_ignored_editions();
void ardb_db_ef_move(const wxArrayString& inUseEditions, const wxArrayString& ignoreEditions);
void ardb_db_ef_reset();


#endif /*ARDB_DB_EDITION_FILTER_H_*/

