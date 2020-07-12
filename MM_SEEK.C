/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: MM_SEEK.C                                          */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/*                                                                      */
/* ABSTRACT: This file contains the MMIO_SEEK                           */
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




LONG  Handle_MMIO_Seek( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)
 {
 PPNGFILESTATUS   pVidInfo;   /* pointer to a PNG file status structure */

 LONG            lNewFilePosition;
 LONG            lPosDesired;
 SHORT           sSeekMode;

 Message("MMIOM_SEEK") ;
 if (!pmmioinfo)
     return (MMIO_ERROR);
 pVidInfo = (PPNGFILESTATUS) pmmioinfo->pExtraInfoStruct;

 lPosDesired = lParam1;
 sSeekMode = (SHORT)lParam2;

            /************************************************************
             * Is Translate Data on?
             ************************************************************/
 if (pmmioinfo->ulTranslate & MMIO_TRANSLATEDATA)
     {
                /********************************************************
                 * Attempt to move the Image buffer pointer to the
                 *    desired location.  App sends SEEK requests in
                 *    positions relative to the image planes & bits/pel
                 * We must also convert this to RGB positions
                 ********************************************************/
     switch (sSeekMode)
            {
            case SEEK_SET:
                        {
                        lNewFilePosition = lPosDesired;
                        break;
                        }
            case SEEK_CUR:
                        {
                        lNewFilePosition = pVidInfo->lImgBytePos + lPosDesired;
                        break;
                        }
            case SEEK_END:
                        {

                        lNewFilePosition = pVidInfo->ulImgTotalBytes += lPosDesired;
                        break;
                        }

            default :
               return (MMIO_ERROR);
            }

                /********************************************************
                 * Make sure seek did not go before start of file.
                 * If so, then don't change anything, just return an error
                 ********************************************************/
     if (lNewFilePosition < 0)
         return (MMIO_ERROR);

                /********************************************************
                 * Make sure seek did not go past the end of file.
                 ********************************************************/
     if (lNewFilePosition > (LONG)pVidInfo->ulImgTotalBytes)
         lNewFilePosition = pVidInfo->ulImgTotalBytes;

     pVidInfo->lImgBytePos = lNewFilePosition;

     return (pVidInfo->lImgBytePos);
     } /* end  IF DATA TRANSLATED */

            /************************************************************
             * Translate Data is OFF...
             ************************************************************
             * if this is a seek from the beginning of the file,
             *    we must account for and pass the header
             ************************************************************/
 if (lParam2==SEEK_SET)
     lPosDesired += PNG_HEADER_SIZE ;

 lNewFilePosition = mmioSeek (pVidInfo->hmmioSS, lPosDesired, sSeekMode);

            /********************************************
             * Ensure we did not move to within the header
             ********************************************/
 if ((lNewFilePosition != MMIO_ERROR) &&
                (lNewFilePosition < PNG_HEADER_SIZE))
    {
    lNewFilePosition = mmioSeek (pVidInfo->hmmioSS, (LONG)PNG_HEADER_SIZE, SEEK_SET);
    }

            /************************************************************
             * Return new position.  Always remove the length of the
             *    header from the this position value
             ************************************************************/
 if (lNewFilePosition != MMIO_ERROR)
     lNewFilePosition -= PNG_HEADER_SIZE;

 return (lNewFilePosition);
 }
