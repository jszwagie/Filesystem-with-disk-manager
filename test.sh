#!/bin/sh

rm output.txt 2> /dev/null

# Dodaj uprawnienia
chmod +x test_file.sh
chmod +x test_terminal.sh
chmod +x prepare_files.sh

# Uruchom skrypt i przekieruj wyjście do pliku
./prepare_files.sh
./test_file.sh

# Usuń pliki:
rm .plik_ukryty a.s b.s minix_file.txt ala_ma_kota.txt lorem_ipsum.txt "plik z długą nazwą.txt"
