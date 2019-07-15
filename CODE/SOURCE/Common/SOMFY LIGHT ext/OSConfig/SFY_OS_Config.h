/**********************************************************************
This document and/or file is SOMFY’s property. All information it
contains is strictly confidential. This document and/or file shall
not be used, reproduced or passed on in any way, in full or in part
without SOMFY’s prior written approval. All rights reserved.
Ce document et/ou fichier est la propriété de SOMFY. Les informations
qu’il contient sont strictement confidentielles. Toute reproduction,
utilisation, transmission de ce document et/ou fichier, partielle ou
intégrale, non autorisée préalablement par SOMFY par écrit est
interdite. Tous droits réservés.
***********************************************************************/

//--------------------------------------------------------------------------
// SOMFY Operating System Abstraction Layer - Version 0.5 - 13/05/2008
// Configuration file
//--------------------------------------------------------------------------


//---------------------------------------------------------------------
// File "SFY_OS_Config.h"   Generated with SOMFY code generator v[3.5.0.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef SFY_OS_CONFIG_H
#define SFY_OS_CONFIG_H


// Public types
//--------------

// 1st step : TASKS CONFIGURATION
// To do : declare your tasks ids classified by decreasing priority
// -------------------------------------------------------------------------
typedef enum
{
  OS_TASKS_NUMBER, // Warning : must be the last one !
} eTask;



// 2nd step : TASKS STACKS CONFIGURATION
// To do : declare the size of all task stacks (in Words)
// Tips : declare a #define for each tasks, then sum them 
// Example following :
// #define OS_TASK_1_STACK_SIZE 100
// #define OS_TASK_2_STACK_SIZE 100 
// #define OS_STACK_SIZE_IN_WORDS (OS_TASK_1_STACK_SIZE + OS_TASK_2_STACK_SIZE)
// -------------------------------------------------------------------------
#define OS_STACK_SIZE_IN_WORDS 100



// 3rd step : INTERRUPT STACK CONFIGURATION
// To do : 1 - Declare the size of the interrupt stack (in Words)
//         2 - Declare the maximum number of OS calls in one interrupt
// ------------------------------------------------------------------------- 

// Declare the size of the interrupt stack (in Words)
#define OS_INTERRUPT_STACK_SIZE_IN_WORDS 200

// Declare the maximum number of OS calls in one interrupt
#define OS_MAXIMUM_OS_CALLS_DURING_INTERRUPT 5



// 4th step : TASK EVENTS CONFIGURATION
// To do : for each tasks, declare all your events   
// WARNING : some OS are limited to 8 events. If using more, be sure that 
// your OS will accept it and that you won't swap to another OS one day !
// Example following :       
// #define OS_TASK1_EXAMPLE_EVT_1 (1<<0)
// #define OS_TASK1_EXAMPLE_EVT_2 (1<<1)
// #define OS_TASK1_EXAMPLE_EVT_3 (1<<2) 
// -------------------------------------------------------------------------



// 5th step : MESSAGE BOXES CONFIGURATION
// To do : 1 - Uncomment the #define if you are using message boxes   
//         2 - Declare your message boxes in the enum
//         3 - Indicate the maximum number of messages for your application
// ------------------------------------------------------------------------- 

// Uncomment if using message boxes
// #define YES_I_AM_USING_MESSAGE_BOXES

// Enumerate all your message boxes
typedef enum
{
  OS_MESSAGE_BOXES_NUMBER, // Warning : must be the last one !
} eMessageBox;

// Indicate the maximum number of messages
#define OS_MAXIMUM_NUMBER_OF_MESSAGES 10



// 6th step : RESOURCES CONFIGURATION
// To do : 1 - Uncomment the #define if you are using resources   
//         2 - Declare your resources in the enum
// ------------------------------------------------------------------------- 

// Uncomment if using resources
// #define YES_I_AM_USING_RESOURCES

// Enumerate your resources
typedef enum
{
  OS_RESOURCES_NUMBER, // Warning : must be the last one !
} eResource;



// 7th step : TIMERS CONFIGURATION
// To do : 1 - Uncomment the #define if you are using timers   
//         2 - Declare your timers in the enum
// ------------------------------------------------------------------------- 

// Uncomment if using timers
// #define YES_I_AM_USING_TIMERS

// Enumerate your timers
typedef enum
{
  OS_TIMERS_NUMBER, // Warning : must be the last one !
} eTimer;



// 8th step : OS TICKS CONFIGURATION
// To do : indicate the number of µs per OS tick   
// WARNING : On Windows, will be automatically set to 1000 µs
// -------------------------------------------------------------------------
#define OS_NUMBER_OF_US_PER_TICK 1000

// 9th step : OS CRITICAL SECTION CONFIGURATION
// To do : indicate ENTRY EXIT Functions to be called   
// -------------------------------------------------------------------------
#define SFY_OS_CRITICAL_SECTION_ENTRY()
#define SFY_OS_CRITICAL_SECTION_EXIT()

// Header end
//--------------
#endif // SFY_OS_CONFIG_H

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
