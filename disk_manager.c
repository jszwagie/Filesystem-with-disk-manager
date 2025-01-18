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
    uint32_t file_type; // 1: hidden, 0: regular
    uint32_t file_size;
    uint32_t first_block_addr;
    //uint16_t name_type; // 1: addressable, 0: inline
    uint32_t name_addr;
} FileEntry; // 16 bytes

typedef struct {
    uint32_t block_type; // 0: file, 1: name
    uint32_t next_block_addr;
    uint8_t data[BLOCK_SIZE - 8];
} Block; // 4096 bytes


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


unsigned calculate_number_of_blocks(unsigned given_size) {
    unsigned disk_size = given_size - sizeof(DiskHeader);
    unsigned max_blocks = disk_size / BLOCK_SIZE;
    while ((disk_size - (max_blocks * BLOCK_SIZE) - max_blocks * sizeof(FileEntry) - max_blocks) < 0) {
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
        fprintf(binary, "%02X ", byte); // Wypisanie bajtu w formacie szesnastkowym
        byte_count++;
        if (byte_count % 16 == 0) fprintf(binary, "\n"); // Nowa linia co 16 bajtów
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
        if (byte == 0) byte = '.'; // Zamiana bajtów zerowych na kropki
        if (byte < 32 || byte > 126 && byte != '.') byte = '*'; // Zamiana znaków niebędących znakami ASCII na gwiazdki
        fprintf(chars, "%c", byte); // Wypisanie bajtu jako znaku
        byte_count++;
        if (byte_count % 16 == 0) fprintf(chars, "\n"); // Nowa linia co 16 bajtów
    }

    fclose(disk);
    fclose(chars);
}


void create_virtual_disk(const char *filename, unsigned size) {
    unsigned num_blocks = calculate_number_of_blocks(size);
    unsigned padding_for_bitmap = num_blocks + 8 - num_blocks % 8 - num_blocks;
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


    printf("\n=== Catalog ===\n");
    fseek(disk, header.catalog_addr, SEEK_SET);
    for (unsigned i = 0; i < header.file_count; i++) {
        FileEntry entry;
        fread(&entry, sizeof(FileEntry), 1, disk);
        printf("File %u:\n", i + 1);
        printf("  Type: %s\n", entry.file_type ? "Hidden" : "Regular");
        printf("  Size: %u bytes\n", entry.file_size);
        printf("  First block address: %u\n", entry.first_block_addr);
        // printf("  Name type: %s\n", entry.name_type ? "Addressable" : "Inline");
        char name[MAX_FILENAME_BYTES] = {0};
        strncpy(name, map_uint_to_str(entry.name_addr), MAX_FILENAME_BYTES);
        printf("  Name: %s\n", name);
    }
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

    unsigned data_per_block = BLOCK_SIZE - 8; // Odejmij miejsce na wskaźnik
    unsigned required_blocks = (file_size + data_per_block - 1) / data_per_block;

    printf("Required blocks: %u\n", required_blocks);

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

    // save file to disk
    unsigned first_block = header.first_block_addr;
    unsigned first_free_block = header.first_free_block;
    unsigned current_block = first_free_block;
    unsigned copy_current_block = first_free_block;
    unsigned next_free_block = 0;
    // unsigned file_size_copy = file_size;

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

    // update catalog
    unsigned file_index = header.file_count;
    catalog[file_index].file_type = 0;
    catalog[file_index].file_size = file_size;
    catalog[file_index].first_block_addr = first_free_block;
    catalog[file_index].name_addr = map_name_to_uint(filename);

    // update header
    header.file_count++;
    header.first_free_block = current_block;

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


int main() {
    create_virtual_disk("disk", 4096 * 16);
    display_disk("disk");
    copy_file_to_disk("disk", "a.s");
    display_disk("disk");
    show_chars("disk", "chars.txt");
    copy_file_to_disk("disk", "w.s");
    display_disk("disk");
    show_chars("disk", "chars1.txt");
    copy_file_to_disk("disk", "b.s");
    display_disk("disk");
    show_chars("disk", "chars2.txt");
    return 0;
}