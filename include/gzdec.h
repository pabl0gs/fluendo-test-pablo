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

/**
 * @brief Read and decompress a gzip file
 *
 * @param gz_file: path to the gzip file to be decompressed.
 * @param decompressed_data: pointer to a char pointer where the decompressed data will be stored.
 * @param data_size: pointer to a size_t variable that will store the size of the decompressed data.
 *
 * @return:EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 *
 **/
int decompressing_buffer(const char* gz_file, char** decompressed_data, size_t* data_size);

/**
 * @brief Initializes and runs a GStreamer pipeline to save decompressed data to a file.
 *
 *
 * @param decompressed_data: pointer to the decompressed data to be saved.
 * @param data_size: size of the decompressed data.
 * @param fileName: path to the output file where the data will be saved.
 *
 * @return: '0' for success, '-1' for failure.
 *
 */
int plugin_init(const char* decompressed_data, size_t data_size, const char* fileName);

#endif // GZDEC_H
#pragma once
