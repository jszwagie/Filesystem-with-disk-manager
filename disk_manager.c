#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define BLOCK_SIZE 4096
#define MAX_FILENAME_BYTES 64
#define PADDING_FB 16

typedef struct {
    uint32_t disk_size;
    uint32_t max_files;
    uint32_t file_count;
    uint32_t first_block_addr;
    uint32_t first_free_block;
    uint32_t name_block_addr;
    uint32_t catalog_addr;
    uint32_t catalog_size;
} DiskHeader; // 32 bytes

typedef struct {
    uint16_t file_type; // 1: hidden, 0: regular
    uint16_t name_type; // 1: addressable, 0: inline
    uint32_t file_size;
    uint32_t first_block_addr;
    uint32_t name_addr;
} FileEntry; // 16 bytes

typedef struct {
    uint32_t block_type; // 0: file, 1: name
    uint32_t next_block_addr;
    uint8_t data[BLOCK_SIZE - 8];
} Block; // 4096 bytes

typedef struct {
    char FileName[MAX_FILENAME_BYTES];
} FileName;

typedef struct {
    uint32_t block_type;
    uint32_t next_block_addr;
    uint32_t name_count;
    FileName names [(BLOCK_SIZE - 12) / sizeof(FileName)];
} NameBlock;

unsigned MAX_NAME_COUNT = (BLOCK_SIZE - 12) / sizeof(FileName);

unsigned min(unsigned a, unsigned b) {
    return a < b ? a : b;
}


uint32_t map_name_to_uint(const char *str) {
    uint32_t result = 0;
    unsigned len = strlen(str);
    for (unsigned i = 0; i < 4 && i < len; i++) {
        result |= ((uint8_t)str[i] << (8 * i));
    }
    return result;
}


char* map_uint_to_str(uint32_t addr) {
    static char str[5];
    for (int i = 0; i < 4; i++) {
        str[i] = (char)((addr >> (8 * i)) & 0xFF);
    }
    str[4] = '\0';
    return str;
}


int calculate_number_of_blocks(unsigned given_size) {
    int disk_size = given_size - sizeof(DiskHeader);
    int max_blocks = disk_size / BLOCK_SIZE;
    while ((disk_size - (max_blocks * BLOCK_SIZE) - max_blocks * sizeof(FileEntry) - max_blocks) < 0 && max_blocks >= 0) {
        max_blocks--;
    }
    return max_blocks;
}


void show_binary(const char *filename, const char *binary_filename) {
    int byte_count = 0;
    uint8_t byte;
    FILE *disk = fopen(filename, "rb");
    FILE *binary = fopen(binary_filename, "w");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }
    while (!feof(disk)) {
        fread(&byte, sizeof(uint8_t), 1, disk);
        fprintf(binary, "%02X ", byte);
        byte_count++;
        if (byte_count % 16 == 0) fprintf(binary, "\n");
    }

    fclose(disk);
    fclose(binary);
}


void show_chars(const char *filename, const char *chars_filename) {
    int byte_count = 0;
    uint8_t byte;
    FILE *disk = fopen(filename, "rb");
    FILE *chars = fopen(chars_filename, "w");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }
    while (!feof(disk)) {
        fread(&byte, sizeof(uint8_t), 1, disk);
        if (byte == 0) byte = '.';
        if (byte < 32 || byte > 126 && byte != '.') byte = '*';
        fprintf(chars, "%c", byte);
        byte_count++;
        if (byte_count % 16 == 0) fprintf(chars, "\n");
    }

    fclose(disk);
    fclose(chars);
}


void create_virtual_disk(const char *filename, unsigned size) {
    unsigned num_blocks = calculate_number_of_blocks(size);
    if (num_blocks <= 0) {
        printf("Disk size is too small.\n");
        exit(EXIT_FAILURE);
    }

    unsigned padding_for_bitmap = num_blocks + PADDING_FB - num_blocks % PADDING_FB - num_blocks;
    DiskHeader header;

    FILE *disk = fopen(filename, "wb");
    if (!disk) {
        perror("Error creating virtual disk");
        exit(EXIT_FAILURE);
    }
    // calculate padding, add padding to the number of blocks

    header.disk_size = size;
    header.max_files = num_blocks;
    header.file_count = 0;
    header.first_block_addr = sizeof(DiskHeader) + num_blocks + padding_for_bitmap + num_blocks * sizeof(FileEntry);
    header.first_free_block = sizeof(DiskHeader) + num_blocks + padding_for_bitmap + num_blocks * sizeof(FileEntry);
    header.name_block_addr = 0;
    header.catalog_addr = sizeof(DiskHeader) + num_blocks + padding_for_bitmap;
    header.catalog_size = num_blocks * sizeof(FileEntry);

    uint8_t *block_bitmap = malloc(num_blocks + padding_for_bitmap);
    if (!block_bitmap) {
        perror("Error allocating memory for block bitmap");
        fclose(disk);
        exit(EXIT_FAILURE);
    }

    FileEntry *catalog = malloc(num_blocks * sizeof(FileEntry));
    if (!catalog) {
        perror("Error allocating memory for catalog");
        fclose(disk);
        free(block_bitmap);
        exit(EXIT_FAILURE);
    }
    memset(block_bitmap, 0, num_blocks + padding_for_bitmap);
    memset(catalog, 0, num_blocks * sizeof(FileEntry));

    fwrite(&header, sizeof(DiskHeader), 1, disk);
    fwrite(block_bitmap, num_blocks + padding_for_bitmap, 1, disk);
    fwrite(catalog, num_blocks * sizeof(FileEntry), 1, disk);

    unsigned unallocated_space = size - ftell(disk);

    if (unallocated_space > 0) {
        uint8_t zero = 0;
        for (unsigned i = 0; i < unallocated_space; i++) {
            fwrite(&zero, sizeof(uint8_t), 1, disk);
        }
    }

    fclose(disk);
    free(block_bitmap);
    free(catalog);

}


void display_disk(const char *filename) {
    DiskHeader header;
    uint8_t block_status;
    FILE *disk = fopen(filename, "rb");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }

    fread(&header, sizeof(DiskHeader), 1, disk);

    printf("=== Disk Header ===\n");
    printf("Disk size: %u bytes\n", header.disk_size);
    printf("Max files: %u\n", header.max_files);
    printf("File count: %u\n", header.file_count);
    printf("First block address: %u\n", header.first_block_addr);
    printf("First free block: %u\n", header.first_free_block);
    printf("Name block address: %u\n", header.name_block_addr);
    printf("Catalog address: %u\n", header.catalog_addr);
    printf("Catalog size: %u\n", header.catalog_size);

    printf("\n=== Block Bitmap ===\n");
    fseek(disk, sizeof(DiskHeader), SEEK_SET);

    unsigned i = header.max_files;
    while (i > 0) {
        printf("|");
        for (unsigned row = 0; row < 100; row++) {
            fread(&block_status, sizeof(uint8_t), 1, disk);
            if (block_status == 0) {
                printf("0");
            } else if (block_status == 1) {
                printf("X");
            } else {
                printf("N");
            }
            i--;
            if (i == 0) {
                break;
            }
        }
        printf("|\n");
    }

    FileEntry *catalog = malloc(header.max_files * sizeof(FileEntry));
    if (!catalog) {
        perror("Error allocating memory for catalog");
        fclose(disk);
        exit(EXIT_FAILURE);
    }

    fseek(disk, header.catalog_addr, SEEK_SET);
    fread(catalog, header.max_files * sizeof(FileEntry), 1, disk);
    printf("\n=== Catalog ===\n");

    for (unsigned i = 0; i < header.file_count; i++) {
        printf("File %u:\n", i + 1);
        printf("  Type: %s\n", catalog[i].file_type ? "Hidden" : "Regular");
        printf("  Size: %u bytes\n", catalog[i].file_size);
        printf("  First block address: %u\n", catalog[i].first_block_addr);
        printf("  Name type: %s\n", catalog[i].name_type ? "Addressable" : "Inline");
        char name[MAX_FILENAME_BYTES] = {0};
        if (catalog[i].name_type == 0) {
            strncpy(name, map_uint_to_str(catalog[i].name_addr), MAX_FILENAME_BYTES);
        }
        else {
            fseek(disk, catalog[i].name_addr, SEEK_SET);
            fread(name, sizeof(char), MAX_FILENAME_BYTES, disk);
            fseek(disk, header.catalog_addr, SEEK_SET);
        }
        printf("  Name: %s\n", name);
    }

    free(catalog);
}


uint32_t save_filename_to_disk(FILE *disk, const char *filename, DiskHeader *header, uint8_t *block_bitmap, FileEntry *catalog) {
    unsigned first_block = header->name_block_addr;
    unsigned current_block = first_block;
    unsigned previous_block = 0;
    unsigned next_block = 0;
    unsigned name_index = 0;
    NameBlock prev_name_block = {0};
    NameBlock name_block = {0};
    while (current_block != 0) {
        fseek(disk, current_block, SEEK_SET);
        fread(&name_block, sizeof(NameBlock), 1, disk);
        if (name_block.name_count < MAX_NAME_COUNT) {
            name_index = name_block.name_count;
            break;
        }
        previous_block = current_block;
        next_block = name_block.next_block_addr;
        current_block = next_block;
    }

    if (current_block == 0) {
        // create new block
        current_block = header->first_free_block;
        memset(&name_block, 0, sizeof(NameBlock));
        name_block.block_type = 1;
        name_block.next_block_addr = 0;
        strncpy(name_block.names[0].FileName, filename, MAX_FILENAME_BYTES);
        block_bitmap[(current_block - header->first_block_addr) / BLOCK_SIZE] = 2;
        // find next free block
        unsigned copy_current_block = current_block + BLOCK_SIZE;
        while (block_bitmap[(copy_current_block - header->first_block_addr) / BLOCK_SIZE] != 0) {
            copy_current_block += BLOCK_SIZE;
        }
        header->first_free_block = copy_current_block;
        // add address to previous block
        if (previous_block != 0) {
            fseek(disk, previous_block, SEEK_SET);
            fread(&prev_name_block, sizeof(NameBlock), 1, disk);
            prev_name_block.next_block_addr = current_block;
            fseek(disk, previous_block, SEEK_SET);
            fwrite(&prev_name_block, sizeof(NameBlock), 1, disk);
        } else {
            header->name_block_addr = current_block;
        }
    }

    strncpy(name_block.names[name_index].FileName, filename, MAX_FILENAME_BYTES);
    name_block.name_count++;

    fseek(disk, current_block, SEEK_SET);
    fwrite(&name_block, sizeof(NameBlock), 1, disk);

    return current_block + (name_index * sizeof(FileName)) + 3 * sizeof(uint32_t);
}


void copy_file_to_disk(const char *diskname, const char *filename) {
    FILE *src_file = fopen(filename, "rb");
    if (!src_file) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }
    FILE *disk = fopen(diskname, "rb+");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }

    // Read disk header
    DiskHeader header = {0};
    fread(&header, sizeof(DiskHeader), 1, disk);

    // Read block bitmap
    fseek(disk, sizeof(DiskHeader), SEEK_SET);
    uint8_t *block_bitmap = malloc(header.max_files * sizeof(uint8_t));
    if (!block_bitmap) {
        perror("Error allocating memory for block bitmap");
        fclose(disk);
        fclose(src_file);
        exit(EXIT_FAILURE);
    }
    fread(block_bitmap, sizeof(uint8_t), header.max_files, disk);

    unsigned free_blocks = 0;
    for (unsigned i = 0; i < header.max_files; i++) {
        if (block_bitmap[i] == 0) {
            free_blocks++;
        }
    }

    fseek(src_file, 0, SEEK_END);
    unsigned file_size = ftell(src_file);
    fseek(src_file, 0, SEEK_SET);

    unsigned data_per_block = BLOCK_SIZE - 2 * sizeof(uint32_t);
    unsigned required_blocks = (file_size + data_per_block - 1) / data_per_block;


    if (free_blocks < required_blocks) {
        printf("Not enough free blocks to copy the file.\n");
        fclose(disk);
        fclose(src_file);
        free(block_bitmap);
        return;
    }

    // read catalog
    fseek(disk, header.catalog_addr, SEEK_SET);
    FileEntry *catalog = malloc(header.max_files * sizeof(FileEntry));
    if (!catalog) {
        perror("Error allocating memory for catalog");
        fclose(disk);
        fclose(src_file);
        free(block_bitmap);
        exit(EXIT_FAILURE);
    }

    for (unsigned i = 0; i < header.max_files; i++) {
        fread(&catalog[i], sizeof(FileEntry), 1, disk);
    }

    // check if file already exists
    for (unsigned i = 0; i < header.file_count; i++) {
        if (catalog[i].name_type == 0) {
            char name[4] = {0};
            strncpy(name, map_uint_to_str(catalog[i].name_addr), sizeof(uint32_t));
            if (strcmp(name, filename) == 0) {
                printf("File already exists.\n");
                fclose(disk);
                fclose(src_file);
                free(block_bitmap);
                free(catalog);
                return;
            }
        } else {
            char name[MAX_FILENAME_BYTES] = {0};
            fseek(disk, catalog[i].name_addr, SEEK_SET);
            fread(name, sizeof(char), MAX_FILENAME_BYTES, disk);
            if (strcmp(name, filename) == 0) {
                printf("File already exists.\n");
                fclose(disk);
                fclose(src_file);
                free(block_bitmap);
                free(catalog);
                return;
            }
        }
    }

    // save file to disk
    unsigned first_block = header.first_block_addr;
    unsigned first_free_block = header.first_free_block;
    unsigned current_block = first_free_block;
    unsigned copy_current_block = first_free_block;
    unsigned next_free_block = 0;

    for (unsigned i = required_blocks; i > 0; i--) {
        // find next free block
        copy_current_block = current_block + BLOCK_SIZE;
        while (block_bitmap[(copy_current_block - first_block) / BLOCK_SIZE] != 0) {
            copy_current_block += BLOCK_SIZE;
        }
        next_free_block = copy_current_block;
        // go back

        // make a block
        Block block = {0};
        block.block_type = 0;
        if (i == 1) {
            block.next_block_addr = 0;
        } else block.next_block_addr = next_free_block;

        // write data
        fread(block.data, sizeof(uint8_t), BLOCK_SIZE - 8, src_file);

        // write block to disk
        fseek(disk, current_block, SEEK_SET);
        fwrite(&block, sizeof(Block), 1, disk);

        // update bitmap
        block_bitmap[(current_block - first_block) / BLOCK_SIZE] = 1;

        current_block = next_free_block;
    }

    // update header
    header.file_count++;
    header.first_free_block = current_block;

    // update catalog
    unsigned file_index = header.file_count - 1;
    if (filename[0] == '.')
        catalog[file_index].file_type = 1;
    else
        catalog[file_index].file_type = 0;
    catalog[file_index].file_size = file_size;
    catalog[file_index].first_block_addr = first_free_block;
    if (strlen(filename) < 4) {
        catalog[file_index].name_addr = map_name_to_uint(filename);
        catalog[file_index].name_type = 0;
    } else {
        uint32_t address;
        // save filename to disk
        address = save_filename_to_disk(disk, filename, &header, block_bitmap, catalog);
        catalog[file_index].name_addr = address;
        catalog[file_index].name_type = 1;
    }



    // write back
    fseek(disk, 0, SEEK_SET);
    fwrite(&header, sizeof(DiskHeader), 1, disk);
    fseek(disk, sizeof(DiskHeader), SEEK_SET);

    fwrite(block_bitmap, sizeof(uint8_t), header.max_files, disk);

    fseek(disk, header.catalog_addr, SEEK_SET);
    for (unsigned i = 0; i < header.max_files; i++) {
        fwrite(&catalog[i], sizeof(FileEntry), 1, disk);
    }

    fclose(disk);
    fclose(src_file);
    free(block_bitmap);
    free(catalog);
}


void remove_filename_from_disk(FILE *disk,  DiskHeader *header, const char *filename, uint8_t *block_bitmap) {

    unsigned name_index = 0;
    unsigned first_block = header->name_block_addr;
    unsigned current_block = first_block;
    unsigned previous_block = 0;
    int found = 0;
    NameBlock name_block = {0};
    while (current_block != 0) {
        fseek(disk, current_block, SEEK_SET);
        fread(&name_block, sizeof(NameBlock), 1, disk);
        for (unsigned i = 0; i < name_block.name_count; i++) {
            char name[MAX_FILENAME_BYTES] = {0};
            strncpy(name, name_block.names[i].FileName, MAX_FILENAME_BYTES);
            if (strcmp(name, filename) == 0) {
                name_index = i;
                found = 1;
                break;
            }
        }
        if (found == 0) {
            previous_block = current_block;
            current_block = name_block.next_block_addr;
        } else {
            break;
        }
    }

    // remove name from block
    for (unsigned i = name_index; i < name_block.name_count - 1; i++) {
        name_block.names[i] = name_block.names[i + 1];
    }
    name_block.name_count--;

    if (name_block.name_count == 0) {
        // remove block
        if (previous_block != 0) {
            fseek(disk, previous_block, SEEK_SET);
            NameBlock prev_block = {0};
            fread(&prev_block, sizeof(NameBlock), 1, disk);
            prev_block.next_block_addr = 0;
            block_bitmap[(current_block - header->first_block_addr) / BLOCK_SIZE] = 0;
            fseek(disk, previous_block, SEEK_SET);
            fwrite(&prev_block, sizeof(NameBlock), 1, disk);
            header->name_block_addr = previous_block;
        } else {
            header->name_block_addr = 0;
            block_bitmap[(current_block - header->first_block_addr) / BLOCK_SIZE] = 0;
        }
    } else {
        // write back
        fseek(disk, current_block, SEEK_SET);
        fwrite(&name_block, sizeof(NameBlock), 1, disk);
    }

}


void remove_file_from_disk(const char *diskname, const char *filename) {
    FILE *disk = fopen(diskname, "rb+");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }

    // Read disk header
    DiskHeader header = {0};
    fread(&header, sizeof(DiskHeader), 1, disk);

    // Read block bitmap
    fseek(disk, sizeof(DiskHeader), SEEK_SET);
    uint8_t *block_bitmap = malloc(header.max_files * sizeof(uint8_t));
    if (!block_bitmap) {
        perror("Error allocating memory for block bitmap");
        fclose(disk);
        exit(EXIT_FAILURE);
    }
    fread(block_bitmap, sizeof(uint8_t), header.max_files, disk);

    // read catalog
    fseek(disk, header.catalog_addr, SEEK_SET);
    FileEntry *catalog = malloc(header.max_files * sizeof(FileEntry));
    if (!catalog) {
        perror("Error allocating memory for catalog");
        fclose(disk);
        free(block_bitmap);
        exit(EXIT_FAILURE);
    }
    fread(catalog, sizeof(FileEntry), header.max_files, disk);

    char *name_copy = malloc(MAX_FILENAME_BYTES);

    int found = 0;
    unsigned file_index = 0;
    for (unsigned i = 0; i < header.file_count; i++) {
        if (catalog[i].name_type == 0) {
            char name[4] = {0};
            strncpy(name, map_uint_to_str(catalog[i].name_addr), sizeof(uint32_t));
            if (strcmp(name, filename) == 0) {
                file_index = i;
                found = 1;
                break;
            }
        } else {
            char name[MAX_FILENAME_BYTES] = {0};
            fseek(disk, catalog[i].name_addr, SEEK_SET);
            fread(name, sizeof(char), MAX_FILENAME_BYTES, disk);
            if (strcmp(name, filename) == 0) {
                file_index = i;
                found = 1;
                name_copy = name;
                break;
            }
        }
    }

    if (found == 0) {
        printf("File not found.\n");
        fclose(disk);
        free(block_bitmap);
        free(catalog);
        return;
    }


    unsigned first_block = catalog[file_index].first_block_addr;
    unsigned current_block = first_block;
    unsigned next_block = 0;
    while (current_block != 0) {
        fseek(disk, current_block, SEEK_SET);
        Block block = {0};
        fread(&block, sizeof(Block), 1, disk);
        next_block = block.next_block_addr;
        block_bitmap[(current_block - header.first_block_addr) / BLOCK_SIZE] = 0;
        current_block = next_block;
    }

    // update catalog
    if (catalog[file_index].name_type == 1) {
        remove_filename_from_disk(disk, &header, name_copy, block_bitmap);
    }

    for (unsigned i = file_index; i < header.file_count - 1; i++) {
        catalog[i] = catalog[i + 1];
    }

    // update header
    header.file_count--;
    if (first_block < header.first_free_block) {
        header.first_free_block = first_block;
    }

    // write back
    fseek(disk, 0, SEEK_SET);
    fwrite(&header, sizeof(DiskHeader), 1, disk);
    fseek(disk, sizeof(DiskHeader), SEEK_SET);

    fwrite(block_bitmap, sizeof(uint8_t), header.max_files, disk);

    fseek(disk, header.catalog_addr, SEEK_SET);
    for (unsigned i = 0; i < header.max_files; i++) {
        fwrite(&catalog[i], sizeof(FileEntry), 1, disk);
    }

    fclose(disk);
    free(block_bitmap);
    free(catalog);
}


void copy_file_outside(const char *diskname, const char *filename) {
    FILE *disk = fopen(diskname, "rb+");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }

    FILE *dest_file = fopen(filename, "wb");
    if (!dest_file) {
        perror("Error opening destination file");
        fclose(disk);
        exit(EXIT_FAILURE);
    }

    // Read disk header
    DiskHeader header = {0};
    fread(&header, sizeof(DiskHeader), 1, disk);

    // Read block bitmap
    fseek(disk, sizeof(DiskHeader), SEEK_SET);
    uint8_t *block_bitmap = malloc(header.max_files * sizeof(uint8_t));
    if (!block_bitmap) {
        perror("Error allocating memory for block bitmap");
        fclose(disk);
        exit(EXIT_FAILURE);
    }
    fread(block_bitmap, sizeof(uint8_t), header.max_files, disk);

    // read catalog
    fseek(disk, header.catalog_addr, SEEK_SET);
    FileEntry *catalog = malloc(header.max_files * sizeof(FileEntry));
    if (!catalog) {
        perror("Error allocating memory for catalog");
        fclose(disk);
        free(block_bitmap);
        exit(EXIT_FAILURE);
    }
    fread(catalog, sizeof(FileEntry), header.max_files, disk);

    int found = 0;
    unsigned file_index = 0;
    for (unsigned i = 0; i < header.file_count; i++) {
        if (catalog[i].name_type == 0) {
            char name[4] = {0};
            strncpy(name, map_uint_to_str(catalog[i].name_addr), sizeof(uint32_t));
            if (strcmp(name, filename) == 0) {
                file_index = i;
                found = 1;
                break;
            }
        } else {
            char name[MAX_FILENAME_BYTES] = {0};
            fseek(disk, catalog[i].name_addr, SEEK_SET);
            fread(name, sizeof(char), MAX_FILENAME_BYTES, disk);
            if (strcmp(name, filename) == 0) {
                file_index = i;
                found = 1;
                break;
            }
        }
    }

    if (found == 0) {
        printf("File not found.\n");
        fclose(disk);
        free(block_bitmap);
        free(catalog);
        return;
    }

    unsigned first_block = catalog[file_index].first_block_addr;
    unsigned current_block = first_block;
    unsigned next_block = 0;
    unsigned file_size = catalog[file_index].file_size;
    while (current_block != 0) {
        fseek(disk, current_block, SEEK_SET);
        Block block = {0};
        fread(&block, sizeof(Block), 1, disk);
        next_block = block.next_block_addr;
        fwrite(block.data, sizeof(uint8_t), min(BLOCK_SIZE - 8, file_size), dest_file);
        current_block = next_block;
        file_size -= (min(BLOCK_SIZE - 8, file_size));
    }

    fclose(disk);
    fclose(dest_file);
    free(block_bitmap);
    free(catalog);
}


void save_config(char *diskname, const char *config_filename) {
    FILE *config = fopen(config_filename, "w");
    if (!config) {
        perror("Error opening config file");
        fclose(config);
        exit(EXIT_FAILURE);
    }

    fwrite(diskname, sizeof(char), strlen(diskname), config);

    fclose(config);
}


char *load_config(const char *config_filename) {
    FILE *config = fopen(config_filename, "r");
    if (!config) {
        perror("Error opening config file");
        exit(EXIT_FAILURE);
    }

    char *diskname = malloc(256);
    if (!diskname) {
        perror("Error allocating memory for diskname");
        fclose(config);
        exit(EXIT_FAILURE);
    }

    fread(diskname, sizeof(char), 256, config);

    fclose(config);
    return diskname;
}


void unload_config(char *diskname) {
    free(diskname);
}


void clear_config(const char *config_filename) {
    FILE *config = fopen(config_filename, "w");
    if (!config) {
        perror("Error opening config file");
        fclose(config);
        exit(EXIT_FAILURE);
    }

    fclose(config);
}


void remove_disk(const char *filename) {
    if (remove(filename) != 0) {
        perror("Error removing virtual disk");
        exit(EXIT_FAILURE);
    }
}


void ls(const char *diskname, char arg) {
    FILE *disk = fopen(diskname, "rb");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }

    // Read disk header
    DiskHeader header = {0};
    fread(&header, sizeof(DiskHeader), 1, disk);

    // read catalog
    fseek(disk, header.catalog_addr, SEEK_SET);
    FileEntry *catalog = malloc(header.max_files * sizeof(FileEntry));
    if (!catalog) {
        perror("Error allocating memory for catalog");
        fclose(disk);
        exit(EXIT_FAILURE);
    }
    fread(catalog, sizeof(FileEntry), header.max_files, disk);

    // find files
    for (unsigned i = 0; i < header.file_count; i++) {
        if (catalog[i].name_type == 0) {
            char name[4] = {0};
            strncpy(name, map_uint_to_str(catalog[i].name_addr), sizeof(uint32_t));
            if (arg == 'a' || name[0] != '.') {
                printf("%s\n", name);
            }
        } else {
            char name[MAX_FILENAME_BYTES] = {0};
            fseek(disk, catalog[i].name_addr, SEEK_SET);
            fread(name, sizeof(char), MAX_FILENAME_BYTES, disk);
            if (arg == 'a' || name[0] != '.') {
                printf("%s\n", name);
            }
        }
    }

    fclose(disk);
    free(catalog);
}


void bad_usage() {
    printf("Usage: ./disk_manager <command> <diskname> <args>\n");
    exit(EXIT_FAILURE);
}


void man() {
    printf("Possible commands:\n");
    printf("  - create <diskname> <size> # create virtual disk\n");
    printf("  - cp <-in/-out> <filename> # copy file to/from disk\n");
    printf("  - rm <filename> # remove file from disk\n");
    printf("  - unmount <diskname> # remove virtual disk\n");
    printf("  - info <diskname> # display disk info\n");
    printf("  - ls <-a> # list files\n");
}


int main(int argc, char *argv[]) {
    /* Usage: ./disk_manager <command> <args>
    Possible commands:
    - create <diskname> <size> # create virtual disk
    - cp <-in/-out> <filename> # copy file to/from disk
    - rm <filename> # remove file from disk
    - unmount <diskname> # remove virtual disk
    - info <diskname> # display disk info
    - ls <-a> # list files
    - man # display manual
    */
    return 0;

}