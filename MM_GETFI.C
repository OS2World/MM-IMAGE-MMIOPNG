/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: MM_GETFI.C                                          */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/*                                                                      */
/* ABSTRACT: This file contains the MMIO_GETFORMATINFO                  */
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




LONG  Handle_MMIO_GetFormatInfo( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)
 {
 PPNGFILESTATUS   pVidInfo;   /* pointer to a PNG file status structure */
/*********************************************
************* mm_getfi.h *********************
* Il file dovrebbe andare bene cosi' come e' *
*********************************************/

             /***********************************************************
             * Declare local variables.
             ***********************************************************/
            PMMFORMATINFO       pmmformatinfo;
           /************************************************************
             * Set pointer to MMFORMATINFO structure.
             ************************************************************/
            pmmformatinfo = (PMMFORMATINFO) lParam1;

            /******* Debug  ****************/
            //Message( "MMIOM_GETFORMATINFO") ;


            /************************************************************
             * Fill in the values for the MMFORMATINFO structure.
             ************************************************************/
            pmmformatinfo->ulStructLen  = sizeof (MMFORMATINFO);
            pmmformatinfo->fccIOProc    = FOURCC_MMOT;
            pmmformatinfo->ulIOProcType = MMIO_IOPROC_FILEFORMAT;
            pmmformatinfo->ulMediaType  = MMIO_MEDIATYPE_IMAGE;

            pmmformatinfo->ulFlags      = MMIO_CANREADTRANSLATED        |
                                          MMIO_CANREADUNTRANSLATED      |
                                          //MMIO_CANWRITETRANSLATED       |
                                          //MMIO_CANWRITEUNTRANSLATED     |
                                          //MMIO_CANREADWRITEUNTRANSLATED |
                                          MMIO_CANSEEKTRANSLATED        |
                                          MMIO_CANSEEKUNTRANSLATED;

            strcpy ((PSZ) pmmformatinfo->szDefaultFormatExt, pszMotionExt);
            if (GetNLSData( &pmmformatinfo->ulCodePage,
                            &pmmformatinfo->ulLanguage ))
               {
               return( -1L );
               }

            if (GetFormatStringLength( FOURCC_MMOT,
                                       &(pmmformatinfo->lNameLength) ))
               {
               return( -1L );
               }


            /************************************************************
             * Return success back to the application.
             ************************************************************/
            return (MMIO_SUCCESS);
   }
