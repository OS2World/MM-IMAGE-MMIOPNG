//#define _DUMMY_
/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: MM_OPEN.C                                          */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for M-Motion Video       */
/*                                                                      */
/* ABSTRACT: This file contains the MMIO_OPEN                           */
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

#define     MMPPNG_INFO    (pVidInfo->pngInfo)
#define     MMPPNG_STRUCT  (pVidInfo->pngStruct)
#define     WIDTH    (MMPPNG_INFO->width)
#define     HEIGHT   (MMPPNG_INFO->height)
#define     ROWBYTES (MMPPNG_INFO->rowbytes)
#define     BMPROWBYTES  ( MMPPNG_INFO->width * 3)
#define     BMPIMGBUFSIZE ( HEIGHT * ROWBYTES )
#define     VI_COLOR_TYPE (MMPPNG_INFO->color_type)
#define     RAW_BYTES_LENGTH  (WIDTH * usBytesPerPixel)



LONG    HandleAlpha( PPNGFILESTATUS pVInfo ) ;

LONG  Handle_MMIO_Open( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)
 {
 PPNGFILESTATUS   pVidInfo;   /* pointer to a PNG file status structure */
 MMIMAGEHEADER   MMImgHdr;
 HMMIO           hmmioSS;
 PBYTE           lpRGBBufPtr;
 char buffer[100] ;
 ULONG           ulRowCount ;
 USHORT          usPadBytes ;
 USHORT          usBytesPerPixel = 3 ;
 MMIOINFO        mmioinfoSS;         /* I/O info block for SS ref*/
 PSZ pszFileName = (CHAR *)lParam1;  /* get the filename from    */
                                     /* parameter                */
 int iSteps = 1 ;
 int iPasso ;

 if (!pmmioinfo)
    return (MMIO_ERROR);
 if ((pmmioinfo->ulFlags & MMIO_READWRITE) &&
    ((pmmioinfo->ulTranslate & MMIO_TRANSLATEDATA) ||
    (pmmioinfo->ulTranslate & MMIO_TRANSLATEHEADER)))
    {    return (MMIO_ERROR);    }

 if(MMIO_SUCCESS != DetermineStorageSystem( pmmioinfo, pszFileName ) )
    return (MMIO_ERROR) ;
 #ifdef _ERR_TRACK_
 Message( "MMIO_OPEN:dopo DetermineStorageSystem...") ;
 #endif
/****************************************************************
* Invia le operazioni di I/O attraverso ( MMIOINFO ) mmioInfoSS *
****************************************************************/
 memset( &mmioinfoSS, '\0', sizeof(MMIOINFO));
 memmove( &mmioinfoSS, pmmioinfo, sizeof(MMIOINFO));
 mmioinfoSS.pIOProc = NULL;
 mmioinfoSS.fccIOProc = pmmioinfo->fccChildIOProc;
 mmioinfoSS.ulFlags |= MMIO_NOIDENTIFY;
/************************************************************
* Try to open the file.  Add the NO IDENTIFY flag to        *
*    ENSURE THAT WE DON'T LOOP RECURSIVELY!!!               *
************************************************************/
 hmmioSS = mmioOpen (pszFileName, &mmioinfoSS, mmioinfoSS.ulFlags);
 if (pmmioinfo->ulFlags & MMIO_DELETE)
    {            /* was the delete successful?         */
    if (!hmmioSS)
      { pmmioinfo->ulErrorRet = MMIOERR_DELETE_FAILED; return (MMIO_ERROR); }
    else
      return (MMIO_SUCCESS);
    }

 if (!hmmioSS)
    return (MMIO_ERROR);

 /* Allocate memory for one PNG FileStatus structures */
  DosAllocMem ((PPVOID) &pVidInfo, sizeof (PNGFILESTATUS), fALLOC);
 if (!pVidInfo)
    { mmioClose (hmmioSS, 0) ; return (MMIO_ERROR) ; }

 DosAllocMem ((PPVOID) &(MMPPNG_STRUCT), sizeof (png_struct), fALLOC);
 if (!MMPPNG_STRUCT)
    { mmioClose (hmmioSS, 0) ; DosFreeMem ((PVOID) pVidInfo); return (MMIO_ERROR) ; }

 DosAllocMem ((PPVOID) &(MMPPNG_INFO), sizeof (png_info), fALLOC);
 if (!MMPPNG_STRUCT)
    { mmioClose (hmmioSS, 0) ; DosFreeMem ((PVOID) MMPPNG_STRUCT);
      DosFreeMem ((PVOID) pVidInfo); return (MMIO_ERROR) ; }

 pVidInfo->hmmioSS = hmmioSS;
 pmmioinfo->pExtraInfoStruct = (PVOID)pVidInfo;
 #ifdef _ERR_TRACK_
 Message( "MMIO_OPEN:dopo i tre DosAllocMem") ;
 #endif
 if( setjmp( MMPPNG_STRUCT->jmpbuf))
   {
   png_read_destroy( MMPPNG_STRUCT, MMPPNG_INFO, (png_info*)0 ) ;
   return FugaVeloce(pVidInfo) ;
   }

 InitFileStruct (pVidInfo); // Imposta Le varie cazzate in pVidInfo
 if (pmmioinfo->ulFlags & MMIO_READ)
    {
    png_read_info( MMPPNG_STRUCT, MMPPNG_INFO ) ;

    if (  VI_COLOR_TYPE == PNG_COLOR_TYPE_PALETTE ) //&&
          //MMPPNG_INFO->bit_depth < 8  )
          png_set_expand( MMPPNG_STRUCT ) ;

    if (  VI_COLOR_TYPE == PNG_COLOR_TYPE_GRAY &&
          MMPPNG_INFO->bit_depth < 8  )
          png_set_expand( MMPPNG_STRUCT ) ;

    if( MMPPNG_INFO->valid & PNG_INFO_tRNS )
       {
       png_set_expand( MMPPNG_STRUCT ) ;
       #ifdef _ERR_TRACK_
       Message( "PNG_INFO_tRNS" ) ;
       #endif
       }

   if( MMPPNG_INFO->valid & PNG_INFO_bKGD )
      {
      png_set_background(MMPPNG_STRUCT, &MMPPNG_STRUCT->background,
                         PNG_BACKGROUND_GAMMA_FILE, 1 , 1.0 ) ;
      #ifdef _ERR_TRACK_
      Message( "PNG_INFO_bKGD" ) ;
      #endif
      }
   else
      {
         png_color_16 back = { 0, 0, 0, 0, 0 } ;
         png_set_background(MMPPNG_STRUCT, &back,
                    PNG_BACKGROUND_GAMMA_SCREEN, 0 , 1.0 ) ;
         #ifdef _ERR_TRACK_
         Message( "MMIO_OPEN: ! PNG_INFO_bKGD" ) ;
         #endif
      }

   if ( MMPPNG_INFO->valid & PNG_INFO_gAMA )
      png_set_gamma( MMPPNG_STRUCT, 1.0 /*0.45*/, MMPPNG_INFO->gamma ) ;
   else
      png_set_gamma( MMPPNG_STRUCT, 1.0 /*0.45*/, 1.0 ) ;

    // se vi sono 16 bit per ogni canale li porta ad 8
    if (MMPPNG_INFO->bit_depth == 16)
       {
       png_set_strip_16(MMPPNG_STRUCT);
       }


   if (  VI_COLOR_TYPE == PNG_COLOR_TYPE_GRAY_ALPHA
      || VI_COLOR_TYPE == PNG_COLOR_TYPE_GRAY )
            {
            #ifdef _ERR_TRACK_
            Message( "Color type: GRAY||GRAY_ALPHA" ) ;
            #endif
            png_set_gray_to_rgb(MMPPNG_STRUCT);
            }


   png_set_bgr(MMPPNG_STRUCT); // non saprei perche'...


   usPadBytes = (USHORT) ((WIDTH * usBytesPerPixel) % 4) ;
   if(usPadBytes)
      {

      ////Message("usPadBytes != 0");
      pVidInfo->ulPaddedBytesPerLine = RAW_BYTES_LENGTH + 4 - usPadBytes ;
      }
   else
      {
      pVidInfo->ulPaddedBytesPerLine = RAW_BYTES_LENGTH ;
      }

    //sprintf( buffer, "Ul PadB... = %d", pVidInfo->ulPaddedBytesPerLine) ;
    ////Message(buffer) ;
    pVidInfo->ulImgTotalBytes = pVidInfo->ulPaddedBytesPerLine * HEIGHT ;//RAW_BYTES_LENGTH * HEIGHT  ;

                /************************************************************
                * If the app intends to read in translation mode, we must
                * allocate and set-up the buffer that will contain the RGB data
                * We must also read in the data to insure that the first
                * read, seek, or get-header operation will have data
                * to use.  This is ONLY NECESSARY FOR TRANSLATED MODE
                * operations, since we must process reads/writes pretending
                * the image is stored from the Bottom-up.
                ************************************************************/

                /************************************************************
                * Fill out the MMIMAGEHEADER structure.
                ************************************************************/
    MMImgHdr.ulHeaderLength = sizeof (MMIMAGEHEADER); // ??????
    MMImgHdr.ulContentType  = MMIO_IMAGE_PHOTO;
    MMImgHdr.ulMediaType    = MMIO_MEDIATYPE_IMAGE;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cbFix = sizeof (BITMAPINFOHEADER2);
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cx              = WIDTH;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cy              = HEIGHT;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cPlanes         = 1;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cBitCount       = 24;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.ulCompression   = BCA_UNCOMP;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cbImage         = BMPIMGBUFSIZE ;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cxResolution    = 0L;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cyResolution    = 0L;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cclrUsed        = 0L;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cclrImportant   = 0L;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.usUnits         = 0L;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.usReserved      = 0L;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.usRecording     = BRA_BOTTOMUP;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.usRendering     = BRH_NOTHALFTONED;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cSize1          = 0L;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.cSize2          = 0L;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.ulColorEncoding = 0L;
    MMImgHdr.mmXDIBHeader.BMPInfoHeader2.ulIdentifier    = 0L;


    pVidInfo->mmImgHdr = MMImgHdr;

         /*********************************************************
         * For translated data READ mode, we must allocate a buffer,
         *    then get the data from the file and load the RGB buffer.
         * PLACE FORMAT SPECIFIC CODE HERE TO LOAD IMAGE INTO BUFFER
         *********************************************************/
   if (pmmioinfo->ulTranslate & MMIO_TRANSLATEDATA)
     {
      /* Crea il buffer per contenere i dati dell'mmagine */
     if (DosAllocMem ((PPVOID) &(pVidInfo->lpRGBBuf), pVidInfo->ulImgTotalBytes, fALLOC))
          { return FugaVeloce(pVidInfo) ; }
      /********************************************************
      * Initialize beginning buffer position
      ********************************************************/
      lpRGBBufPtr = pVidInfo->lpRGBBuf;
      if( MMPPNG_INFO->interlace_type == 1 )
         {
         iSteps = png_set_interlace_handling( MMPPNG_STRUCT) ;
         }

      for ( iPasso = 0 ; iPasso < iSteps ; iPasso ++ )
         {
         /* Legge una riga per volta */
         for (ulRowCount = 0; ulRowCount < HEIGHT ; ulRowCount++)
           {
           png_read_rows( MMPPNG_STRUCT, &lpRGBBufPtr, NULL, 1 ) ;
           lpRGBBufPtr += (LONG)pVidInfo->ulPaddedBytesPerLine ;
           }   /* end of FOR loop to read RGB data */
         lpRGBBufPtr = pVidInfo->lpRGBBuf;
         }

     /* Corregge ALPHA_CHANNEL */
     lpRGBBufPtr = pVidInfo->lpRGBBuf;

     lpRGBBufPtr = pVidInfo->lpRGBBuf;
     ImgBufferFlip ( lpRGBBufPtr, (LONG)pVidInfo->ulPaddedBytesPerLine, HEIGHT )   ;
     pVidInfo->lImgBytePos =  0 ;
     }   /*  end IF TRANSLATED block */
   } /* end IF READ block */
 return (MMIO_SUCCESS);
 } /* end case of MMIOM_OPEN */





/************************************************************************************
* Determine the storage system/child IOProc that actually obtains the data for us. *
***********************************************************************************/
LONG  DetermineStorageSystem( PMMIOINFO pmmioinfo, PSZ pszFileName )
   {
   FOURCC          fccStorageSystem;   /* SS I/O Proc FOURCC       */

   if (!pmmioinfo->fccChildIOProc)
      {
      if (pmmioinfo->ulFlags & MMIO_CREATE)
        {
        // per il momento non lo supporto !
        Message ("MMIO_CREATE non supportato") ; return (MMIO_ERROR) ;
        /* Need to determine SS if create from pmmioinfo and filename. */
        if (mmioDetermineSSIOProc( pszFileName, pmmioinfo, &fccStorageSystem, NULL ))
           fccStorageSystem = FOURCC_DOS;
        }
      else
        {
        // mmioIdentifyStorageSystem ritorna 0 se OK
        if (mmioIdentifyStorageSystem( pszFileName, pmmioinfo, &fccStorageSystem ))
           return (MMIO_ERROR);
        }
      if (!fccStorageSystem)
         return (MMIO_ERROR);
      else
         pmmioinfo->fccChildIOProc = fccStorageSystem;
      }
  return MMIO_SUCCESS ;
  } /* end storage system identification block */

LONG    HandleAlpha( PPNGFILESTATUS pVInfo )
 {
 int x, y ;
 SHORT padBytes ;
 PCHAR lpA ;
 PCHAR lpRGB ;
 PCHAR tmpBuf ;
 lpA =  lpRGB = pVInfo->lpRGBBuf; ;

 padBytes =  ((pVInfo->pngInfo->width * 3) % 4) ;
 if( padBytes != 0 )
    //Message( "HandleAlpha: padBytes != 0" ) ;
 for ( y = 0 ; y < pVInfo->pngInfo->height ; y++ )
    {
    for ( x = 0 ; x < pVInfo->pngInfo->width ; x++ )
      {
      *lpA = *lpRGB ; lpA++ ; lpRGB++ ;
      *lpA = *lpRGB ; lpA++ ; lpRGB++ ;
      *lpA = *lpRGB ; lpA++ ; lpRGB++ ; lpRGB++ ;
      }
    if( padBytes != 0)
      lpA += padBytes ;
    }
 return 1L ;
 }

/************************************************************************
 * Function : ImgBufferFlip
 * Description :
 *    This function will take a full image buffer and reverse the order of
 *    the lines, effectively flipping the image vertically.
 *    This is needed because MMotion stores the YUV image data from the
 *    top down, and PM Bitmaps store data from bottom up.
 *
 *    THE IOPROC IS EXPECTED TO PASS DATA TO, AND ACCEPT DATA FROM THE
 *    APPLICATION IN BOTTOM UP ORDER!!
 ************************************************************************/
BOOL ImgBufferFlip(PBYTE lpImgBuf,    /* Buffer to flip             */
          ULONG ulBytesPerRow,    /* Total bytes - w/ pads, etc */
          ULONG ulHeight)
{                /* Rows high                  */
    PBYTE           lpTempBuf;
    ULONG           ulRowCount;
    ULONG           ulImgTotalBytes;
    ULONG           ulLastLine;
    PBYTE           lpFrom;
    PBYTE           lpTo;

    ulImgTotalBytes = (ulBytesPerRow * ulHeight);
    ulLastLine = ulImgTotalBytes - ulBytesPerRow;

    if (DosAllocMem((PPVOID) & lpTempBuf, ulBytesPerRow, fALLOC))
        return (FALSE);

    lpFrom = (PBYTE) lpImgBuf;
    lpTo = (PBYTE) & lpImgBuf[ulLastLine];

    for (ulRowCount = 0; ulRowCount < (ulHeight >> 1); ulRowCount++) {
        memcpy(lpTempBuf, lpFrom, ulBytesPerRow);
        memcpy(lpFrom, lpTo, ulBytesPerRow);
        memcpy(lpTo, lpTempBuf, ulBytesPerRow);
        lpFrom += ulBytesPerRow;
        lpTo -= ulBytesPerRow;
    }

    DosFreeMem((PVOID) lpTempBuf);
    return (TRUE);
}                /* end of function call  */

