/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: MM_GETH.C                                          */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/*                                                                      */
/* ABSTRACT: This file contains the MMIO_GETHEADER                      */
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

LONG  Handle_MMIO_GetHeader( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)
 {
 PPNGFILESTATUS       pVidInfo;

 //Message("MMIOM_GETHEADER") ;
 if (!pmmioinfo)
   return (0);
 pVidInfo = (PPNGFILESTATUS)pmmioinfo->pExtraInfoStruct;

/**************************************************
* Getheader only valid in READ or READ/WRITE mode.
* There is no header to get in WRITE mode.  We
* must also have a valid file handle to read from
**************************************************/
 if ((pmmioinfo->ulFlags & MMIO_WRITE) ||(!(pVidInfo->hmmioSS)))
        return (0);

/************************************************************
* Check for Translation mode.
************************************************************/
 if (!(pmmioinfo->ulTranslate & MMIO_TRANSLATEHEADER))
  {
/********************************************************
* Translation is off.
********************************************************/
  //Message("MMIOM_GETHEADER : (translate mode)") ;
  if (lParam2 < PNG_HEADER_SIZE)
    {
    pmmioinfo->ulErrorRet = MMIOERR_INVALID_BUFFER_LENGTH;
    Message("MMIO_GETHEADER: lParam2 < PNG_HEADER_SIZE") ;
    return (0);
    }
  if (!lParam1)
    {
    Message("MMIO_GETHEADER: !lParam1") ;
    pmmioinfo->ulErrorRet = MMIOERR_INVALID_STRUCTURE;
    return (0);
    }

    /********************************************************
    * Read in first 16 bytes to fill up MMotion header.
    ********************************************************/
  memcpy ((PVOID) lParam1, (PVOID)&pVidInfo->pngHeader, PNG_HEADER_SIZE);

                /* Indicate that the header has been set, which    */
                /* is meaningless in read mode, but allows the     */
                /* application to do writes in read/write mode     */
  pVidInfo->bSetHeader = TRUE;
  Message("MMIO_GETHEADER: ritorna PNG_HEADER_SIZE") ;
  return (PNG_HEADER_SIZE);
  }   /* end IF NOT TRANSLATED block */

            /******************
             * TRANSLATION IS ON
             ******************/
 if (lParam2 < sizeof (MMIMAGEHEADER))
   {
   pmmioinfo->ulErrorRet = MMIOERR_INVALID_BUFFER_LENGTH;
   return (0);
   }
 if (!lParam1)
   {
   pmmioinfo->ulErrorRet = MMIOERR_INVALID_STRUCTURE;
   return (0);
   }
   //Message("MMIOM_GETHEADER : (translated)") ;
   memcpy ((PVOID)lParam1, (PVOID)&pVidInfo->mmImgHdr, sizeof (MMIMAGEHEADER));
   return (sizeof (MMIMAGEHEADER));
 }
