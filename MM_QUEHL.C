/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: MM_GETHL.C                                         */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/*                                                                      */
/* ABSTRACT: This file contains the MMIO_GETHEADERLENGTH                */
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




LONG  Handle_MMIO_QueryHeaderLength( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)
 {
 PPNGFILESTATUS       pVidInfo;

 LONG lRetV ;
 if (!pmmioinfo)
    return (0);

            /************************************************************
             * If header is in translated mode then return the media
             * type specific structure size.
             ************************************************************/
 if (pmmioinfo->ulTranslate & MMIO_TRANSLATEHEADER)
    { //Message("MMIOM_QUERYHEADERLENGTH (Translate)") ;
      lRetV = sizeof (MMIMAGEHEADER) ;
      return (sizeof (MMIMAGEHEADER)); }

                 /********************************************************
                 * Header is not in translated mode so return the size
                 * of the MMotion header.
                 ********************************************************/

 Message("MMIOM_QUERYHEADERLENGTH (Untranslate)") ;
 return (PNG_HEADER_SIZE);
 }

