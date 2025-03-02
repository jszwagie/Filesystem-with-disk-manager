# Disk Manager Project

This project implements a virtual disk manager in C, which allows for the creation, mounting, and manipulation of a virtual disk. The disk manager supports various operations such as copying files to and from the virtual disk, listing files, and removing files. Additionally, it includes a comprehensive test suite to verify the functionality of the disk manager. The disk manager is written in ANSI C.

## Files

- disk_manager.c: The main source code file containing the implementation of the virtual disk manager.
- test_filesystem.sh: A shell script to test the functionality of the disk manager.
- prepare_files.sh: A shell script to create test files used in the test suite.
- run_tests.sh: A shell script to prepare test files, run the test suite, and clean up.


## Compilation

To compile the disk manager, run the following command:

```sh
gcc -std=c89 -o disk_manager disk_manager.c
```

## Usage

The disk manager supports the following commands:

- `create <diskname> <size>`: Create a virtual disk with the specified name and size.
- `cp <-in/-out> <filename>`: Copy a file to/from the virtual disk.
- `rm <filename>`: Remove a file from the virtual disk.
- `mount <diskname>`: Mount the virtual disk.
- `unmount`: Unmount the virtual disk.
- `info`: Display information about the virtual disk.
- `ls <-a>`: List files on the virtual disk. Use the `-a` option to include hidden files.
- `man`: Display the manual with possible commands.

### Example Commands

1. **Create a virtual disk**:
   ```sh
   ./disk_manager create test_disk.img 409600
   ```

2. **Mount the virtual disk**:
   ```sh
   ./disk_manager mount test_disk.img
   ```

3. **Copy a file to the virtual disk**:
   ```sh
   ./disk_manager cp -in test_file.txt
   ```

4. **List files on the virtual disk**:
   ```sh
   ./disk_manager ls
   ```

5. **Copy a file from the virtual disk**:
   ```sh
   ./disk_manager cp -out test_file.txt
   ```

6. **Remove a file from the virtual disk**:
   ```sh
   ./disk_manager rm test_file.txt
   ```

7. **Unmount the virtual disk**:
   ```sh
   ./disk_manager unmount
   ```

8. **Display disk information**:
   ```sh
   ./disk_manager info
   ```

## Testing

The project includes a comprehensive test suite to verify the functionality of the disk manager. To run the tests run this instruction:

```sh
./run_tests.sh
```

The test results will be saved in **output.txt**.
