#ifndef GZDEC_H
#define GZDEC_H

/*INCLUDES*/
#include <gst/gst.h>
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*DEFINES*/
#define CHUNK_SIZE 16384  // Buffer size for decompressing

/*FUNCTIONS*/

int decompressing_buffer(const char* gz_file, char** decompressed_data, size_t* data_size);

int plugin_init(const char* decompressed_data, size_t data_size, const char* fileName);

#endif // GZDEC_H
#pragma once
