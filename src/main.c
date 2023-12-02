#include <gst/gst.h>
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 16384  // Tamaño del búfer para descomprimir

// Declaración de la función de uso
void usage();

int main(int argc, char* argv[]) {

    const char* gz_file = NULL;
    const char* output_file_name = NULL;

    /* Handle program arguments */
    if (argc < 4) {
        fprintf(stderr, "Error: not enough program arguments\n");
        usage();
        return EXIT_FAILURE;
    }
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-orig") == 0) {
            gz_file = argv[i + 1];
            printf("Stream file (.gz) found!: %s\n", gz_file);
        }
        else if (strcmp(argv[i], "-out") == 0) {
            output_file_name = argv[i + 1];
        }
        else if (strcmp(argv[i], "-help") == 0){
            usage();
            return EXIT_SUCCESS;
        }
        else {
            fprintf(stderr, "Invalid option: %s\n", argv[i]);
            usage();
            return EXIT_FAILURE;
        }
    }   

    /* Decompress stream */
    char* decompressed_data;
    size_t data_size;
    if (decompressing_buffer(gz_file, &decompressed_data, &data_size) != EXIT_SUCCESS) {
        fprintf(stderr, "Error at decompressing file.\n");
        return EXIT_FAILURE;
    }
    else {
        printf("Stream successfully descompressed! \n");
    }

    /* GStreamer pipeline */
    if (plugin_init(decompressed_data, data_size, output_file_name) != 0) {
        fprintf(stderr, "Error with GStreamer pipeline.\n");
        free(decompressed_data);
        return EXIT_FAILURE;
    }
    else {
        printf("Pipeline succesfully run! \n");
    }

    /* Free memory */
    free(decompressed_data);

    return EXIT_SUCCESS;
}

/**
* @brief Displays usage information for the program.
* 
* @return: no returns
*
*/
void usage() {
    printf("Usage: program -orig <path-to-source-file.gz> -out <path-to-output-file.txt>\n");
    printf("Show help window: -help\n");
}
