/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: MM_IDENT.C                                         */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/*                                                                      */
/* ABSTRACT: This file contains the MMIO_IDENTIFYFILE                   */
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


LONG  Handle_MMIO_IdentifyFile( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)
   {
   PPNGFILESTATUS   pVidInfo;
        /*############## mm_ident.h ###################################*
         * Identify whether this file can be processed.
         *#############################################################*/
            /************************************************************
             * Declare local variables.
             ************************************************************/
   UCHAR           tmpbuf[8] ;
   UCHAR           refbuf[9] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, '\0' } ;
   PNGHEADER       pngHeader;    /* MMotion structure variable*/
   HMMIO           hmmioTemp;     /* MMIO File Handle          */
   ULONG           ulWidth;
   ULONG           ulHeight;
   ULONG           ulRequiredFileLength;
   ULONG           ulActualFileLength;
   BOOL            fValidPNGFile = FALSE;
   int i ;
   ULONG           ulTempFlags = MMIO_READ | MMIO_DENYWRITE | MMIO_NOIDENTIFY;
                   /* flags used for temp open  */
                   /* and close                 */

              /************************************************************
             * We need either a file name (lParam1) or file handle (lParam2)
             ************************************************************/
   if (!lParam1 && !lParam2)
          return (MMIO_ERROR);

            /* Copy the file handle, assuming one was provided... */
   hmmioTemp = (HMMIO)lParam2;

            /************************************************************
             * If no handle, then open the file using the string name
             ************************************************************/
   if (!hmmioTemp)
         {
         if (!(hmmioTemp = mmioOpen ((PSZ) lParam1, NULL, ulTempFlags)))
             {
             return (MMIO_ERROR);
             }
         }

            /************************************************************
             * Read in enough bytes to check out file.
             ************************************************************/
   if (sizeof (PNGHEADER) != mmioRead (hmmioTemp,(PVOID) tmpbuf, 8))
        {
                /********************************************************
                 * Fail so close file and then return.
                 ********************************************************/
         if (!lParam2) /* Don't close handle if provided to us  */
              mmioClose (hmmioTemp, 0);
              return (MMIO_ERROR);
        }

            /************************************************************
             * Close file before returning.
             ************************************************************/
        if (!lParam2) /* Don't close handle if provided to us  */
              mmioClose (hmmioTemp, 0);

            /************************************************************
             * Check validity of file and return result.
             ************************************************************/
   fValidPNGFile = TRUE ;
   for( i = 0 ; i < 8 ; i++ )
       if( tmpbuf[i]!=refbuf[i] )
           {
           fValidPNGFile = FALSE; break ;
           }

           /************************************************************
             * Close file before returning.
             ************************************************************/
   if (!lParam2)  /* Don't close handle if provided to us      */
        mmioClose (hmmioTemp, 0);

   if (fValidPNGFile)
       {  //Message("Il file PNG e' valido!") ;
        return (MMIO_SUCCESS); }
   else
       { //Message("Il file PNG non e' valido!") ;
         return (MMIO_ERROR); }
 }
