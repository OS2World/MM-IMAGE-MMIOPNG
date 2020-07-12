/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: PNGPROC.C                                         */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/*                                                                      */
/* ABSTRACT: This file contains the MMIO                                */
/*           IOProc for PNG files.                                      */
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



LONG EXPENTRY IOProc_Entry (PVOID  pmmioStr, USHORT usMsg, LONG   lParam1, LONG   lParam2)
    {
    PMMIOINFO   pmmioinfo = (PMMIOINFO) pmmioStr ;    /* MMIOINFO block */
    switch (usMsg)
        {

        case MMIOM_OPEN:
            {
            //Message("MMIOM_OPEN") ;
            return Handle_MMIO_Open( pmmioinfo, lParam1, lParam2) ;
            } /* end case of MMIOM_OPEN */

        case MMIOM_CLOSE:
            {
            //Message("MMIOM_CLOSE") ;
            return Handle_MMIO_Close( pmmioinfo, lParam1, lParam2 ) ;
            } /* end case of MMIOM_CLOSE */

        case MMIOM_READ:
            {
            //Message("MMIOM_READ") ;
            return Handle_MMIO_Read( pmmioinfo, lParam1, lParam2) ;
            }   /* end case  of MMIOM_READ */
        case MMIOM_SEEK:
            {
            return Handle_MMIO_Seek( pmmioinfo, lParam1, lParam2)   ;
            }  /* end case of MMIOM_SEEK */
        case MMIOM_GETHEADER:
            {
            //Message("MMIOM_GETHEADER") ;
            return Handle_MMIO_GetHeader(pmmioinfo, lParam1, lParam2)   ;
            } /* end case of MMIOM_GETHEADER */
        case MMIOM_IDENTIFYFILE:
            {
            //Message("MMIOM_IDENTIFYFILE") ;
            return Handle_MMIO_IdentifyFile( pmmioinfo, lParam1, lParam2)   ;
            } /* end case of MMIOM_IDENTIFYFILE */
        case MMIOM_QUERYHEADERLENGTH:
            {
            return Handle_MMIO_QueryHeaderLength(pmmioinfo, lParam1, lParam2)   ;
            } /* end case of MMIOM_QUERYHEADERLENGTH */
        case MMIOM_GETFORMATINFO:
            {
            //Message("MMIOM_GETFORMATINFO") ;
            return Handle_MMIO_GetFormatInfo( pmmioinfo, lParam1, lParam2) ;
            } /* end case of MMIOM_GETFORMATINFO */
        case MMIOM_GETFORMATNAME:
           {
            //Message("MMIO_GETFORMATNAME") ;
            return Handle_MMIO_GetFormatName( pmmioinfo, lParam1, lParam2) ;
            }  /* end case of MMIOM_GETFORMATNAME */
        case MMIOM_VAFFA:
            {
            Message( (PSZ) lParam1 ) ; return (MMIO_ERROR) ;
            }
        default:
            {
             /* Declare Local Variables. */
             PPNGFILESTATUS       pVidInfo;
             LONG                lRC;

             if (!pmmioinfo)
                 return (MMIO_ERROR);

             pVidInfo = (PPNGFILESTATUS) pmmioinfo->pExtraInfoStruct;
             if (pVidInfo != NULL && pVidInfo->hmmioSS)
                {
                 lRC = mmioSendMessage (pVidInfo->hmmioSS, usMsg, lParam1, lParam2);
                 if (!lRC)
                    pmmioinfo->ulErrorRet = mmioGetLastError (pVidInfo->hmmioSS);
                 return (lRC);
                }
            else
               {
                if (pmmioinfo != NULL)
                   pmmioinfo->ulErrorRet = MMIOERR_UNSUPPORTED_MESSAGE;
                return (MMIOERR_UNSUPPORTED_MESSAGE);
               }
            }   /* end case of Default */
        } /* end SWITCH statement for MMIO messages */
    return (0);
    }      /* end of window procedure */
