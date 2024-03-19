#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    unsigned long start_block;
    unsigned long end_block;
} file_range;

typedef struct {
    unsigned long size;
    unsigned long block_size;
    unsigned long physical_partition_offset;
    int range_count;
    file_range *ranges;
    char original_partition_path[512];
} file_metadata;

file_metadata read_file_metadata(const char *metadata_file_path) {
    FILE *fp;
    file_metadata metadata;
    memset(&metadata, 0, sizeof(metadata));

    fp = fopen(metadata_file_path, "r");
    if (!fp) {
        printf("Error opening metadata file.\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = (char *)malloc(fsize + 1);
    if (!buffer) {
        fclose(fp);
        printf("Memory allocation failed.\n");
        exit(1);
    }

    fread(buffer, fsize, 1, fp);
    fclose(fp);
    buffer[fsize] = '\0';

    char *ptr = buffer;
    int itemsParsed = sscanf(ptr, "%s %lu %lu %d", metadata.original_partition_path ,&metadata.size, &metadata.block_size, &metadata.range_count);
    if (itemsParsed != 4) {
        printf("Failed to parse file size and block size.\n");
        free(buffer);
        exit(1);
    }

    printf("Parse metadata: size: %lu, block_size: %lu, range_count: %d\n", metadata.size, metadata.block_size, metadata.range_count);

    ptr = strchr(ptr, '\n') + 1; // Skip device path
    ptr = strchr(ptr, '\n') + 1; // Skip size and block size line

    metadata.ranges = (file_range *)malloc(metadata.range_count * sizeof(file_range));
    if (!metadata.ranges) {
        printf("Memory allocation for ranges failed.\n");
        free(buffer);
        exit(1);
    }

    for (int i = 0; i < metadata.range_count; ++i) {
        ptr = strchr(ptr, '\n');
        if (ptr) {
            ptr++;
            sscanf(ptr, "%lu %lu", &metadata.ranges[i].start_block, &metadata.ranges[i].end_block);
        }
    }

    free(buffer);
    return metadata;
}

void create_device_mapper_entry(file_metadata metadata, const char* output_file) {
    FILE *table_file = fopen(output_file, "w");
    if (!table_file) {
        printf("Error creating device-mapper table file.\n");
        exit(1);
    }

    int sectors_in_block = metadata.block_size/512;
    unsigned long virtual_start = 0;
    for (int i = 0; i < metadata.range_count; ++i) {
        unsigned long length = metadata.ranges[i].end_block - metadata.ranges[i].start_block;
        fprintf(table_file, "%lu %lu linear %s %lu\n", virtual_start*sectors_in_block, length*sectors_in_block, metadata.original_partition_path, metadata.ranges[i].start_block*sectors_in_block);
        virtual_start += length;
    }

    fclose(table_file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <metadata_file_path> <output file>\n", argv[0]);
        return 1;
    }

    file_metadata metadata = read_file_metadata(argv[1]);

    printf("Metadata read successfully.\n");

    create_device_mapper_entry(metadata, argv[2]);

    free(metadata.ranges);

    return 0;
}
