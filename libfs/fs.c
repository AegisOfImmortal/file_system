#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

//Global Variables
int isMounted = 0;
int RootFreeEntry = 0;
int FATFreeEntry = 0;
#define FAT_EOC 0xFFFF
#define Root_Directory_Size 32
#define SIGNATURE "ECS150FS"
#define NULL_STRING "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
#define INT2VOIDP(i) (void*)(uintptr_t)(i)
//The size of virtual disk blocks is 4096 bytes
//SuperBlock
struct SuperBlock
{
    char signature[8];
    uint16_t total_blocks_of_disk;
    uint16_t root_directory_block_index;
    uint16_t data_block_index;
    uint16_t number_of_data_blocks;
    uint8_t number_of_blocks_for_FAT;
    uint8_t padding[4079];
};
typedef struct SuperBlock* SuperBlock_t;

//FAT
struct FAT
{
    uint16_t * entries;
};

//Root directory Entry
struct Entry
{
    char filename[FS_FILENAME_LEN];
    uint32_t file_size;
    uint16_t index_of_first_data_block;
    char padding[10];
};
typedef struct Entry* Entry_t;

//File Descriptor
struct FileDescriptor
{
    int fileID;
    int offset;
    char * filename;
};

//Global Structs
struct SuperBlock SBbuff;
SuperBlock_t superblock;
struct Entry RootEntry[FS_FILE_MAX_COUNT];
struct FileDescriptor FDarr[FS_OPEN_MAX_COUNT];
struct FAT FAT;


SuperBlock_t SuperblockInit() {
    memset(SBbuff.signature, 0, 8);
    SBbuff.total_blocks_of_disk = 0;
    SBbuff.root_directory_block_index = 0;
    SBbuff.data_block_index = 0;
    SBbuff.number_of_data_blocks = 0;
    SBbuff.number_of_blocks_for_FAT = 0;
    memset(SBbuff.padding, 0, 4079);
    return &SBbuff;
}

int fs_mount(const char *diskname)
{
    //open the disk, check if can be opened
    if(block_disk_open(diskname) == -1 || diskname == NULL || isMounted == 1){
        return -1;
    }
    
    //Read the superblock
    superblock = SuperblockInit();
    block_read(0, (void*)superblock);
    
    //the signature of the file system should correspond to the one defined by the specifications
    if(memcmp(superblock->signature, SIGNATURE, 8) != 0){
        return -1;
    }
    
    //Read the fatblock
    FAT.entries = (uint16_t*)malloc(superblock->number_of_data_blocks*sizeof(uint16_t));
    for (int i = 0; i < superblock->root_directory_block_index; i++) {
        block_read(i, ((void*)FAT.entries) + BLOCK_SIZE*i);
    }
    block_read(superblock->root_directory_block_index, INT2VOIDP(RootFreeEntry));
    
    //Check free entries
    for (int i = 0; i < superblock->number_of_data_blocks; i++) {
        if(FAT.entries[i] == 0) {
            FATFreeEntry++;
        }
    }
    for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if(RootEntry[i].filename[0] == 0) {
            RootFreeEntry++;
        }
    }
    
    //Init File Descriptor Array
    for (int i = 0; i < FS_OPEN_MAX_COUNT; ++i)
    {
        FDarr[i].filename = (char*)malloc(FS_FILENAME_LEN*sizeof(char));
        FDarr[i].fileID = -1;
        FDarr[i].offset = -1;
    }
    
    isMounted = 1;
    return 0;
}

int fs_umount(void)
{
    block_write(superblock->root_directory_block_index, (void *)RootEntry);
    
    //Free File Descriptor Array
    for (int i = 0; i < FS_OPEN_MAX_COUNT; ++i)
    {
        free(FDarr[i].filename);
    }
    
    //check if virtual disk file is closed
    if(block_disk_close() == -1) {
        return -1;
    }
    
    isMounted = 0;
    return 0;
}

int fs_info(void)
{
    //check the number of total blocks
    if(superblock->total_blocks_of_disk != block_disk_count()){
        return -1;
    }
    
    //print information about the mounted file system
    printf("FS Info:\n");
    printf("total_blk_count=%d\n",(int)superblock->total_blocks_of_disk);
    printf("fat_blk_count=%d\n",(int)superblock->number_of_blocks_for_FAT);
    printf("rdirblk_=%d\n",(int)superblock->root_directory_block_index);
    printf("data_blk=%d\n",(int)superblock->data_block_index);
    printf("data_blk_count=%d\n",(int)superblock->number_of_data_blocks);
    printf("fat_free_ratio=%d/%d\n",FATFreeEntry, (int)superblock->number_of_data_blocks);
    printf("root_free_raio=%d/%d\n",RootFreeEntry, FS_FILE_MAX_COUNT );
    return 0;
}

int fs_create(const char *filename)
{
    // if @filename is invalid, if there is no file named @filename to delete, or if file @filename is currently open
    if (strlen(filename) > FS_FILENAME_LEN|| filename == NULL || isMounted == 0){
        return -1;
    }
    
    // if a file with the same name already exists on the FS
    for(int i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if(strcmp(RootEntry[i].filename, filename) != 0){
            return -1;
        }
    }
    
    // Create the file
    for(int i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if(RootEntry[i].filename[0] == 0) {
            strcpy(RootEntry[i].filename, filename);
            RootEntry[i].file_size = 0;
            RootEntry[i].index_of_first_data_block = FAT_EOC;
            block_write(superblock->root_directory_block_index, (void*) RootEntry);
            return 0;
        }
    }
    
    // File was not created
    return -1;
}

int fs_delete(const char *filename)
{
    // if @filename is invalid
    if (strlen(filename) > FS_FILENAME_LEN|| filename == NULL || isMounted == 0){
        return -1;
    }
    
    // Check if file is open
    for (int k = 0; k < FS_OPEN_MAX_COUNT; k++) {
        if(strcmp(FDarr[k].filename, filename) == 0)
            return -1;
    }
    //Delete the file
    for(int i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if(strcmp(RootEntry[i].filename, filename) == 0) {
            //Delete table entries
            uint16_t deleteIndex = RootEntry[i].index_of_first_data_block;
            while(FAT.entries[deleteIndex] != FAT_EOC)
            {
                uint16_t temp = FAT.entries[deleteIndex];
                FAT.entries[deleteIndex] = 0;
                FATFreeEntry++;
                deleteIndex = temp;
            }
            FAT.entries[deleteIndex] = 0;
            
            //Delete Root entries and flush directory
            RootEntry[i].filename[0] = 0;
            RootEntry[i].file_size = 0;
            RootEntry[i].index_of_first_data_block = FAT_EOC;
            block_write(superblock->root_directory_block_index, (void*)RootEntry);
            RootFreeEntry++;
            return 0;
        }
    }
    // Deletion Failed
    return -1;
}

int fs_ls(void)
{
    if(isMounted == 0){
        return -1;
    }
    printf("FS Ls:\n");
    for(int i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if(RootEntry[i].filename[0] != 0) {
            printf("file: %s, size: %d, data_blk: %d\n", RootEntry[i].filename, RootEntry[i].file_size, RootEntry[i].index_of_first_data_block);
        }
    }
    return 0;
}

int fs_open(const char *filename)
{
    /* TODO: Phase 3 */
    return 0;
}

int fs_close(int fd)
{
    /* TODO: Phase 3 */
    return 0;
}

int fs_stat(int fd)
{
    /* TODO: Phase 3 */
    return 0;
}

int fs_lseek(int fd, size_t offset)
{
    /* TODO: Phase 3 */
    return 0;
}

int fs_write(int fd, void *buf, size_t count)
{
    /* TODO: Phase 4 */
    return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
    /* TODO: Phase 4 */
    return 0;
}

