/********************** START OF SPECIFICATIONS *************************/
/*                                                                      */
/* SOURCE FILE NAME: PNGPROC.H                                          */
/*                                                                      */
/* DESCRIPTIVE NAME: Multi-Media I/O Procedure for PNG files            */
/*                                                                      */
/* COPYRIGHT:                                                           */
/*              Copyright (c) Giorgio Costa    1996, 1997               */
/*                        All Rights Reserved                           */
/*                                                                      */
/* ABSTRACT: This file is the include/header file for use with the      */
/*           PNG      IOProc (file PNG.C)                               */
/*                                                                      */
/*********************** END OF SPECIFICATIONS **************************/

//#pragma pack(2)
#pragma pack(1)
#include <os2.h>
#include <os2medef.h>
#include <mmioos2.h>

#include "png.h"

static char     pszMotionExt  [] = "PNG";

typedef RGB FAR *PRGB;

typedef struct _PNGHEADER
  {
  char buffer[8];
  }   PNGHEADER ;

typedef PNGHEADER* PPNGHEADER ;

#define     PNG_HEADER_SIZE        sizeof( PNGHEADER )

/****************************************
 * IOProc information structure, used for every file opened
 *    by this IOProc
 ****************************************/
typedef struct _PNGFILESTATUS
    {
    png_structp pngStruct            ;
    png_infop  pngInfo              ;
    ULONG   ulImgTotalBytes;
    ULONG  ulPaddedBytesPerLine ;
    PBYTE   lpRGBBuf ;              /* 24-bit RGB Buf for trans data */
    PBYTE   lpImgBuf ;              // Usato solo per la scrittura -@-@-@-
    LONG    lImgBytePos;            /* Current pos in RGB buf        */
    BOOL    bSetHeader;             /* TRUE if header set in WRITE mode*/
    HMMIO   hmmioSS  ;              /* MMIO handle to Storage System IOProc that provides data.      */
    PNGHEADER pngHeader ;
    MMIMAGEHEADER   mmImgHdr;       /* Standard image header         */
    ULONG   ulFlags ;
    } PNGFILESTATUS;
typedef PNGFILESTATUS FAR *PPNGFILESTATUS;


#define     ONE_BLOCK                   6L
#define     FOURCC_MMOT                 mmioFOURCC('P', 'P', 'N', 'G')

/* RC file defines */
#define     HEX_FOURCC_MMOT             0x474E5050L
#define     MMOTION_IOPROC_NAME_TABLE   7000
#define     MMOTION_NLS_CHARSET_INFO    7500

#ifndef BITT_NONE
#define BITT_NONE 0
#define BI_NONE   0
#endif

/**********************
 * Function Declarations
 **********************/
LONG EXPENTRY IOProc_Entry (PVOID  pmmioStr, USHORT usMsg, LONG   lParam1, LONG lParam2);
LONG GetFormatString (LONG lSearchId, PSZ  pszFormatString, LONG lBytes);
LONG GetFormatStringLength (LONG  lSearchId, PLONG plNameLength);
ULONG APIENTRY GetNLSData (PULONG, PULONG);

LONG Message(PSZ msg) ;
LONG FugaVeloce(PPNGFILESTATUS pVidInfo)  ;
void InitFileStruct (PPNGFILESTATUS) ;
BOOL ImgBufferFlip (PBYTE   lpImgBuf, ULONG   ulBytesPerRow, ULONG   ulHeight) ;
LONG  Handle_MMIO_Open( PMMIOINFO  pmmioInfo, LONG   lParam1, LONG   lParam2)  ;
LONG  Handle_MMIO_Read( PMMIOINFO  pmmioInfo, LONG   lParam1, LONG   lParam2)  ;
LONG  Handle_MMIO_Close( PMMIOINFO  pmmioInfo, LONG   lParam1, LONG   lParam2)  ;
LONG  Handle_MMIO_Seek( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)   ;
LONG  Handle_MMIO_GetFormatInfo( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)   ;
LONG  Handle_MMIO_GetFormatName( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)   ;
LONG  Handle_MMIO_GetHeader( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)   ;
LONG  Handle_MMIO_QueryHeaderLength( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)   ;
LONG  Handle_MMIO_IdentifyFile( PMMIOINFO  pmmioinfo, LONG   lParam1, LONG   lParam2)   ;
LONG  DetermineStorageSystem( PMMIOINFO pmmioinfo, PSZ pszFileName ) ;
/************************
*  definizioni mie      *
************************/
typedef png_struct FAR *PPNG_STRUCT ;
typedef png_info   FAR *PPNG_INFO   ;
#pragma pack()
