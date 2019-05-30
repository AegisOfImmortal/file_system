# ECS 150 Project 4 Report 
### File system 

<p> Author: Sze Yuen Fung, Haoran Ding</p> 
<p> SID: 915016590, 913203198 </p> 
<hr>

##Introduction 
For this project, we are trying to implement the support of a very simple file system, called the ECS150-FS, which  is based on a FAT(File Allocation Table) and supports up to 128 files in a single root directory.  There are four consecutive logical parts of the layout of ECS150-FS , which are *Superblock*, *File Allocation Table* (FAT), *Root directory* and *Data blocks*. The size of virtual disk blocks is 4096 bytes. 

 To describe the data structures correctly in this project, we use the integer types defined in `stdint.h`
- struct SuperBlock
	- To describe the data structures correctly in this project, we use the integer types defined in `stdint.h`:
		- we use `uint16_t` for  `total_blocks_of_disk` , `number_of_data_blocks`and `data_block_index`.
		- We call `uint8_t` for `number_of_blocks_for_FAT` and `padding`. 
- struct FAT
	- we use  `uint16_t` for `entries`

- `fs_mount`to mount a file system
	- we mount the file system that the virtual disk file @diskname contains. We need to mount the file system before files can be read or written. 
	- We return -1 if virtual disk file @diskname cannot be opened, or if no valid that the file system can be located. Otherwise, we return 0. 
- `fs_unmount`to unmount a file system
	- We unmount the currently touted file system and close the underlying virtual disk file. 
	- We return -1 if there is no underlying virtual disk was opened, or if there is a virtual disk that cannot be closed, or if there are still open file descriptors. Otherwise, we return 0. 
- `fs_info`  to display information about file system
	- We use it to display information about the currently mounted file system.
	- We return -1 if there is no underlying virtual disk was opened. Otherwise, we return 0. 
- `fs_create` to  add a new file 
	- We want to create a new and empty file, called @filename , in the root directory of the mounted file system and fill it out with proper information. 
	- We return -1 if @filename is invalid, if there is a file named @filename already exists, or if the string of the @filename is too long, or if the root directory already contains `%FS_FILE_MAX_COUNT` files. Otherwise, we return 0. 
- `fs_delete` to  delete a file
	- We want to delete the named @filename from the root directory of the mounted file system.
	- We return -1 if @filename is invalid, if there is no file named @filename to delete, or if there is a file @filename is currently open. Otherwise, we return 0. 
- `fs_ls`to list files on the file system
	- We want to list  information about the files location and prints the listing of all the files in the file system. 
	- We return -1 if -1 if no underlying virtual disk was opened. Otherwise, we return 0. 



