/*
 * File:	flash_config.s
 */

	.global FLASH_CONFIG
	.global _FLASH_CONFIG	
	.text

/*
 * FLASH configuration
 */
FLASH_CONFIG:
_FLASH_CONFIG:
.long 0x00000000 
.long 0x00000000
.long 0x00000000
.long 0x00000000
.long 0x00000000
.long 0x00000000
.end
