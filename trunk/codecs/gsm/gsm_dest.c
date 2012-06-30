/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/* $Header: n:\\development/speak_freely/GSM/GSM_DEST.C,v 1.1.1.1 1998/10/15 00:47:39 Administrator Exp $ */

#include "gsm.h"
#include "private.h"
#include "proto.h"

void gsm_destroy P1((S), gsm S)
{
		if (S) free((char *)S);
}
