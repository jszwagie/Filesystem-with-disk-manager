#!/bin/sh

rm output.txt 2> /dev/null

# Dodaj uprawnienia
chmod +x test_f.sh
chmod +x test_t.sh
chmod +x p_files.sh

# Uruchom skrypt i przekieruj wyjście do pliku
./p_files.sh
./test_f.sh

# Usuń pliki:
rm .plik_ukryty a.s b.s minix_file.txt ala_ma_kota.txt lorem_ipsum.txt "Długa nazwa.txt"
