// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

/**
* @file STM8_Archiver.c
* @brief Archiver manage persistant data write and load in eeprom.
* @details 
* @author Gabriel Mula
* @version xx
* @date Septembre 2015
*
*/
#include <string.h>                     //memcpy ext...
#include "CRC.h"                        //obvious...
#include "SFY_Macros.h"                 //GET_MSBYTE
#include "STM8_Archiver.h"
#include "SQC_DataProvider.h"

typedef struct Archiver_Pdata_SQC_s
{
  TU32 CriticalAreaWrite;     //关键区域写
  TU32 NormalAreaWrite;       //普通区域写
  TU8  CriticalAreaWriteFail; //关键区域写失败
  TU8  NormalAreaWriteFail;   //普通区域写失败
  TU8  CriticalAreaReadError; //关键区域读取错误
  TU8  CriticalAreaReadFail;  //关键区域读取失败
}Archiver_Pdata_SQC_s;

typedef struct Archiver_SQC_s
{
  TU16 SpaceUsedInByte;                                 ///< Space (in byte) used by registered object
  TU16 UsableSpace  ;                                   ///< Space (in byte) used by registered object
  Archiver_Pdata_SQC_s Counter;
}Archiver_SQC_s;

/**
*  @struct Object_FootPrint_t
*  @brief Data stored about an object by the archiver
*  @details The Archiver need some information about object. Those information are store in this structure.
*/
typedef struct
{
  void *pt_OnObjectInRam;                             ///< Pointer on registered object 
  void (*pf_SetToFactory)(void);                      ///< In case of corrupt block, this function will be called to reset object with default value
  TU8 Object_Size;                                    ///< object size in byte      
  TU8 Object_Eeprom_RelativeBlockStartAddress;        ///< Object address, relative to physical start of block adress. Exemple : Block start adress = 0x4000,Object address = 0x4004, Object_Eeprom_RelativeBlockStartAddress = 4
  TU8 Object_ParentLogicBlock;                        ///< Object is store in block number Object_ParentLogicBlock
}Object_FootPrint_t;

/**
*  @struct BlockType_e
*  @brief Type of Logical Block.
*  @details Archiver use two different type of block
*  The DataBlock, who do not replicate data in memorie and who are dangerous in case of write durring power cut
*  CriticalBlock, who replicate data in memorie and who are safe in case of write durring power cut
*/
typedef enum
{
  DataBlock,                    ///< Block is a simple data block, write once, no copy, write unsafe on powercut.普通数据块 写一次 不需要备份
  CriticalBlock,                ///< Block is a critical block, Copy are made, safe write.                       关键数据块 需要备份
}BlockType_e;

/**
*  @struct LogicBlockReg_t
*  @brief LogicBlock definition. 
*  @details Represent a logical block, store some important data like space left in block ext...
*/
typedef struct
{ 
  BlockType_e BlockType;                        ///< Block type @see BlockType_e           数据块类型
  TU8 FreeSpaceleft;                            ///< space left (in byte) in this block    剩余空间
  TU8 BlockRelativeFreePosition;                ///< Where the free space in block start (position relative to start of block).   空数据地址
  TU8 NumberOffObjectRegisteredInThisBlock;     ///< Count how many object are registered to this block                           已使用多少
  TU8 PhysicalBlockID;                          ///< Critical block use multiple contiguous physical block,So for BlockType == CriticalBlock, PhysicalBlockID is the first Physical Block of the Logical Block                                      
}LogicBlockReg_t;
                                                 
/**
*  @struct Archiver_t
*  @brief Archiver object Structure 
*  @details Store an array of registered objects and an array of block used by archiver
*/
typedef struct
{
  Archiver_SQC_s SQC;
  TU8 RegisteredObject_Ctr;                             ///< Counter of registered object
  
  Object_FootPrint_t RegisteredObject[MAX_NBR_OBJECT];  ///< Object footprint array. @see Object_FootPrint_t  25个数据
  LogicBlockReg_t    LogicBlock_Table[LOGIC_BLOCK_CNT]; ///< Block type array @see LogicBlockReg_t            5块
  TU8 WriteBuffer[FLASH_BLOCK_SIZE];                    ///< Buffer used on write operation                   写缓存大小128
}Archiver_t;


Archiver_t Archiver;
#define me Archiver


static TS8   Archiver_SearchBlockWithEnouthSpace(TU8 ObjectSizeInByte,TBool IsObjectCritical);
static void  Archiver_AllocatePhysicalBlock();
static TBool Archiver_IsBlockCorrupt(TU8 PhysicalBlock);
static TU16  Archiver_GetBlockCRC(TU8 PhysicalBlock);
static TU16  Archiver_ComputeBlockCRC(TU8 PhysicalBlock);
static TBool Archiver_WriteBufferToPhysicalBlock(TU8* Buffer,TU8 PhysicalBlock);
static TU16  Archiver_ComputeAndAppendCRC16ToBuffer(TU8 *Buffer,TU16 size);
static TBool Archiver_LoadWriteBuffer(TU8 LogicalBlock,TU8* Buffer);
static TS8   Archiver_GetFirstValidPhysicalBlockID(TU8 LogicalCriticalBlock);
static void  Archiver_LoadObject(TU8 SourcePhysicalBlockId,TU8 LogicalBlockToLoad);
static TBool Archiver_IsBlockVirgin(TU8 Block);
static TBool Archiver_IsAllVirgin();
static TU8*  Get_SQC_data(TU8* DataSize);
static void  Archiver_SetSQCToFactory();

static TS8  Archiver_SearchRegisteredObject(const void* Me);
static void Archiver_SetAllToFactory(void);
//static TU8  Archiver_ReadTU8(TU32 Address) ;
static TU16 Archiver_ReadTU16(TU32 Address);
//static TU32 Archiver_ReadTU32(TU32 Address);
static void Archiver_SetupHardware();



static TU8* Get_SQC_data(TU8* DataSize)
{
  *DataSize = sizeof(me.SQC);
  return (TU8*) &me.SQC;
}

void Archiver_NoSetToFactoryFunction()
{
}
 
/**
 * @brief Initialise Archiver object.
 * @details This function must be called before using Archiver Object. 
 *          It setup STM8 hardware, init Object_FootPrint_t Array and Setup LogicBlockReg_t Array according to definition in STM8_Archiver_Conf.h.
 *          Archiver object is unique (only one instance allowed).
 * @return Archiver_t UNIQUE instance address
 */
void Archiver_Init()
{
  Archiver_SetupHardware();                           //flash 操作配置时钟和解锁
  memset(Archiver.WriteBuffer,0x00,FLASH_BLOCK_SIZE); //写缓存清零  这128
  Archiver_AllocatePhysicalBlock();                   //分配存储块

  for(TU8 i = 0u;i<MAX_NBR_OBJECT;++i)               //MAX_NBR_OBJECT=25组
  {
    me.RegisteredObject[i].Object_Size = 0U;
    me.RegisteredObject[i].Object_Eeprom_RelativeBlockStartAddress = 0U;
    me.RegisteredObject[i].Object_ParentLogicBlock = 0xFF;
    me.RegisteredObject[i].pt_OnObjectInRam = NULL;
  }
  
  me.SQC.SpaceUsedInByte  = 0U; //空间使用
  me.RegisteredObject_Ctr = 0U; //记录目标计数

  //SQC 
  SQC_DataProviderNameInterface_s intf;
  intf.Fptr_Get = Get_SQC_data;
  SQC_DataProvider_RegisterDataSource(SQC_DataProvider_Archiver ,&intf);
  
  Archiver_RegisterObject(&me.SQC.Counter,
                          Archiver_SetSQCToFactory,
                          sizeof(me.SQC.Counter),
                          FALSE);   
}

void Archiver_Create()   //存储数据创建
{
  if(Archiver_IsAllVirgin() == TRUE)  //存储数据为空则恢复出厂设置
  {
     Archiver_SetAllToFactory();
     Archiver_SaveAllObject();
  } 
    Archiver_LoadMe(&me.SQC.Counter); //EEPROM操作导入参数
}

void Archiver_ResetCreate()   //存储数据创建
{
    Archiver_SetAllToFactory();
    Archiver_SaveAllObject();
    Archiver_LoadMe(&me.SQC.Counter); //EEPROM操作导入参数
}

//存储标志恢复出厂设置
static void Archiver_SetSQCToFactory()
{
  me.SQC.Counter.CriticalAreaReadError = 0u;
  me.SQC.Counter.CriticalAreaReadFail= 0u;
  me.SQC.Counter.CriticalAreaWrite= 0u;
  me.SQC.Counter.CriticalAreaWriteFail= 0u;
  me.SQC.Counter.NormalAreaWrite= 0u;
  me.SQC.Counter.NormalAreaWriteFail= 0u;
}

/**
 * @brief build LogicBlock_Table  创建逻辑块列表
* @details Archiver use a static organisation of memories block : \n
 * [ --------------------------------------Critical Area-----------------------------------------]      [--------Data area------------]\n
 *  critical block N, copy of critical block N,critical block N+1 , copy of critical block N+1............Data Block D,Data block D+1...
 */
static void Archiver_AllocatePhysicalBlock()
{
  TU8 PhysicalBlockID = 0U;
  TU8 LogicalBlock    = 0U;
  
  for(LogicalBlock = 0U; LogicalBlock < CRITICAL_BLOCK_CNT;++LogicalBlock)//CRITICAL_BLOCK_CNT  = 1
  {
    me.LogicBlock_Table[LogicalBlock].FreeSpaceleft = FLASH_BLOCK_SIZE - CRC_LENGHT;
    me.LogicBlock_Table[LogicalBlock].BlockRelativeFreePosition = 0U;
    me.LogicBlock_Table[LogicalBlock].NumberOffObjectRegisteredInThisBlock = 0U;
    me.LogicBlock_Table[LogicalBlock].BlockType = CriticalBlock;          //关键数据块
    me.LogicBlock_Table[LogicalBlock].PhysicalBlockID = PhysicalBlockID;
    PhysicalBlockID += (1 + CRITICAL_BLOCK_COPY);
  }
  for(LogicalBlock = CRITICAL_BLOCK_CNT; LogicalBlock < LOGIC_BLOCK_CNT;++LogicalBlock) //CRITICAL_BLOCK_CNT  = 1   LOGIC_BLOCK_CNT = 5
  {
    me.LogicBlock_Table[LogicalBlock].FreeSpaceleft = FLASH_BLOCK_SIZE - CRC_LENGHT; //128-2=126
    me.LogicBlock_Table[LogicalBlock].BlockRelativeFreePosition = 0U;
    me.LogicBlock_Table[LogicalBlock].NumberOffObjectRegisteredInThisBlock = 0U;
    me.LogicBlock_Table[LogicalBlock].BlockType = DataBlock;                         //普通数据块
    me.LogicBlock_Table[LogicalBlock].PhysicalBlockID = PhysicalBlockID;
    PhysicalBlockID += 1;
  }
  me.SQC.UsableSpace = (LOGIC_BLOCK_CNT * (FLASH_BLOCK_SIZE - CRC_LENGHT));//5*126
}

/**
 * @brief Setup STM8 Eeprom Hardware   ok sage
 */
static void Archiver_SetupHardware()
{

  /* Define FLASH programming time */
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
  
  /* Unlock Data memory */
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
}


/**
 * @brief Register an object in the Archiver object
 * @details Any object managed by archiver must be registered. An object registered can ben save in and restore from eeprom.
 * @param obj Object that must be registered
 * @param pf_SetToFactory Function pointer that must be called to reset object to factory value.
 * @param ObjectSize Object total size in Byte
 * @param IsCritical True if the object must be reister in the critical area. FALSE else.
 * @return True if oject is successfully registered. false else. 
 */
TBool Archiver_RegisterObject( void *obj,void (*pf_SetToFactory)(void),TU8 ObjectSize,TBool IsCritical)
{
  TS8 FreeBlock = Archiver_SearchBlockWithEnouthSpace(ObjectSize,IsCritical);
  TBool Rtn = FALSE;
  if(FreeBlock != -1)
  {    
    me.RegisteredObject[me.RegisteredObject_Ctr].Object_Size = ObjectSize;           //目标数据大小
    me.RegisteredObject[me.RegisteredObject_Ctr].pf_SetToFactory  = pf_SetToFactory; //安全参数值 出厂参数
    me.RegisteredObject[me.RegisteredObject_Ctr].pt_OnObjectInRam = obj;             //存储目标
    me.RegisteredObject[me.RegisteredObject_Ctr].Object_Eeprom_RelativeBlockStartAddress = me.LogicBlock_Table[FreeBlock].BlockRelativeFreePosition;
    me.RegisteredObject[me.RegisteredObject_Ctr].Object_ParentLogicBlock = (TU8) FreeBlock;
    ++me.RegisteredObject_Ctr;             //记录目标增加
    me.SQC.SpaceUsedInByte += ObjectSize;  //已使用空间
    
    me.LogicBlock_Table[FreeBlock].BlockRelativeFreePosition += ObjectSize;//位置
    me.LogicBlock_Table[FreeBlock].FreeSpaceleft -= ObjectSize;            //剩余空间
    me.LogicBlock_Table[FreeBlock].NumberOffObjectRegisteredInThisBlock += 1;

    Rtn = TRUE;
  }

  return Rtn;
}

/**
 * @brief Search a block with enouth space.
 * @param ObjectSizeInByte Object size in byte.
 * @param IsObjectCritical Trus if the object is critical and must be registered in critical area
 * @return return -1 if there is not enouth space, else return the logicblock index.
 */
static TS8 Archiver_SearchBlockWithEnouthSpace(TU8 ObjectSizeInByte,TBool IsObjectCritical)
{
  TU8 Start,Stop;
  TS8 Rtn = -1;
  if(IsObjectCritical == TRUE)//是关键数据区域
  {
    Start = 0U;
    Stop = CRITICAL_BLOCK_CNT - 1;
  }else                       //是普通数据区域
  {
    Start = CRITICAL_BLOCK_CNT;
    Stop = LOGIC_BLOCK_CNT - 1;
  }
  
  for(TU8 i=Start;i<=Stop && Rtn == -1 ;++i)
  {
    if(Archiver.LogicBlock_Table[i].FreeSpaceleft >= ObjectSizeInByte)
    {
      Rtn = (TS8)i;
    }
  }
  return Rtn;
}

/**
 * @brief Test if data have ben corrupted
 * @details The function compare the CRC16 store at the end of the physical block to a crc re-compute on data of the block.
 * @param PhysicalBlock Physical block to test.
 * @return return True if the block is corrupt. False else.
 */
static TBool Archiver_IsBlockCorrupt(TU8 PhysicalBlock)
{
  return (Archiver_ComputeBlockCRC(PhysicalBlock) == Archiver_GetBlockCRC(PhysicalBlock)) ? (FALSE) : (TRUE);
}

/**
 * @brief Get the CRC16 store at the end of the block
 * @param PhysicalBlock to read
 * @return The Block CRC
 */
static TU16 Archiver_ComputeBlockCRC(TU8 PhysicalBlock)
{
  TU16 CRC = 0u;
  CRC_Init(&CRC);
  TU32 StartArea = Bloc_AStart_Adress_Table[PhysicalBlock];
  TU8 PointerAttr * Pt = &(*((TU8 PointerAttr *)((MemoryAddressCast)StartArea)));
  CRC_ComputeArray(&CRC,(TU8*)Pt,FLASH_BLOCK_SIZE-CRC_LENGHT);
  return CRC;
}

/**
 * @brief Read a block, and compute it CRC16
 * @param PhysicalBlock Physical block to test.
 * @return computed CRC of the block
 */
static TU16 Archiver_GetBlockCRC(TU8 PhysicalBlock)
{
  return Archiver_ReadTU16(Bloc_AStop_Adress_Table[PhysicalBlock] - (CRC_LENGHT - 1));
}

/**
 * @brief Read a TU8 in eeprom
 * @param Address Physical address to read
 * @return The byte at Address
 */
//static TU8 Archiver_ReadTU8(TU32 Address)
//{
//  return *((TU8 PointerAttr *)((MemoryAddressCast)Address));
//}

/**
 * @brief Read a TU16 in eeprom
 * @param Address Physical address to read
 * @return The TU16 at Address
 */
static TU16 Archiver_ReadTU16(TU32 Address)
{
  return *((TU16 PointerAttr *)((MemoryAddressCast)Address));
}

/**
 * @brief Read a TU32 in eeprom
 * @param Address Physical address to read
 * @return The TU32 at Address
 */
//static TU32 Archiver_ReadTU32(TU32 Address)
//{
//  return *((TU32 PointerAttr *)((MemoryAddressCast)Address));
//}

/**
 * @brief Save all (critical or not) registered object
 * @details It's a buffered write. Object of one block are copied in a writebuffer. Then CRC is compute and append to the buffer. Then the entiere buffer is wrote in eeprom
 * CRC is check after write. <b> Write until success write</b>
*/
void Archiver_SaveAllObject(void)
{
  TBool Error = FALSE; 

  Archiver_SaveOnlyCriticalObject(); 
  
  //For each Logical block
  for(TU8 Logical = CRITICAL_BLOCK_CNT; Logical < LOGIC_BLOCK_CNT; ++Logical)//1   5
  {
    //Copy all object registered in the current block to the writebuffer. Skip if nothing in this block
    if(Archiver_LoadWriteBuffer(Logical,me.WriteBuffer) == TRUE)
    {
      //Compute and append CRC16 to write buffer
      TU16 CRC = Archiver_ComputeAndAppendCRC16ToBuffer(me.WriteBuffer,FLASH_BLOCK_SIZE);

      //get the  physical block id of the logical block
      TU8 PhysicalBlockID = me.LogicBlock_Table[Logical].PhysicalBlockID;
      //Then write the buffer in eeprom
      if(Archiver_WriteBufferToPhysicalBlock(me.WriteBuffer,PhysicalBlockID)== FALSE)
      {
        Error = TRUE;
      }
    }   
  }

  if(Error == TRUE)
  {//普通区域写失败
    SQC_Count(&me.SQC.Counter.NormalAreaWriteFail,sizeof(me.SQC.Counter.NormalAreaWriteFail));
  }
  else
  {//普通区域写正常
    SQC_Count(&me.SQC.Counter.NormalAreaWrite,sizeof(me.SQC.Counter.NormalAreaWrite));
  }
}


/**
 * @brief Save only critical object.  只存储关键数据
 * @details It's a buffered write. Object of one block are copied in a writebuffer. 
 * Then CRC is compute and append to the buffer. Then the entiere buffer is wrote in eeprom.
 * CRC is check after write. All required copy are write, the a new logical block is load. 
 * <b> Write of each block until success </b>
*/
void Archiver_SaveOnlyCriticalObject(void)
{

  TBool Error = FALSE; 
  //For each LogicalCritical block
  for(TU8 LogicalCritical = 0U; LogicalCritical < CRITICAL_BLOCK_CNT; ++LogicalCritical)
  {
    //Copy all object registered in the current critical block to the writebuffer. Skip if nothing in this block

    if(Archiver_LoadWriteBuffer(LogicalCritical,me.WriteBuffer) == TRUE)
    {
      //Compute and append CRC16 to write buffer
      TU16 CRC = Archiver_ComputeAndAppendCRC16ToBuffer(me.WriteBuffer,FLASH_BLOCK_SIZE);

      //get the first physical id of the logical critical block
      TU8 PhysicalBlockID = me.LogicBlock_Table[LogicalCritical].PhysicalBlockID;
      //Then write the buffer in eeprom, and all it copy
      for(TU8 Copy = 0U; Copy <= CRITICAL_BLOCK_COPY; ++Copy)
      {
        if(Archiver_WriteBufferToPhysicalBlock(me.WriteBuffer,PhysicalBlockID + Copy) == FALSE)
        {

          Error = TRUE;
        }
        else
        {
//          DEBUGPRINT("Write Success\n")
        }
      }
    }
  }
  
  if(Error == TRUE)
  {//写失败 置标志
    SQC_Count(&me.SQC.Counter.CriticalAreaWriteFail,sizeof(me.SQC.Counter.CriticalAreaWriteFail));
  }
  else
  {//写成功 置标志
    SQC_Count(&me.SQC.Counter.CriticalAreaWrite,sizeof(me.SQC.Counter.CriticalAreaWrite));
  }

}




/**
 * @param LogicalBlock The block to prepare to be write (all registered object of this block will be loaded in write buffer)
 * @param Buffer Temporary buffer to fill
 * @return FALSE if nothing to write, TRUE if there is data to write
*/
static TBool Archiver_LoadWriteBuffer(TU8 LogicalBlock,TU8* Buffer)
{
  TBool Rtn = FALSE;
  if(me.LogicBlock_Table[LogicalBlock].NumberOffObjectRegisteredInThisBlock != 0)
  {
     Rtn = TRUE;
    //PositionInBuffer represent the current position in the write buffer
    TU8 PositionInBuffer = 0U;
    
    //Clean the buffer
    memset(Buffer,0x00,FLASH_BLOCK_SIZE);   
    
    //Loop throw all registered object to find object registered in this block
    for(TU8 i = 0u;i<me.RegisteredObject_Ctr;++i)
    {
      //when object is registered in the block
      if(me.RegisteredObject[i].Object_ParentLogicBlock == LogicalBlock)
      {
        //Copy it to buffer
        void *Destination = (Buffer + PositionInBuffer);
        memcpy(Destination,me.RegisteredObject[i].pt_OnObjectInRam,me.RegisteredObject[i].Object_Size);
        PositionInBuffer += me.RegisteredObject[i].Object_Size;
      }
    }
  }
  
  return Rtn;
}


/**
 * @brief save Buffer into block. <b> The function loop until successfull write </b>
 * @param Buffer Pointer to eeprom block size buffer to write
 * @param PhysicalBlock Physical eeprom Block to write in
*/
static TBool Archiver_WriteBufferToPhysicalBlock(TU8* Buffer,TU8 PhysicalBlock)
{
  TU8 attempts = 0U;
  do
  {
    FLASH_ProgramBlock(PhysicalBlock,FLASH_MEMTYPE_DATA,FLASH_PROGRAMMODE_STANDARD,Buffer);
    FLASH_WaitForLastOperation(FLASH_MEMTYPE_DATA);
    attempts++;
  }while(Archiver_IsBlockCorrupt(PhysicalBlock) == TRUE && attempts < 5u);
  
  return (attempts < 5u) ? (TRUE) : (FALSE);
}

/**
 * @brief Compute and append CRC16 of the buffer
 * @param Buffer Buffer to compute CRC of
 * @param size Size of the Buffer
 * @return CRC16 append at the end of the buffer
 * @note Data on the two last room of the buffer are overewrite without any verification.
*/
static TU16 Archiver_ComputeAndAppendCRC16ToBuffer(TU8 *Buffer,TU16 size)
{
  TU16 CRC=0;
  CRC_Init(&CRC);
  CRC_ComputeArray(&CRC,Buffer,size-CRC_LENGHT);
  Buffer[size-2] = GET_MSBYTE(CRC);
  Buffer[size-1] = GET_LSBYTE(CRC);
  return CRC;
}


/**
 * @brief Load an object in the specified block
 * @param SourcePhysicalBlockId Physical eeprom block to read from
 * @param ObjectID Object number to read in SourcePhysicalBlockId
 * @note <b> SourcePhysicalBlockId is not check by the function. coherance of ObjectID and SourcePhysicalBlockId is not check </b>
*/
static void Archiver_LoadObject(TU8 SourcePhysicalBlockId,TU8 ObjectID)
{
  void *Destination = me.RegisteredObject[ObjectID].pt_OnObjectInRam;
  const void* Source = (void*)(Bloc_AStart_Adress_Table[SourcePhysicalBlockId] + me.RegisteredObject[ObjectID].Object_Eeprom_RelativeBlockStartAddress);
  TU8 size = me.RegisteredObject[ObjectID].Object_Size;
  memcpy(Destination,Source,size);
}

/**
 * @brief Parse Physical block of the specified logical critical block and return the first non corrupt block ID 
 * @param LogicalCriticalBlock Logical block to parse
 * @return The first valid physical block id. If all block are corrupt, return -1
*/
static TS8 Archiver_GetFirstValidPhysicalBlockID(TU8 LogicalCriticalBlock)
{
  TU8 PhysicalBlockID = me.LogicBlock_Table[LogicalCriticalBlock].PhysicalBlockID;
  TS8 Rtn = -1;
  for(TU8 i = PhysicalBlockID; i <= (PhysicalBlockID + CRITICAL_BLOCK_COPY) && Rtn == -1 ;++i)
  {
    TBool Corrupt = Archiver_IsBlockCorrupt(i);
    TBool Virgin  = Archiver_IsBlockVirgin(i);
    
    if(Corrupt == FALSE && Virgin == FALSE)
    {
      Rtn = (TS8) i;
    }
    else if(Corrupt == TRUE)
    {//关键区域读取错误
      SQC_Count(&me.SQC.Counter.CriticalAreaReadError,sizeof(me.SQC.Counter.CriticalAreaReadError));
    }
  }  //关键区域读取失败
  SQC_Count(&me.SQC.Counter.CriticalAreaReadFail,sizeof(me.SQC.Counter.CriticalAreaReadFail));
  return Rtn;
}


/**
 * @brief Check if a Block is virgin. On STM8, eeprom initial state is fill with 0.
 * @param Block Physical eeprom block to test 
 * @return TRUE if the block is virgin (filled with 0)
*/
static TBool Archiver_IsBlockVirgin(TU8 Block)
{
  TU32 StartArea = Bloc_AStart_Adress_Table[Block];
  TU32 EndOfArea = Bloc_AStop_Adress_Table[Block];
  
  TU8 PointerAttr * Pt    = &(*((TU8 PointerAttr *)((MemoryAddressCast)StartArea)));
  TU8 PointerAttr * Ptend = &(*((TU8 PointerAttr *)((MemoryAddressCast)EndOfArea)));
  
  while(*Pt == 0u && Pt < Ptend)
  {
    ++Pt;
  }
    
  return ( Pt == Ptend) ? (TRUE) : (FALSE);
}

/**
 * @brief Check if the critical area is virgin
 * @return TRUE if all block of the critical area are virgin
*/
static TBool Archiver_IsAllVirgin()
{
  TBool Rtn = TRUE;
  for(TU8 i = 0u ; i < FLASH_DATA_BLOCKS_COUNT && Rtn == TRUE; ++i)
  {
    if(Archiver_IsBlockVirgin(i) == FALSE)
    {
      Rtn = FALSE;
    }
  }
  return Rtn;
}


/**
 * @brief Set all registered object to their factory value
 * @details This funtion parse all registered object and call the pf_SetToFactory() callback function.
*/
 //设置所有记录目标为出厂值
 void Archiver_SetAllToFactory(void)
{
  for(TU8 i = 0U; i < me.RegisteredObject_Ctr;++i)
  {
    me.RegisteredObject[i].pf_SetToFactory();
  }
}



void Archiver_LoadMe(void *Me)
{
  //Search for the object in RegisteredObject array
  TS8 ObjectNumber = Archiver_SearchRegisteredObject(Me);
  if(ObjectNumber != -1)
  {
    //Object found. Check if it's a critical Object or a normal object, load procedure is slightly different  
    TU8 LogicalBlockSource = me.RegisteredObject[ObjectNumber].Object_ParentLogicBlock;
    if(me.LogicBlock_Table[LogicalBlockSource].BlockType == DataBlock) //如果是普通数据
    {
      //Check if data are corrupt. If they are, set factory value
      if(Archiver_IsBlockCorrupt(me.LogicBlock_Table[LogicalBlockSource].PhysicalBlockID) == TRUE)
      {
        me.RegisteredObject[ObjectNumber].pf_SetToFactory();
      }
      else
      {
        //Block is not corrupt, you can load data
        Archiver_LoadObject(me.LogicBlock_Table[LogicalBlockSource].PhysicalBlockID,ObjectNumber);
      }
    }
    else
    {//如果是关键数据
       TS8 ValidPhysicalBlock = Archiver_GetFirstValidPhysicalBlockID(LogicalBlockSource);
       if(ValidPhysicalBlock == -1)
       {
         //The Complete Critical area is corrupt !!!! Set to factory
          me.RegisteredObject[ObjectNumber].pf_SetToFactory();
       }
       else
       {
          Archiver_LoadObject(ValidPhysicalBlock,ObjectNumber);
       }
    }
  }
  else
  {
    //You have forget to register this object in archiver...
    trap();
  }
}
static TS8 Archiver_SearchRegisteredObject(const void* Me)
{
  TS8 Rtn = -1;
  for(TU8 i = 0u; ( (i < me.RegisteredObject_Ctr) && (Rtn == -1) ) ;++i)
  {
    //Check in registered object list if the object is know
    if(Archiver.RegisteredObject[i].pt_OnObjectInRam == Me)
    {
      Rtn = (TS8) i;
    }
  }
  return Rtn;
}

TBool Motor_Parameter_BackUp(TU32 Address, TU8 Me)
{
   TBool Rtn = FALSE;
   
    /*Define FLASH programming time*/
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
    /* Unlock Data memory */
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    FLASH_ProgramByte(Address, Me);

    /* Check program word action */
    if(Me == FLASH_ReadByte(Address))
      Rtn= TRUE;
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    return Rtn;    
}

TU8 Motor_Parameter_Read(TU32 Address)
{
  TU8 Me=0;
  Me = FLASH_ReadByte(Address);
  return Me; 
}
