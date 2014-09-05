/*  RxWav
   Copyright (C) 1999  Giorgio Vicario

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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA     */

#define INCL_REXXSAA
#include <os2emx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regexp.h>
#include <math.h>
#include <float.h>
#include "RxWav.h"

/***********************************************************************
Altre funzioni di utilita'
***********************************************************************/
BOOL
string2long (PSZ string, LONG * number)
{
  ULONG accumulator;		/* converted number           */
  INT length;			/* length of number           */
  INT sign;			/* sign of number             */

  sign = 1;			/* set default sign           */
  if (*string == '-')
    {				/* negative?
				   sign = -1;                         /* change sign                */
      string++;			/* step past sign             */
    }

  length = strlen (string);	/* get length of string       */
  if (length == 0 ||		/* if null string             */
      length > MAX_DIGITS)	/* or too long                */
    return FALSE;		/* not valid                  */

  accumulator = 0;		/* start with zero            */

  while (length)
    {				/* while more digits          */
      if (!isdigit (*string))	/* not a digit?               */
	return FALSE;		/* tell caller                */
      /* add to accumulator         */
      accumulator = accumulator * 10 + (*string - '0');
      length--;			/* reduce length              */
      string++;			/* step pointer               */
    }
  *number = accumulator * sign;	/* return the value           */
  return TRUE;			/* good number                */
}


PSHORT
AllineaCh (PSHORT pCh, ULONG nByte, ULONG func)
{
  PBYTE pCh8 = NULL;
  ULONG pflag;
  ULONG psize;
  LONG p;
  APIRET rc;

  psize = (ULONG) &nByte;
  pflag = PAG_READ | PAG_WRITE | PAG_COMMIT;

  rc = DosQueryMem (pCh, &psize, &pflag);
  if (rc)
    {
      switch (rc)
	{
	case NO_ERROR:
	  break;
	case ERROR_INVALID_ADDRESS:
	  SendMsg (func, ERR_TRACCIA_ERRATA);
	  return 0;
	  break;
	default:
	  printf ("       DosQueryMem rc:%i\n", rc);
	  return 0;
	  break;
	}
    }

  p = (long) pCh *2;

  pCh = pCh + (nByte * 2) - 2;
  psize = (ULONG) &nByte;
  rc = DosQueryMem (pCh, &psize, &pflag);
  if (rc)
    {
      switch (rc)
	{
	case NO_ERROR:
	  break;
	case ERROR_INVALID_ADDRESS:
	  SendMsg (func, ERR_TRACCIA_INSUFFICIENTE);
	  return 0;
	  break;
	default:
	  printf ("       DosQueryMem rc:%i\n", rc);
	  return 0;
	  break;
	}
    }

  return (PSHORT) p;
}


int
FetchStem (RXSTEMDATA stemin, float *dati)
{
  sprintf (stemin.varname + stemin.stemlen, "%d", stemin.count);

  stemin.shvb.shvnext = NULL;
  stemin.shvb.shvvalue.strptr = stemin.ibuf;
  stemin.shvb.shvvalue.strlength = MAX;
  stemin.shvb.shvname.strptr = stemin.varname;
  stemin.shvb.shvname.strlength = strlen (stemin.varname);
  stemin.shvb.shvcode = RXSHV_FETCH;
  stemin.shvb.shvret = 0;

  if (RexxVariablePool (&stemin.shvb) == RXSHV_BADN)
    {
      SendMsg (0, ERR_REXXPOOL);
      return 0;
    }

  if (stemin.shvb.shvret)
    return stemin.shvb.shvret;

  strcpy (stemin.ibuf, stemin.shvb.shvvalue.strptr);

  *dati = atof (stemin.ibuf);
  return 0;

}
