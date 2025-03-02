#!/bin/sh

# Compile the program
gcc -std=c89 -o disk_manager disk_manager.c

# Path to the disk manager
DISK_MANAGER=./disk_manager

# Name and size of the virtual disk
DISK_NAME=test_disk.img
DISK_SIZE=409600 # 400 KB

# Test files
TEST_FILE_IN=test_file.txt
TEST_FILE_OUT=alice_has_a_cat.txt
LONG_NAME_FILE="Very long name with spaces.txt"
LONG_FILE="lorem_ipsum.txt"
DOT_FILE=".hidden_file"

# File to redirect all output
OUTPUT_FILE="output.txt"
OUTPUT_FILE_COMMANDS="output.txt"


# 1. Creating a virtual disk

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Creating a virtual disk of size 400 KB..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER create $DISK_NAME $DISK_SIZE" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER create $DISK_NAME $DISK_SIZE >> $OUTPUT_FILE 2>&1
echo "ls" >> $OUTPUT_FILE_COMMANDS 2>&1
ls >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 2. Mounting the virtual disk

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Mounting the virtual disk..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER mount $DISK_NAME" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER mount $DISK_NAME >> $OUTPUT_FILE 2>&1
echo "cat config.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
cat config.txt >> $OUTPUT_FILE 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 3. Copying a file from the Minix system to the virtual disk

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Copying a file to the virtual disk..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in $TEST_FILE_IN" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in $TEST_FILE_IN >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Disk information
echo "Disk information after copying the file:" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 4. Handling long names with spaces

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testing handling of long names with spaces..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in \"$LONG_NAME_FILE\"" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in "$LONG_NAME_FILE" >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Disk information
echo "Disk information after testing long names:" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 5. ls command

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testing ls command..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER ls" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER cp -in a.s" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in a.s >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER cp -in b.s" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in b.s >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER ls" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 6. Copying a file from the virtual disk to the Minix system

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Copying a file from the virtual disk..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in alice_has_a_cat.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in alice_has_a_cat.txt >> $OUTPUT_FILE 2>&1
echo "mv alice_has_a_cat.txt alice_has_a_catc.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
mv alice_has_a_cat.txt alice_has_a_catc.txt
echo "$DISK_MANAGER cp -out alice_has_a_cat.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -out alice_has_a_cat.txt >> $OUTPUT_FILE 2>&1
echo "Comparing files:" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "diff alice_has_a_cat.txt alice_has_a_catc.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
diff alice_has_a_cat.txt alice_has_a_catc.txt >> $OUTPUT_FILE 2>&1
echo "cat alice_has_a_cat.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
cat alice_has_a_cat.txt >> $OUTPUT_FILE 2>&1
echo "cat alice_has_a_catc.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
cat alice_has_a_catc.txt >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 7. Copying a long file

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testing copying long files..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in lorem_ipsum.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in lorem_ipsum.txt >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "mv lorem_ipsum.txt lorem_ipsumc.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
mv lorem_ipsum.txt lorem_ipsumc.txt
echo "$DISK_MANAGER cp -out lorem_ipsum.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -out lorem_ipsum.txt >> $OUTPUT_FILE 2>&1
echo "Comparing files:" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "diff lorem_ipsum.txt lorem_ipsumc.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
diff lorem_ipsum.txt lorem_ipsumc.txt >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 8. Deleting a file from the virtual disk

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Deleting a file from the virtual disk..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER ls" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER rm $TEST_FILE_IN" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm $TEST_FILE_IN >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER ls" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 9. Handling dot files

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testing handling of hidden files..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in \"$DOT_FILE\"" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in "$DOT_FILE" >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER ls" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER ls -a" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls -a >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 10. Non-contiguous allocation

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testing non-contiguous allocation..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Deleting several files..." >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm lorem_ipsum.txt >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm .hidden_file >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm a.s >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm b.s >> $OUTPUT_FILE 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

echo "Copying a file to the virtual disk..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in lorem_ipsum.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in lorem_ipsum.txt >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 11. Testing name blocks

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testing name blocks..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Deleting files from the virtual disk..." >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm "file with long name.txt" >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm "alice_has_a_cat.txt" >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm "lorem_ipsum.txt" >> $OUTPUT_FILE 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Creating 64 files with long names on the local disk
for i in `seq 1 64`
do
    echo "name block test $i" > "long_file_name_$i.txt"
done
echo "Inserting 64 files with long names and length of 1 block..." >> $OUTPUT_FILE_COMMANDS 2>&1
for i in `seq 1 64`
do
    $DISK_MANAGER cp -in "long_file_name_$i.txt" >> $OUTPUT_FILE 2>&1
done
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Deleting the last file
echo "Deleting the last file..." >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm "long_file_name_64.txt" >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# Deleting a file from the middle
echo "Deleting a file from the middle..." >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm "long_file_name_32.txt" >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1


# 12. Deleting the virtual disk

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Deleting the virtual disk..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER unmount" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER unmount >> $OUTPUT_FILE 2>&1
echo "cat config.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
cat config.txt >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Deleting temporary files

for i in `seq 1 64`
do
    rm "long_file_name_$i.txt"
done
rm alice_has_a_catc.txt lorem_ipsumc.txt
