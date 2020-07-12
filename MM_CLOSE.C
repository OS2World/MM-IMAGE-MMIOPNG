/********************** START OF SPECIFICATIONS *************************/
/* */
/* SOURCE FILE NAME: MM_CLOSE.C                                         */
/* */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/* */
/* ABSTRACT: This file contains the MMIO_CLOSE                          */
/* handling for PNG files.                                   */
/* */
/*********************** END OF SPECIFICATIONS **************************/

/************************************************************************/
/* Put all #defines here                                                */
/************************************************************************/

#define INCL_32
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


LONG Handle_MMIO_Close(PMMIOINFO pmmioinfo, LONG lParam1, LONG lParam2)
{
	ULONG           ulHeight;	/* Image height                */
	USHORT          usBitCount;
	ULONG           ulImgPelWidth;
	PBYTE           lpYUVLine;	/* One line of packed YUV      */
	LONG            lYUVBytesPerLine;
	ULONG           ulMaxPelWidth;	/* # pels on 4-pel boundaries  */
	/* # pels on a YUV line in the output file                       */
	ULONG           ulYUVPelWidth;
	ULONG           ulRGBMaxBytesPerLine;	/* #bytes on 4-pel bounds  */
	PBYTE           lpRGBLine;	/* One line of 24bit RGB       */
	PBYTE           lpImgBufPtr;	/* Current loc in RGB image buf */
	LONG            lRGBBytesPerLine;	/* #bytes on a line in image   */
	ULONG           ulRowCount;	/* loop counter                */
	LONG            lBytesWritten;	/* #bytes output on a write    */
	LONG            lRetCode = MMIO_SUCCESS;
	USHORT          rc;
	PPNGFILESTATUS  pVidInfo;

	//Message("MMIOM_CLOSE");
	if (!pmmioinfo)
		return (MMIO_ERROR);
	pVidInfo = (PPNGFILESTATUS) pmmioinfo->pExtraInfoStruct;

	/************************************************************
         * see if we are in Write mode and have a buffer to write out.
         *    We have no image buffer in UNTRANSLATED mode.
         ************************************************************/
	if ((pmmioinfo->ulFlags & MMIO_WRITE) && (pVidInfo->lpImgBuf)) {
#ifdef __WRITE_ENABLED__
#else
		Message("Scrittura non Supportata !!");
		lRetCode = MMIO_ERROR;
#endif // __WRITE_ENABLED__
	}			/* end IF WRITE & IMAGE BUFFER block */
	/***********************************************************
	             * Free the RGB buffer, if it exists
	             ***********************************************************/
	if (pVidInfo->lpRGBBuf) {
		DosFreeMem((PVOID) pVidInfo->lpRGBBuf);
		//Message("pVidInfo->lpRGBBuf liberato !");
	}
	/********************************
         * Close the file with mmioClose.
         *******************************/
	rc = mmioClose(pVidInfo->hmmioSS, 0);
	DosFreeMem((PVOID) pVidInfo->pngStruct);
	DosFreeMem((PVOID) pVidInfo->pngInfo);
	DosFreeMem((PVOID) pVidInfo);

	if (rc != MMIO_SUCCESS)
		return (rc);
	return (lRetCode);
}
