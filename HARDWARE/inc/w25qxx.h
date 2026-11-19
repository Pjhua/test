#ifndef __25QXX_H
#define __25QXX_H

#include "io_bit.h"

#define W25QXX_CS PCout(7)

// W25QXX指令表
#define W25QXX_WriteEn 0x06     // 写使能
#define W25QXX_WriteDis 0x04    // 写失能
#define W25QXX_ReadSR 0x05      // 读寄存器
#define W25QXX_WriteSR 0x01     // 写寄存器
#define W25QXX_ReadData 0x03    // 读数据
#define W25QXX_PageProgram 0x02 // 页写
#define W25QXX_SectorErase 0x20 // 扇区擦除
#define W25QXX_BlockErase 0xD8  // 区块擦除
#define W25QXX_ChipErase 0xC7   // 芯片擦除
#define W25QXX_ReadId 0x90      // 读ID

uint16_t W25qxx_Init(void);
void W25qxx_WriteEnable(void);
void W25qxx_WriteDisable(void);
void W25qxx_WaitBusy(void);
unsigned char W25qxx_ReadStatusRegister(void);
void W25qxx_WriteStatusRegister(unsigned char data);
unsigned short W25qxx_ReadID(void);

void W25qxx_ReadDatas(unsigned char *buffer, unsigned int read_address, unsigned short num_data);
void W25qxx_WritePage(unsigned char *buffer, unsigned int write_addr, unsigned short num_data);

void W25qxx_EraseSector(unsigned int sector_addr); // 扇擦除
void W25qxx_EraseBlock(unsigned int block);        // 块擦除

void W25qxx_Usart1_DownloadFile(unsigned short sector, unsigned short n);
void W25qxx_fatfs_DownloadFile(unsigned short sector, unsigned short n, const char *filename);

#endif
