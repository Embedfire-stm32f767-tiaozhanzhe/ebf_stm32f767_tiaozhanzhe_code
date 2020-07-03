/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "string.h"

#ifndef FATFS_FLASH_SPI
	#define FATFS_FLASH_SPI				1
#endif

#ifndef FATFS_USE_SDIO
	#define FATFS_USE_SDIO			1
#endif

/* Set in defines.h file if you want it */
#ifndef TM_FATFS_CUSTOM_FATTIME
	#define TM_FATFS_CUSTOM_FATTIME		0
#endif


/* Include SD card files if is enabled */
#if FATFS_USE_SDIO == 1
	#include "./sdmmc/bsp_sdmmc_sd.h"
#endif

#if FATFS_FLASH_SPI == 1
	#include "./flash/bsp_qspi_flash.h"
#endif


/* Definitions of physical drive number for each media */
#define ATA			    0
#define SPI_FLASH		1

DRESULT SD_read(BYTE *buff, DWORD sector, UINT count);
DRESULT SD_write(const BYTE *buff, DWORD sector, UINT count);
DRESULT TM_FATFS_FLASH_SPI_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT TM_FATFS_FLASH_SPI_disk_write(BYTE *buff, DWORD sector, UINT count);
DRESULT TM_FATFS_FLASH_SPI_disk_ioctl(BYTE cmd, char *buff);
DRESULT TM_FATFS_SD_SDIO_disk_ioctl(BYTE cmd, char *buff);
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{

	DSTATUS status = STA_NOINIT;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = BSP_SD_Init();	/* SDIO communication */
			#endif
			break;
		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
			status = QSPI_FLASH_Init();	/* SDIO communication */
			#endif
			break;

		default:
			status = STA_NOINIT;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{

	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = BSP_SD_GetStatus();	/* SDIO communication */
			#endif
			break;
		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
		{
			//QSPI_FLASH_ReadID();检查FLASH工作状态是否正常,可以通过读FLASHID判断
			status = 0; 	/* SDIO communication */
		}
			#endif
			break;

		default:
			status = STA_NOINIT;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = SD_read(buff, sector, count);	/* SDIO communication */
			#endif
			break;
		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
			status = TM_FATFS_FLASH_SPI_disk_read(buff, sector, count);	/* SDIO communication */
			#endif
		break;
		default:
			status = RES_PARERR;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DRESULT status = RES_PARERR;
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}
	
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = SD_write((BYTE *)buff, sector, count);	/* SDIO communication */
			#endif
		break;

		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
			status = TM_FATFS_FLASH_SPI_disk_write((BYTE *)buff, sector, count);	/* SDIO communication */
			#endif
		break;
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = TM_FATFS_SD_SDIO_disk_ioctl(cmd, buff);					/* SDIO communication */
			#endif
			break;
		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
			status = TM_FATFS_FLASH_SPI_disk_ioctl(cmd, buff);	/* SDIO communication */
			#endif
		break;
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif

__weak DWORD get_fattime(void) {
	/* Returns current time packed into a DWORD variable */
	return	  ((DWORD)(2013 - 1980) << 25)	/* Year 2013 */
			| ((DWORD)7 << 21)				/* Month 7 */
			| ((DWORD)28 << 16)				/* Mday 28 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				/* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}


/******************************实际的底层驱动接口，放在这方便查看和移植******************************/
DRESULT SD_read(BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_OK;
  
    if ((DWORD)buff & 3) 
    {
        DWORD scratch[512 / 4];

        while (count--) 
        {
            memcpy(scratch, buff, 512);
            res = SD_read((void *)scratch, sector++, 1);
            
            if (res != RES_OK)
            {
               break;
            }
            buff += 512;
        }

        return(res);
     }
  if(BSP_SD_ReadBlocks_DMA((uint32_t*)buff, 
                       (uint64_t) (sector * 512), 
                       512, 
                       count) != MSD_OK)
  {
    res = RES_ERROR;
  }
  
  return res;
}

DRESULT SD_write(const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_OK;
  
    if ((DWORD)buff & 3) 
    {
        DWORD scratch[512 / 4];

        while (count--) 
        {
            memcpy(scratch, buff, 512);
            res = SD_write((void *)scratch, sector++, 1);
            
            if (res != RES_OK)
            {
               break;
            }
            buff += 512;
        }

        return(res);
     }
    if(BSP_SD_WriteBlocks_DMA((uint32_t*)buff, 
                        (uint64_t)(sector * 512), 
                        512, count) != MSD_OK)
    {
        res = RES_ERROR;
    }
  
    return res;
}  
  
DRESULT TM_FATFS_FLASH_SPI_disk_read(BYTE *buff, DWORD sector, UINT count)
{
	sector+=4096;//扇区偏移，外部Flash文件系统空间放在外部Flash后面16M空间
	BSP_QSPI_Read(buff, sector <<12, count<<12);
	return RES_OK;
}

DRESULT TM_FATFS_FLASH_SPI_disk_write(BYTE *buff, DWORD sector, UINT count)
{
	uint32_t write_addr;  
	sector+=4096;//扇区偏移，外部Flash文件系统空间放在外部Flash后面16M空间
	write_addr = sector<<12;    
	BSP_QSPI_Erase_Block(write_addr);
	BSP_QSPI_Write(buff,write_addr,4096);
	return RES_OK;
}


extern SD_CardInfo      uSdCardInfo;

DRESULT TM_FATFS_SD_SDIO_disk_ioctl(BYTE cmd, char *buff)
{
	switch (cmd) 
	{
		case GET_SECTOR_SIZE :     // Get R/W sector size (WORD) 
			*(WORD * )buff = 512;
		break;
		case GET_BLOCK_SIZE :      // Get erase block size in unit of sector (DWORD)
			*(DWORD * )buff = uSdCardInfo.CardBlockSize;
		break;

		case GET_SECTOR_COUNT:
			*(DWORD * )buff = uSdCardInfo.CardCapacity/uSdCardInfo.CardBlockSize;
			break;
		case CTRL_SYNC :
		break;
	}
	return RES_OK;
}

DRESULT TM_FATFS_FLASH_SPI_disk_ioctl(BYTE cmd, char *buff)
{

	switch (cmd) {
		case GET_SECTOR_COUNT:
			*(DWORD * )buff = 4096;		//sector数量   
		break;
		case GET_SECTOR_SIZE :     // Get R/W sector size (WORD)

			*(WORD * )buff = 4096;		//flash最小写单元为页，256字节，此处取2页为一个读写单位
		break;
		case GET_BLOCK_SIZE :      // Get erase block size in unit of sector (DWORD)
			*(DWORD * )buff = 1;		//flash以1个sector为最小擦除单位
		break;
		case CTRL_ERASE_SECTOR:
		break;
		case CTRL_SYNC :
		break;
	}
	return RES_OK;
}
