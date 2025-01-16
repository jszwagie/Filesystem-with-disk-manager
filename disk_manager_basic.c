#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define BLOCK_SIZE 4096
#define MAX_FILENAME_BYTES 64

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
    uint8_t file_type; // 1: hidden, 0: regular
    uint32_t file_size;
    uint32_t first_block_addr;
    uint8_t name_type; // 1: addressable, 0: inline
    uint32_t name_addr;
    uint16_t padding;
} FileEntry; // 16 bytes

typedef struct {
    uint8_t block_type; // 0: file, 1: name
    uint32_t next_block_addr;
    uint8_t data[BLOCK_SIZE - 5];
} Block; // 4096 bytes

unsigned min(unsigned a, unsigned b) {
    return a < b ? a : b;
}


// Name Block will be implemented in the next version

uint32_t map_name_to_uint(const char *str) {
    uint32_t result = 0;
    for (unsigned i = 0; i < 4 && i < strlen(str); i++) {
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


unsigned calculate_number_of_blocks(unsigned given_size) {
    unsigned disk_size = given_size - sizeof(DiskHeader);
    unsigned max_blocks = disk_size / BLOCK_SIZE;
    while ((disk_size - (max_blocks * BLOCK_SIZE) - max_blocks * sizeof(FileEntry) - max_blocks) < 0) {
        max_blocks--;
    }
    return max_blocks;
}


void show_binary(const char *filename, const char *binary_filename) {
    FILE *disk = fopen(filename, "rb");
    FILE *binary = fopen(binary_filename, "w");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }
    int byte_count = 0;
    while (!feof(disk)) {
        uint8_t byte;
        fread(&byte, sizeof(uint8_t), 1, disk);
        fprintf(binary, "%02X ", byte); // Wypisanie bajtu w formacie szesnastkowym
        byte_count++;
        if (byte_count % 16 == 0) fprintf(binary, "\n"); // Nowa linia co 16 bajtów
    }

    fclose(disk);
    fclose(binary);
}


void create_virtual_disk(const char *filename, unsigned size) {
    FILE *disk = fopen(filename, "wb");
    if (!disk) {
        perror("Error creating virtual disk");
        exit(EXIT_FAILURE);
    }

    unsigned num_blocks = calculate_number_of_blocks(size);
    // calculate padding, add padding to the number of blocks
    unsigned padding_for_bitmap = num_blocks + 8 - num_blocks % 8 - num_blocks;
    printf("padding: %u\n", padding_for_bitmap);

    DiskHeader header;
    header.disk_size = size;
    header.max_files = num_blocks;
    header.file_count = 0;
    header.first_block_addr = sizeof(DiskHeader) + num_blocks + padding_for_bitmap + num_blocks * sizeof(FileEntry);
    header.first_free_block = sizeof(DiskHeader) + num_blocks + padding_for_bitmap + num_blocks * sizeof(FileEntry);
    header.name_block_addr = 0;
    header.catalog_addr = sizeof(DiskHeader) + num_blocks + padding_for_bitmap;
    header.catalog_size = num_blocks * sizeof(FileEntry);

    uint8_t block_bitmap[num_blocks + padding_for_bitmap];
    FileEntry catalog[num_blocks];
    memset(block_bitmap, 0, num_blocks + padding_for_bitmap);
    memset(catalog, 0, num_blocks * sizeof(FileEntry));

    fwrite(&header, sizeof(DiskHeader), 1, disk);
    fwrite(block_bitmap, num_blocks + padding_for_bitmap, 1, disk);
    fwrite(catalog, num_blocks * sizeof(FileEntry), 1, disk);

    fclose(disk);

}


void display_disk(const char *filename) {
    FILE *disk = fopen(filename, "rb");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }

    DiskHeader header;
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
            uint8_t block_status;
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


    printf("\n=== Catalog ===\n");
    fseek(disk, header.catalog_addr, SEEK_SET);
    for (unsigned i = 0; i < header.file_count; i++) {
        FileEntry entry;
        fread(&entry, sizeof(FileEntry), 1, disk);
        printf("File %u:\n", i + 1);
        printf("  Type: %s\n", entry.file_type ? "Hidden" : "Regular");
        printf("  Size: %u bytes\n", entry.file_size);
        printf("  First block address: %u\n", entry.first_block_addr);
        printf("  Name type: %s\n", entry.name_type ? "Addressable" : "Inline");
        char name[MAX_FILENAME_BYTES] = {0};
        if (entry.name_type == 0) { // Inline name
            strncpy(name, map_uint_to_str(entry.name_addr), MAX_FILENAME_BYTES);
        }
        printf("  Name: %s\n", name);
    }
}


void copy_file_to_disk(const char *diskname, const char *filename) {
    FILE *disk = fopen(diskname, "rb+");
    if (!disk) {
        perror("Error opening virtual disk");
        exit(EXIT_FAILURE);
    }

    FILE *src_file = fopen(filename, "rb");
    if (!src_file) {
        perror("Error opening source file");
        fclose(disk);
        exit(EXIT_FAILURE);
    }

    // Read disk header
    DiskHeader header = {0};
    fread(&header, sizeof(DiskHeader), 1, disk);


    if (header.file_count >= header.max_files) {
        printf("No space for new files in the catalog.\n");
        fclose(disk);
        fclose(src_file);
        return;
    }

    // Get source file size
    fseek(src_file, 0, SEEK_END);
    uint32_t file_size = ftell(src_file);
    fseek(src_file, 0, SEEK_SET);

    unsigned required_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Read block bitmap
    fseek(disk, sizeof(DiskHeader), SEEK_SET);
    uint8_t block_bitmap[header.max_files];
    fread(block_bitmap, sizeof(uint8_t), header.max_files, disk);


    unsigned free_blocks = 0;
    for (unsigned i = 0; i < header.max_files; i++) {
        if (block_bitmap[i] == 0) {
            free_blocks++;
        }
    }
    if (free_blocks < required_blocks) {
        printf("Not enough space on the virtual disk.\n");
        fclose(disk);
        fclose(src_file);
        return;
    }

    // Read catalog
    fseek(disk, header.catalog_addr, SEEK_SET);
    FileEntry catalog[header.file_count];
    for (unsigned i = 0; i < header.file_count; i++) {
        fread(&catalog[i], sizeof(FileEntry), 1, disk);
    }

    // insert data
    unsigned first_block = header.first_free_block;
    unsigned current_block = first_block;
    unsigned next_free_block = 0;
    unsigned prev_block = 0;

    for (unsigned i = 0; i < required_blocks; i++) {

        // write data
        Block block = {0};
        block.block_type = 0;
        block.next_block_addr = 0;
        fread(block.data, sizeof(uint8_t), BLOCK_SIZE - 5, src_file);
        fseek(disk, current_block, SEEK_SET);
        fwrite(&block, sizeof(Block), 1, disk);
        block_bitmap[(current_block - header.first_block_addr)/BLOCK_SIZE] = 1;
        required_blocks--;

        // find next free block
        for (unsigned j = i; j < header.max_files; j++) {
            if (block_bitmap[j] == 0) {
                next_free_block = j;
                break;
            }
        }

        prev_block = current_block;
        current_block = first_block + next_free_block * BLOCK_SIZE;

        if (required_blocks == 0) {
            break;
        }
    }

    // make a copy of block bitmap
    uint8_t block_bitmap_copy[header.max_files];
    memcpy(block_bitmap_copy, block_bitmap, header.max_files);


    // Update catalog
    // find first free catalog entry
    unsigned catalog_entry = 0;
    for (unsigned i = 0; i < header.max_files; i++) {
        if (catalog[i].file_size == 0) {
            catalog_entry = i;
            break;
        }
    }


    catalog[catalog_entry].name_type = 0;
    catalog[catalog_entry].name_addr = map_name_to_uint(filename);
    catalog[catalog_entry].file_type = 0;
    catalog[catalog_entry].file_size = file_size;
    catalog[catalog_entry].first_block_addr = first_block;

    header.file_count++;
    header.first_free_block = first_block + next_free_block * BLOCK_SIZE;
    // Write header back
    fseek(disk, 0, SEEK_SET);
    fwrite(&header, sizeof(DiskHeader), 1, disk);


    // Write block bitmap back
    fseek(disk, sizeof(DiskHeader), SEEK_SET);
    fwrite(block_bitmap_copy, sizeof(uint8_t), header.max_files, disk);

    // Write catalog back
    fseek(disk, header.catalog_addr, SEEK_SET);
    for (unsigned i = 0; i < header.max_files; i++) {
        fwrite(&catalog[i], sizeof(FileEntry), 1, disk);
    }

    fclose(disk);
    fclose(src_file);

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
    uint8_t block_bitmap[header.max_files];
    fread(block_bitmap, sizeof(uint8_t), header.max_files, disk);

    // Read catalog
    fseek(disk, header.catalog_addr, SEEK_SET);
    FileEntry catalog[header.file_count];
    for (unsigned i = 0; i < header.file_count; i++) {
        fread(&catalog[i], sizeof(FileEntry), 1, disk);
    }

    // find file in catalog
    unsigned file_index = 0;
    for (unsigned i = 0; i < header.file_count; i++) {
        if (catalog[i].name_addr == map_name_to_uint(filename)) {
            file_index = i;
            break;
        }
    }

    // find first block of the file
    unsigned first_block = catalog[file_index].first_block_addr;
    unsigned current_block = first_block;
    unsigned next_block = 0;

    // mark blocks as free
    while (current_block != 0) {
        fseek(disk, current_block, SEEK_SET);
        Block block = {0};
        fread(&block, sizeof(Block), 1, disk);
        next_block = block.next_block_addr;
        block_bitmap[(current_block - header.first_block_addr)/BLOCK_SIZE] = 0;
        current_block = next_block;
    }

    // make a copy of block bitmap
    uint8_t block_bitmap_copy[header.max_files];
    memcpy(block_bitmap_copy, block_bitmap, header.max_files);

    // mark catalog entry as free
    catalog[file_index].file_size = 0;
    catalog[file_index].first_block_addr = 0;
    catalog[file_index].name_addr = 0;
    catalog[file_index].name_type = 0;
    catalog[file_index].file_type = 0;

    header.file_count--;
    if (first_block < header.first_free_block) {
        header.first_free_block = first_block;
    }

    // Write header back
    fseek(disk, 0, SEEK_SET);
    fwrite(&header, sizeof(DiskHeader), 1, disk);

    // Write block bitmap back
    fseek(disk, sizeof(DiskHeader), SEEK_SET);
    fwrite(block_bitmap_copy, sizeof(uint8_t), header.max_files, disk);

    // Write catalog back
    fseek(disk, header.catalog_addr, SEEK_SET);
    for (unsigned i = 0; i < header.max_files; i++) {
        fwrite(&catalog[i], sizeof(FileEntry), 1, disk);
    }

    fclose(disk);
}

void copy_file_outside(const char *diskname, const char *filename) {
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
    uint8_t block_bitmap[header.max_files];
    fread(block_bitmap, sizeof(uint8_t), header.max_files, disk);

    // Read catalog
    fseek(disk, header.catalog_addr, SEEK_SET);
    FileEntry catalog[header.file_count];
    for (unsigned i = 0; i < header.file_count; i++) {
        fread(&catalog[i], sizeof(FileEntry), 1, disk);
    }

    // find file in catalog
    unsigned file_index = 0;
    for (unsigned i = 0; i < header.file_count; i++) {
        if (catalog[i].name_addr == map_name_to_uint(filename)) {
            file_index = i;
            break;
        }
    }

    // find first block of the file
    unsigned first_block = catalog[file_index].first_block_addr;
    unsigned current_block = first_block;
    unsigned next_block = 0;

    FILE *dest_file = fopen(filename, "wb");
    if (!dest_file) {
        perror("Error opening destination file");
        fclose(disk);
        exit(EXIT_FAILURE);
    }

    unsigned file_size = catalog[file_index].file_size;
    // copy data
    while (current_block != 0) {
        fseek(disk, current_block, SEEK_SET);
        Block block = {0};
        fread(&block, sizeof(Block), 1, disk);
        next_block = block.next_block_addr;
        fwrite(block.data, sizeof(uint8_t), min(BLOCK_SIZE - 5, file_size), dest_file);
        file_size -= min(BLOCK_SIZE - 5, file_size);
        current_block = next_block;
    }

    fclose(disk);

}


int main() {
    unsigned size = 131072;
    printf("Number of blocks: %u\n", calculate_number_of_blocks(size));
    create_virtual_disk("disk", size);
    display_disk("disk");
    copy_file_to_disk("disk", "v.s");
    display_disk("disk");

    // remove file from our catalog on linux to allow copying it again
    remove("v.s");

    copy_file_outside("disk", "v.s");
    printf("File copied\n");
    remove_file_from_disk("disk", "v.s");
    display_disk("disk");
    return 0;
}