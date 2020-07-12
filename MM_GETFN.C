/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: MM_GETFN.C                                          */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/*                                                                      */
/* ABSTRACT: This file contains the MMIO_GETFORMATNAME                  */
/*            handling for PNG files.                                   */
/*                                                                      */
/*********************** END OF SPECIFICATIONS **************************/

/************************************************************************/
/* Put all #defines here                                                */
/************************************************************************/

#define INCL_32                         /* force 32 bit compile */
#define INCL_GPIBITMAPS
#define INCL_DOSFILEMGR
#define INCL_WIN
#define INCL_GPI
#define INCL_PM

#define MEMCHECK
#define INCL_PM

#define MEMCHECK

/************************************************************************/
/* Put all #includes here                                               */
/************************************************************************/

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <os2medef.h>
#include <mmioos2.h>
#include "pngproc.h"


LONG  Handle_MMIO_GetFormatName( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)
   {
   PPNGFILESTATUS       pVidInfo;
   LONG lBytesCopied;

   //Message( "MMIOM_GETFORMATNAME" ) ;
            /************************************************************
             * Copy the M-Motion format string into buffer supplied by
             * lParam1.  Only put in the amount of my string up to the
             * allocated amount which is in lParam2.  Leave enough room
             * for the NULL termination.
             ************************************************************/
  lBytesCopied = GetFormatString( FOURCC_MMOT, (char *)lParam1, lParam2 );
            return (lBytesCopied);
    }
