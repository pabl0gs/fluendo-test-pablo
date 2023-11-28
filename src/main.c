#include <gst/gst.h>
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 16384  // Buffer size for descompressing

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Can't find .gz file\n");
        return EXIT_FAILURE;
    }
    const char* gz_file = argv[1];
    printf("Stream file (.gz) found!: %s\n", gz_file);

    // Open .gz file
    FILE* file = fopen(gz_file, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", gz_file);
        return EXIT_FAILURE;
    }

    // Initialize zlib structure for decompressing
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK) {
        fprintf(stderr, "Error initializing zlib for decompression\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Buffers for reading and writing
    unsigned char in_buffer[CHUNK_SIZE];
    unsigned char out_buffer[CHUNK_SIZE];

    // Config zlib struct for reading
    stream.next_in = in_buffer;
    stream.avail_in = 0;

    // Config zlib struct for writing 
    stream.next_out = out_buffer;
    stream.avail_out = CHUNK_SIZE;

    // Decompression loop
        int ret;
    do {
        // Read compressed data
        if (stream.avail_in == 0) {
            stream.avail_in = fread(in_buffer, 1, CHUNK_SIZE, file);
            if (stream.avail_in == 0 && feof(file)) {
                break;  // File-end
            }
            stream.next_in = in_buffer;
        }

        // Decompress data 
        ret = inflate(&stream, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END) {
            fprintf(stderr, "Error in decompression: %s\n", zError(ret));
            inflateEnd(&stream);
            fclose(file);
            return EXIT_FAILURE;
        }

        // Escribir datos descomprimidos en algún lugar (puedes modificar esto según tus necesidades)
        // Aquí, simplemente se escriben en la salida estándar
        //fwrite(out_buffer, 1, CHUNK_SIZE - stream.avail_out, stdout); //Print data

        // Write into the output folder
        stream.next_out = out_buffer;
        stream.avail_out = CHUNK_SIZE;
    } while (ret != Z_STREAM_END);


    // End decompression and close file
    inflateEnd(&stream);
    fclose(file);

    return EXIT_SUCCESS;
}
