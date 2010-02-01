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

#include "speling.h"

/*
 * Happily stolen and converted from Apache's mod_speling.c
 */

/*
 * spdist() is taken from Kernighan & Pike,
 *  _The_UNIX_Programming_Environment_
 * and adapted somewhat to correspond better to psychological reality.
 * (Note the changes to the return values)
 *
 * According to Pollock and Zamora, CACM April 1984 (V. 27, No. 4),
 * page 363, the correct order for this is:
 * OMISSION = TRANSPOSITION > INSERTION > SUBSTITUTION
 * thus, it was exactly backwards in the old version. -- PWP
 *
 * This routine was taken out of tcsh's spelling correction code
 * (tcsh-6.07.04) and re-converted to apache data types ("char" type
 * instead of tcsh's NLS'ed "Char"). Plus it now ignores the case
 * during comparisons, so is a "approximate strcasecmp()".
 * NOTE that is still allows only _one_ real "typo",
 * it does NOT try to correct multiple errors.
 */

sp_reason
spdist(wxString &s, wxString &t)
{
    unsigned int index;
    wxString s2 = s.MakeLower (), t2 = t.MakeLower (), s3, t3;

    if (!s2.Cmp (t2))
        return SP_MISCAPITALIZED;   /* exact match (sans case) */

    for (index = 0; index < s.Length ()
            && index < s.Length ()
            && s2.GetChar (index) == t2.GetChar (index); index++);
    s2 = s2.Mid (index);
    t2 = t2.Mid (index);

    if (s2.Length () >= 1) {
        if (t2.Length () >= 1) {
            if (s2.Length () >= 2 && t2.Length () >= 2
                    && s2.GetChar (0) == t2.GetChar (1)
                    && t2.GetChar (0) == s2.GetChar (1)
                    && s2.Mid (2).Cmp (t2.Mid (2)) == 0) {
                return SP_TRANSPOSITION;        /* transposition */
            }
            if (s2.Mid (1).Cmp (t2.Mid (1)) == 0) {
                return SP_SIMPLETYPO;   /* 1 char mismatch */
            }
        }
        if (t2.Cmp (s2.Mid (1)) == 0) {
            return SP_EXTRACHAR;        /* extra character */
        }
    }
    if (t2.Length () >= 1 && s2.Cmp (t2.Mid (1)) == 0) {
        return SP_MISSINGCHAR;  /* missing character */
    }
    return SP_VERYDIFFERENT;    /* distance too large to fix. */
}


int
check_speling (wxString &sBad, wxArrayString &oList)
{
    wxString sCandidate;

    for (unsigned int i = 0; i < oList.Count (); i++) {
        /*
         * simple typing errors are checked (like, e.g.,
         * missing/extra/transposed char)
         */
        // Let's not compare further than the bad string's length
        sCandidate = oList.Item (i).Left (sBad.Length ());
        if (spdist (sBad, sCandidate) != SP_VERYDIFFERENT) {
            return i;
        }
    }
    return -1;
}
