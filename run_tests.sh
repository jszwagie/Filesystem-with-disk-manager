#!/bin/sh

rm output.txt 2> /dev/null

# Add permissions
chmod +x test_filesystem.sh
chmod +x prepare_files.sh

# Run the script and redirect output to a file
./prepare_files.sh
./test_filesystem.sh

# Remove files:
rm .hidden_file a.s b.s test_file.txt alice_has_a_cat.txt lorem_ipsum.txt "Very long name with spaces.txt"
