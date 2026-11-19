/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"		/* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "sd_driver.h"
#include "spi.h"
#include "rtc.h" 

/* Definitions of physical drive number for each drive */
// #define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
// #define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
// #define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_SD 0

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv)
	{
	case DEV_SD:
		return FR_OK;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	switch (pdrv)
	{
	case DEV_SD:
		Spi1_BusLock(); /* SD 与 W25QXX 共用 SPI1，整笔事务加总线锁 */
		stat = SD_Initialize();
		Spi1_BusUnlock();
		// translate the reslut code here
		return stat;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv,	  /* Physical drive nmuber to identify the drive */
	BYTE *buff,	  /* Data buffer to store read data */
	LBA_t sector, /* Start sector in LBA */
	UINT count	  /* Number of sectors to read */
)
{
	DRESULT res;

	switch (pdrv)
	{
	case DEV_SD:
		// translate the arguments here
		Spi1_BusLock(); /* SD 与 W25QXX 共用 SPI1，整笔事务加总线锁 */
		res = (DRESULT)SD_ReadDisk(buff, sector, count);
		Spi1_BusUnlock();
		// translate the reslut code here
		return res;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
	BYTE pdrv,		  /* Physical drive nmuber to identify the drive */
	const BYTE *buff, /* Data to be written */
	LBA_t sector,	  /* Start sector in LBA */
	UINT count		  /* Number of sectors to write */
)
{
	DRESULT res;

	switch (pdrv)
	{
	case DEV_SD:
		// translate the arguments here
		Spi1_BusLock(); /* SD 与 W25QXX 共用 SPI1，整笔事务加总线锁 */
		res = (DRESULT)SD_WriteDisk((unsigned char *)buff, sector, count);
		Spi1_BusUnlock();
		// translate the reslut code here
		return res;
	}

	return RES_PARERR;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	BYTE cmd,  /* Control code */
	void *buff /* Buffer to send/receive control data */
)
{
	switch (pdrv)
	{
	case DEV_SD:
		switch (cmd)
		{
		case CTRL_SYNC:
			break;
		case GET_SECTOR_COUNT:
			Spi1_BusLock(); /* 读 CSD 需要发 SD 命令，同样要占 SPI1 总线 */
			*(unsigned int *)buff = SD_GetSectorCount();
			Spi1_BusUnlock();
			break;
		case GET_SECTOR_SIZE:
			*(unsigned int *)buff = 512;
			break;
		case GET_BLOCK_SIZE:
			break;
		case CTRL_TRIM:
			break;
		}
		return RES_OK;
	}

	return RES_PARERR;
}

DWORD get_fattime(void)
{
	    /* RTC_WKUP中断每秒刷新一次RTC_TimeStruct/RTC_DateStruct */
    return ((DWORD)(RTC_DateStruct.RTC_Year + 20) << 25) | /* RTC_Year是从2000年起的年数，+20换算成从1980年起 */
           ((DWORD)RTC_DateStruct.RTC_Month << 21) |
           ((DWORD)RTC_DateStruct.RTC_Date << 16) |
           ((DWORD)RTC_TimeStruct.RTC_Hours << 11) |
           ((DWORD)RTC_TimeStruct.RTC_Minutes << 5) |
           ((DWORD)(RTC_TimeStruct.RTC_Seconds >> 1));
}
