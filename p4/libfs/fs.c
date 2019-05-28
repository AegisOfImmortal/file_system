#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

/* TODO: Phase 1 */
#define FAT_EOC 0xFFFF
#define Root_Directory_Size 32
#define SIGNATURE "ECS150FS"
#define NULL_STRING "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
//The size of virtual disk blocks is 4096 bytes
//SuperBlock
struct SuperBlockInfo
{
	char signature[8];

	uint8_t total_blocks_of_disk;
    uint8_t root_directory_block_index;
    uint8_t data_block_index;
    uint8_t number_of_data_blocks;
    uint4_t number_of_blocks_for_FAT;
}；

//Root directory
struct FAT
{
	char filename[16];
	uint16_t size;
	unit8_t index_of_first_data_block;
};

struct SuperBlockInfo superBlockInfo;


int fs_mount(const char *diskname)
{
	
	/* TODO: Phase 1 */
	//open the disk 
	//check @diskname is invalid or not
	//check if virtual disk file can be opened or not
	if(block_disk_open(diskname) == -1){
		return -1;
	}
	return 0;
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
	//check if there is virtual disk file still opens
	if(check_opening() != -1){
		return -1;
	}
	//check if virtual disk file is closed
	if(block_disk_close() == -1) {
		return -1;
	}
	return 0;
}

int fs_info(void)
{
	/* TODO: Phase 1 */
	//check the number of total blocks 
	if(superBlockInfo.total_blocks_of_disk != block_disk_count()){
		return -1;
	}

	//print information about the mounted file system
	printf("FS Info:\n");
	printf("total_blk_count=%d\n",(int)superBlockInfo.total_blocks_of_disk);
	printf("fat_blk_count=%d\n",(int)superBlockInfo.number_of_block_for_FAT);
	printf("rdirblk_=%d\n",(int)superBlockInfo.root_directory_block_index);
	printf("data_blk=%d\n",(int)superBlockInfo.data_block_index);
	printf("data_blk_count=%d\n",(int)superBlockInfo.number_of_data_blocks);
	printf("fat_free_ratio=%d/%d\n",count_empty_FAT(), (int)superBlockInfo.number_of_data_blocks);
	return 0;
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

