/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CBFS_H
#define __CBFS_H

#include <compiler.h>
#include <linux/compiler.h>

enum cbfs_result {
	CBFS_SUCCESS = 0,
	CBFS_NOT_INITIALIZED,
	CBFS_BAD_HEADER,
	CBFS_BAD_FILE,
	CBFS_FILE_NOT_FOUND
};

enum cbfs_filetype {
	CBFS_TYPE_STAGE = 0x10,
	CBFS_TYPE_PAYLOAD = 0x20,
	CBFS_TYPE_OPTIONROM = 0x30,
	CBFS_TYPE_BOOTSPLASH = 0x40,
	CBFS_TYPE_RAW = 0x50,
	CBFS_TYPE_VSA = 0x51,
	CBFS_TYPE_MBI = 0x52,
	CBFS_TYPE_MICROCODE = 0x53,
	CBFS_COMPONENT_CMOS_DEFAULT = 0xaa,
	CBFS_COMPONENT_CMOS_LAYOUT = 0x01aa
};

struct cbfs_header {
	u32 magic;
	u32 version;
	u32 rom_size;
	u32 boot_block_size;
	u32 align;
	u32 offset;
	u32 pad[2];
} __packed;

struct cbfs_fileheader {
	u8 magic[8];
	u32 len;
	u32 type;
	u32 checksum;
	u32 offset;
} __packed;

struct cbfs_cachenode {
	struct cbfs_cachenode *next;
	u32 type;
	void *data;
	u32 data_length;
	char *name;
	u32 name_length;
	u32 checksum;
} __packed;

extern enum cbfs_result file_cbfs_result;

/*
 * Return a string describing the most recent error condition.
 *
 * @return A pointer to the constant string.
 */
const char *file_cbfs_error(void);

/*
 * Initialize the CBFS driver and load metadata into RAM.
 *
 * @param end_of_rom	Points to the end of the ROM the CBFS should be read
 *                      from.
 */
void file_cbfs_init(uintptr_t end_of_rom);

/*
 * Get the header structure for the current CBFS.
 *
 * @return A pointer to the constant structure, or NULL if there is none.
 */
const struct cbfs_header *file_cbfs_get_header(void);

/*
 * Get a handle for the first file in CBFS.
 *
 * @return A handle for the first file in CBFS, NULL on error.
 */
const struct cbfs_cachenode *file_cbfs_get_first(void);

/*
 * Get a handle to the file after this one in CBFS.
 *
 * @param file		A pointer to the handle to advance.
 */
void file_cbfs_get_next(const struct cbfs_cachenode **file);

/*
 * Find a file with a particular name in CBFS.
 *
 * @param name		The name to search for.
 *
 * @return A handle to the file, or NULL on error.
 */
const struct cbfs_cachenode *file_cbfs_find(const char *name);


/***************************************************************************/
/* All of the functions below can be used without first initializing CBFS. */
/***************************************************************************/

/*
 * Find a file with a particular name in CBFS without using the heap.
 *
 * @param end_of_rom	Points to the end of the ROM the CBFS should be read
 *                      from.
 * @param name		The name to search for.
 *
 * @return A handle to the file, or NULL on error.
 */
const struct cbfs_cachenode *file_cbfs_find_uncached(uintptr_t end_of_rom,
						     const char *name);

/*
 * Get the name of a file in CBFS.
 *
 * @param file		The handle to the file.
 *
 * @return The name of the file, NULL on error.
 */
const char *file_cbfs_name(const struct cbfs_cachenode *file);

/*
 * Get the size of a file in CBFS.
 *
 * @param file		The handle to the file.
 *
 * @return The size of the file, zero on error.
 */
u32 file_cbfs_size(const struct cbfs_cachenode *file);

/*
 * Get the type of a file in CBFS.
 *
 * @param file		The handle to the file.
 *
 * @return The type of the file, zero on error.
 */
u32 file_cbfs_type(const struct cbfs_cachenode *file);

/*
 * Read a file from CBFS into RAM
 *
 * @param file		A handle to the file to read.
 * @param buffer	Where to read it into memory.
 *
 * @return If positive or zero, the number of characters read. If negative, an
 *         error occurred.
 */
long file_cbfs_read(const struct cbfs_cachenode *file, void *buffer,
		    unsigned long maxsize);

#endif /* __CBFS_H */
