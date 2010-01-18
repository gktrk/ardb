/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2002 Francois Gombault
 *  gombault.francois@wanadoo.fr
 *  
 *  Official project page: https://savannah.nongnu.org/projects/anarchdb/
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

#ifndef _speling_h
#define _speling_h

#include "types.h"

/*
 * Happily stolen from Apache's mod_speling.c
 */

typedef enum {
  SP_IDENTICAL = 0,
  SP_MISCAPITALIZED = 1,
  SP_TRANSPOSITION = 2,
  SP_MISSINGCHAR = 3,
  SP_EXTRACHAR = 4,
  SP_SIMPLETYPO = 5,
  SP_VERYDIFFERENT = 6
} sp_reason;

sp_reason spdist (wxString &s, wxString &t);
int check_speling (wxString &sBad, wxArrayString &oList);

#endif
