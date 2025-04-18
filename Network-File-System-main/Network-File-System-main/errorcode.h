#ifndef ERRORCODE_H
#define ERRORCODE_H

#define SUCCESS 0
#define CREATE_ERROR 101 // Error in Creating New File/Directory
#define PERMISSION_DENIED 102 // Permission Denied
#define INVALID_PATH 103 // Given path is invalid
#define INVALID_DIRNAME 104 // Given directory name is invalid
#define DIR_ALREADY_EXISTS 106 // Directory already exists
#define FILE_ALREADY_EXISTS 107 // File already exists
#define FILE_NOT_FOUND 108 // File not found
#define DIR_NOT_FOUND 109 // Directory not found
#define DELETE_ERROR 110 // Error in deleting file/directory
#define SRC_NOT_FOUND 111 // Source file/directory not found
#define DEST_NOT_FOUND 112 // Destination file/directory not found
#define COPY_ERROR 113 // Error in copying file/directory
#define SRC_IS_DIR 114 // Source is a directory
#define DEST_IS_DIR 115 // Destination is a directory
#define SRC_IS_FILE 116 // Source is a file
#define DEST_IS_FILE 117 // Destination is a file
#define SS_DOWN 118 // Storage Server is down

#endif