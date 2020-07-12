/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: MM_READ.C                                          */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/*                                                                      */
/* ABSTRACT: This file contains the MMIO_READ                           */
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


LONG  Handle_MMIO_Read( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)
   {
            /************************************************************
             * Declare Local Variables
             ************************************************************/
   PPNGFILESTATUS   pVidInfo;
   LONG            rc;
   LONG            lBytesToRead;
   char            msgbuf[255] ;

   if (!pmmioinfo)
       return (MMIO_ERROR);
   pVidInfo = (PPNGFILESTATUS)pmmioinfo->pExtraInfoStruct;

            /************************************************************
             * Is Translate Data off?
             ************************************************************/
   if (!(pmmioinfo->ulTranslate & MMIO_TRANSLATEDATA))
       {
       Message("MMIOM_READ: (No translate)") ;
                /********************************************************
                 * Since no translation, provide exact number of bytes req
                 ********************************************************/
       if (!lParam1)
           return (MMIO_ERROR);
                //dove stracazzo va a finire questa chiamata ?????
       rc = mmioRead ( pVidInfo->hmmioSS, (PVOID) lParam1, (ULONG) lParam2);
       return (rc);
       }

            /************************************************************
             * Otherwise, Translate Data is on...
             ************************************************************/

            /************************************************************
             * Ensure we do NOT write more data out than is remaining
             *    in the buffer.  The length of read was requested in
             *    image bytes, so confirm that there are that many of
             *    virtual bytes remaining.
             ************************************************************/
            //Message("MMIOM_READ: (Translate)") ;
   if ((ULONG)(pVidInfo->lImgBytePos + lParam2) > pVidInfo->ulImgTotalBytes)
          lBytesToRead = pVidInfo->ulImgTotalBytes - pVidInfo->lImgBytePos;
   else
      lBytesToRead = (ULONG)lParam2;
            /************************************************************
             * Perform this block on ALL reads.  The image data should
             * be in the RGB buffer at this point, and can be handed
             * to the application.
             * Conveniently, the virtual image position is the same
             *    as the RGB buffer position, since both are 24 bit-RGB
             ************************************************************/
   memcpy ((PVOID)lParam1, &(pVidInfo->lpRGBBuf[pVidInfo->lImgBytePos]), lBytesToRead);
            /************************************************************
             * Move RGB Buffer pointer forward by number of bytes read.
             * The Img buffer pos is identical, since both are 24 bits
             ************************************************************/
   pVidInfo->lImgBytePos += lBytesToRead;
   return (lBytesToRead);
   }
