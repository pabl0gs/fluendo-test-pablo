#include "../include/gzdec.h"

/*DECLARATIONS OF FUNCTIONS*/

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



/* FUNCTIONS */

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
int decompressing_buffer(const char* gz_file, char** decompressed_data, size_t* data_size) {
    FILE* file = fopen(gz_file, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", gz_file);
        return EXIT_FAILURE;
    }

    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK) {
        fprintf(stderr, "Error initializing zlib for decompression\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    unsigned char in_buffer[CHUNK_SIZE];
    unsigned char out_buffer[CHUNK_SIZE];

    *decompressed_data = NULL;
    *data_size = 0;

    int ret;
    do {
        stream.avail_in = fread(in_buffer, 1, CHUNK_SIZE, file);
        if (stream.avail_in == 0 && feof(file)) {
            break;
        }
        stream.next_in = in_buffer;

        do {
            stream.next_out = out_buffer;
            stream.avail_out = CHUNK_SIZE;

            ret = inflate(&stream, Z_NO_FLUSH);
            if (ret != Z_OK && ret != Z_STREAM_END) {
                fprintf(stderr, "Error in decompression: %s\n", zError(ret));
                inflateEnd(&stream);
                fclose(file);
                free(*decompressed_data);
                return EXIT_FAILURE;
            }

            size_t bytes_written = CHUNK_SIZE - stream.avail_out;
            *decompressed_data = (char*)realloc(*decompressed_data, *data_size + bytes_written);
            memcpy(*decompressed_data + *data_size, out_buffer, bytes_written);
            *data_size += bytes_written;

        } while (stream.avail_out == 0);

    } while (ret != Z_STREAM_END);

    inflateEnd(&stream);
    fclose(file);

    return EXIT_SUCCESS;
}


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
int plugin_init(const char* decompressed_data, size_t data_size, const char* fileName) {
    GstElement* pipeline, * source, * sink;
    GstBus* bus;
    GstMessage* msg;
    GstStateChangeReturn ret;

    /* Initialize GStreamer */
    gst_init(NULL, NULL);

    /* Create the elements */
    source = gst_element_factory_make("videotestsrc", "source");
    sink = gst_element_factory_make("filesink", "sink");

    /* Check if elements are created successfully */
    if (!source || !sink) {
        g_printerr("GStreamer: not all elements could be created.\n");
        return -1;
    }

    /* Create the empty pipeline */
    pipeline = gst_pipeline_new("FileSavePipeline");

    if (!pipeline) {
        g_printerr("GStreamer: pipeline could not be created.\n");
        return -1;
    }
    else {
        g_printerr("GStreamer: pipeline succesfully created\n");
    }

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
    if (gst_element_link(source, sink) != TRUE) {
        g_printerr("GStreamer: elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    char* outputFileName = fileName;
    /* Set the file path for the sink element */
    g_object_set(sink, "location", outputFileName, NULL);

    /* Modify the source element properties as needed */
    g_object_set(source, "num-buffers", 100, NULL);  // Generate 100 frames, adjust as needed

    /* Set the pipeline to "playing" state */
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("GStreamer: unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }
    else {
        g_printerr("GStreamer: pipeline set to the playing state.\n");
    }

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Parse message */
    if (msg != NULL) {
        GError* err;
        gchar* debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("GStreamer: error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("GStreamer: debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("GStreamer: End-Of-Stream reached. \n");
            break;
        default:
            /* Should not be reached */
            g_printerr("GStreamer: unexpected message received.\n");
            break;
        }
        gst_message_unref(msg);
    }

    /* Free resources */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}


