/* *********************************************************************
/ This document and/or file is SOMFY�s property. All information
/ it contains is strictly confidential. This document and/or file
/ shall not be used, reproduced or passed on in any way, in full
/ or in part without SOMFY�s prior written approval.
/ All rights reserved.
/ Ce document et/ou fichier est la propri�t� de SOMFY.
/ Les informations qu�il contient sont strictement confidentielles.
/ Toute reproduction, utilisation, transmission de ce document
/ et/ou fichier, partielle ou int�grale, non autoris�e
/ pr�alablement par SOMFY par �crit est interdite.
/ Tous droits r�serv�s.
/ ********************************************************************* */
#ifndef SFY_UTILITIES_H
    #define SFY_UTILITIES_H

		#define MEMCPY                        memcpy
		#define MEMCPY_FLASH_TO_RAM           memcpy
		#define MEMCMP                        memcmp
		#define MEMSET                        memset

		#define STRCPY                        strcpy_s
		#define STRNCPY(pdest, pSrc, size)    strncpy_s(pdest, size, pSrc, size)
		#define STRCMP                        strcmp
		#define STRNCMP                       strncmp

#endif // SFY_UTILITIES_H
