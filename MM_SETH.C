/********************** START OF SPECIFICATIONS *************************/
/* SOURCE FILE NAME: MM_SETH.C                                          */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/* ABSTRACT: This file contains the MMIO_SETHEADER                      */
/* handling for PNG files.                                   */
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

LONG Handle_MMIO_SetHeader(PMMIOINFO pmmioinfo, LONG lParam1, LONG lParam2)
{
    /************************************************************
         * Declare local variables.
         ************************************************************/
    PMMIMAGEHEADER  pMMImgHdr;
    PPNGFILESTATUS   pVidInfo;
    USHORT          usNumColors;
    ULONG           ulImgBitsPerLine;
    ULONG           ulImgBytesPerLine;
    ULONG           ulBytesWritten;
    ULONG           ulWidth;
    ULONG           ul4PelWidth;
    ULONG           ulHeight;
    USHORT          usPlanes;
    USHORT          usBitCount;
    USHORT          usPadBytes;

    /************************************************************
         * Check for valid MMIOINFO block.
         ************************************************************/
    if (!pmmioinfo)
        return (MMIO_ERROR);
    pVidInfo = (PPNGFILESTATUS) pmmioinfo->pExtraInfoStruct;

    /************************************************************
         * Only allow this function if we are in WRITE mode
         * And only if we have not already set the header
         ************************************************************/
    if ((!(pmmioinfo->ulFlags & MMIO_WRITE)) || (!(pVidInfo->hmmioSS)) || (pVidInfo->bSetHeader))
        return (0);

    /********************************************************
         * Make sure lParam1 is a valid pointer
         ********************************************************/
    if (!lParam1) {
        pmmioinfo->ulErrorRet = MMIOERR_INVALID_STRUCTURE;
        return (0);
    }
    /************************************************************
         * Header is not in translated mode.
         ************************************************************/
    if (!(pmmioinfo->ulTranslate & MMIO_TRANSLATEHEADER)) {
        /********************************************************
                 * Make sure lParam2 is correct size
                 ********************************************************/
        if (lParam2 != MMOTION_HEADER_SIZE) {
            pmmioinfo->ulErrorRet = MMIOERR_INVALID_BUFFER_LENGTH;
            return (0);
        }
        /********************************************************
                 * Ensure that the header at least begins with "YUV12C"
                 ********************************************************/
        if (strncmp((char *) lParam1, "YUV12C", 6)) {
            pmmioinfo->ulErrorRet = MMIOERR_INVALID_STRUCTURE;
            return (0);
        }
        /********************************************************
                 * Take 16 byte buffer (lParam1), write to file and .
                 *    copy to internal structure.
                 ********************************************************/
        memcpy((PVOID) & pVidInfo->mmotHeader,
               (PVOID) lParam1, (ULONG) MMOTION_HEADER_SIZE);
        ulBytesWritten = mmioWrite(pVidInfo->hmmioSS,
                       (PVOID) lParam1,
                       (ULONG) MMOTION_HEADER_SIZE);

        /*********************************
                 * Check for an error on the write..
                 *********************************/
        if (ulBytesWritten != MMOTION_HEADER_SIZE)
            return (0);    /* 0 indicates error */

        /*********************************
                 * Success...
                 *********************************/
        pVidInfo->bSetHeader = TRUE;
        return (sizeof(MMOTIONHEADER));
    }            /* end IF NOT TRANSLATED block */
    /************************************************************
                 * Header is translated.
                 ************************************************************/
    /************************************************************
                 * Create local pointer media specific structure.
                 ************************************************************/
    pMMImgHdr = (PMMIMAGEHEADER) lParam1;

    /************************************************************
         * Check for validity of header contents supplied
         ************************************************************
         *  --  Length must be that of the standard header
         *  --  NO Compression
         *      1 plane
         *      24, 8, 4 or 1 bpp
         ************************************************************/
    usBitCount = pMMImgHdr->mmXDIBHeader.BMPInfoHeader2.cBitCount;
    if ((pMMImgHdr->mmXDIBHeader.BMPInfoHeader2.ulCompression !=
         BCA_UNCOMP) ||
        (pMMImgHdr->mmXDIBHeader.BMPInfoHeader2.cPlanes != 1) ||
        (!((usBitCount == 24) || (usBitCount == 8) ||
           (usBitCount == 4) || (usBitCount == 1)))
        ) {
        pmmioinfo->ulErrorRet = MMIOERR_INVALID_STRUCTURE;
        return (0);
    }
    if (lParam2 != sizeof(MMIMAGEHEADER)) {
        pmmioinfo->ulErrorRet = MMIOERR_INVALID_BUFFER_LENGTH;
        return (0);
    }
    /************************************************************
         * Complete MMOTIONHEADER.
         ************************************************************/
    memcpy((PVOID) & pVidInfo->mmotHeader.mmID, "YUV12C", 6);
    pVidInfo->mmotHeader.mmXorg = 0;
    pVidInfo->mmotHeader.mmYorg = 0;

    /********************************************************
         * Ensure we will save stuff on 4-pel boundaries when
         *     we actually convert to YUV and pack the bits.
         * We DON'T change what the user is actually going to
         *     give us.  HE thinks he is on 1-pel boundaries,
         *     and that is how we buffer the RGB data.
         ********************************************************/
    ulWidth = pMMImgHdr->mmXDIBHeader.BMPInfoHeader2.cx;
    ulHeight = pMMImgHdr->mmXDIBHeader.BMPInfoHeader2.cy;
    if (ulWidth % 4)
        ul4PelWidth = (((ulWidth >> 2) + 1) << 2);
    else
        ul4PelWidth = ulWidth;
    pVidInfo->mmotHeader.mmXlen = (USHORT) ul4PelWidth;
    pVidInfo->mmotHeader.mmYlen = (USHORT) ulHeight;

    /************************************************************
         * Write the MMotion Header.
         ************************************************************/
    ulBytesWritten = mmioWrite(pVidInfo->hmmioSS,
                   (PVOID) & pVidInfo->mmotHeader,
                   (ULONG) MMOTION_HEADER_SIZE);

    /***************************************
         * Check for an error on the write...
         ***************************************/
    if (ulBytesWritten != MMOTION_HEADER_SIZE)
        return (0);

    /************************************************************
         * Flag that MMIOM_SETHEADER has been done.  It can only
         *    be done ONCE for a file.  All future attempts will
         *    be flagged as errors.
         ************************************************************/
    pVidInfo->bSetHeader = TRUE;

    /************************************************************
         * Create copy of MMIMAGEHEADER for future use.
         ************************************************************/
    pVidInfo->mmImgHdr = *pMMImgHdr;

    /************************************************************
         * Check bitcount, set palette if less than 24.
         ************************************************************/
    if (usBitCount < 24) {
        /*********************************************************
                 * Find out how many colors are in the palette.
                 *********************************************************/
        usNumColors = (USHORT) (1 << usBitCount);

        /*********************************************************
                 * Take the RGB2 Palette and convert it to an RGB palette
                 *    Place the converted palette in MMFILESTATUS struct
                 *********************************************************/
        RGB2_To_RGB(pVidInfo->mmImgHdr.bmiColors,
                (PRGB) & (pVidInfo->rgbPalette),
                usNumColors);
    }
    /*********************************************************
         * We must allocate the buffer.  The app will load the
         * buffer on subsequent write calls
         *********************************************************/
    usPlanes = pVidInfo->mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cPlanes;

    /********************************************************
         * Determine total Image size
         ********************************************************
         * Find bits-per-line BEFORE padding and 1bpp or 4bpp pel overflow
         ********************************************************/
    ulImgBitsPerLine = ulWidth * usPlanes * usBitCount;
    ulImgBytesPerLine = ulImgBitsPerLine >> 3;

    /********************************************************
         * Account for extra pels not on an even byte boundary
         *   for 1bpp and 4bpp
         ********************************************************/
    if (ulImgBitsPerLine % 8)
        ulImgBytesPerLine++;

    pVidInfo->ulImgPelBytesPerLine = ulImgBytesPerLine;

    /********************************************************
         * Ensure the row length in bytes accounts for byte padding.
         *   All bitmap data rows are aligned on LONG/4-BYTE boundaries.
         *   The data FROM an application should always appear in this form
         ********************************************************/
    usPadBytes = (USHORT) (ulImgBytesPerLine % 4);
    if (usPadBytes)
        ulImgBytesPerLine += 4 - usPadBytes;

    pVidInfo->ulImgPaddedBytesPerLine = ulImgBytesPerLine;
    pVidInfo->ulImgTotalBytes = ulImgBytesPerLine * ulHeight;

    /********************************************************
         * Get space for full image buffer.
         ********************************************************/
    if (DosAllocMem((PPVOID) & (pVidInfo->lpImgBuf),
            pVidInfo->ulImgTotalBytes,
            fALLOC))
        return (MMIO_ERROR);

    /********************************************************
         * Set up initial pointer value within RGB buffer & image
         ********************************************************/
    pVidInfo->lImgBytePos = 0;

    return (sizeof(MMIMAGEHEADER));
}                /* end case of MMIOM_SETHEADER */
