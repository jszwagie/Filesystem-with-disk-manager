#!/bin/sh

# Kompilacja programu
gcc -o disk_manager disk_manager.c

# Ścieżka do menedżera dysku
DISK_MANAGER=./disk_manager

# Nazwa i rozmiar wirtualnego dysku
DISK_NAME=test_disk.img
DISK_SIZE=409600 # 400 KB

# Pliki testowe
TEST_FILE_IN=minix_file.txt
TEST_FILE_OUT=ala_ma_kota.txt
LONG_NAME_FILE="plik z długą nazwą.txt"
LONG_FILE="lorem_ipsum.txt"
DOT_FILE=".plik_ukryty"

# Plik, do którego przekierujemy całe wyjście
OUTPUT_FILE="output.txt"
OUTPUT_FILE_COMMANDS="output.txt"


# 1. Tworzenie wirtualnego dysku

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Tworzenie wirtualnego dysku o rozmiarze 400 KB..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER create $DISK_NAME $DISK_SIZE" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER create $DISK_NAME $DISK_SIZE >> $OUTPUT_FILE 2>&1
echo "ls" >> $OUTPUT_FILE_COMMANDS 2>&1
ls >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 2. Montowanie wirtualnego dysku

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Montowanie wirtualnego dysku..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER mount $DISK_NAME" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER mount $DISK_NAME >> $OUTPUT_FILE 2>&1
echo "cat config.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
cat config.txt >> $OUTPUT_FILE 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 3. Kopiowanie pliku z systemu Minix na dysk wirtualny

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Kopiowanie pliku na dysk wirtualny..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in $TEST_FILE_IN" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in $TEST_FILE_IN >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Informacje o dysku
echo "Informacje o dysku po kopiowaniu pliku:" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 4. Obsługa długich nazw ze spacjami

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testowanie obsługi długich nazw ze spacjami..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in \"$LONG_NAME_FILE\"" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in "$LONG_NAME_FILE" >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Informacje o dysku
echo "Informacje o dysku po teście długich nazw:" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 5. Polecenie ls

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testowanie polecenia ls..." >> $OUTPUT_FILE_COMMANDS 2>&1
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


# 6. Kopiowanie pliku z dysku wirtualnego na dysk systemu Minix

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Kopiowanie pliku z dysku wirtualnego..." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in ala_ma_kota.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in ala_ma_kota.txt >> $OUTPUT_FILE 2>&1
echo "mv ala_ma_kota.txt ala_ma_kota_old.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
mv ala_ma_kota.txt ala_ma_kota_old.txt
echo "$DISK_MANAGER cp -out ala_ma_kota.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -out ala_ma_kota.txt >> $OUTPUT_FILE 2>&1
echo "Porównanie plików:" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "---------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "diff ala_ma_kota.txt ala_ma_kota_old.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
diff ala_ma_kota.txt ala_ma_kota_old.txt >> $OUTPUT_FILE 2>&1
echo "cat ala_ma_kota.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
cat ala_ma_kota.txt >> $OUTPUT_FILE 2>&1
echo "cat ala_ma_kota_old.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
cat ala_ma_kota_old.txt >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 7. Kopiowanie długiego pliku

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testowanie kopiowania długich plików.." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in lorem_ipsum.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in lorem_ipsum.txt >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "mv lorem_ipsum.txt lorem_ipsum_old.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
mv lorem_ipsum.txt lorem_ipsum_old.txt
echo "$DISK_MANAGER cp -out lorem_ipsum.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -out lorem_ipsum.txt >> $OUTPUT_FILE 2>&1
echo "Porównanie plików:" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "diff lorem_ipsum.txt lorem_ipsum_old.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
diff lorem_ipsum.txt lorem_ipsum_old.txt >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 8. Usuwanie pliku z wirtualnego dysku

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Usuwanie pliku z dysku wirtualnego.." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER ls" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER rm $TEST_FILE_IN" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm $TEST_FILE_IN >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER ls" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 9. Obsługa plików kropkowych

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testowanie obsługi plików ukrytych.." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in \"$DOT_FILE\"" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in "$DOT_FILE" >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER ls" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER ls -a" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER ls -a >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 10. Nieciągła alokacja

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testowanie nieciągłej alokacji.." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Usuwanie kilku plików.." >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm lorem_ipsum.txt >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm .plik_ukryty >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm a.s >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm b.s >> $OUTPUT_FILE 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

echo "Kopiowanie pliku na dysk wirtualny.." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER cp -in lorem_ipsum.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER cp -in lorem_ipsum.txt >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# 11. Testowanie bloków nazw

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Testowanie bloków nazw.." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Usuwanie plików z dysku wirtualnego.." >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm "plik z długą nazwą.txt" >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm "ala_ma_kota.txt" >> $OUTPUT_FILE 2>&1
$DISK_MANAGER rm "lorem_ipsum.txt" >> $OUTPUT_FILE 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Tworzenie 64 plików o długiej nazwie na dysku lokalnym
for i in `seq 1 64`
do
    echo "test bloków nazw $i" > "długa_nazwa_pliku_$i.txt"
done
echo "Wstawianie 64 plików o długiej nazwie i długości 1 bloku.." >> $OUTPUT_FILE_COMMANDS 2>&1
for i in `seq 1 64`
do
    $DISK_MANAGER cp -in "długa_nazwa_pliku_$i.txt" >> $OUTPUT_FILE 2>&1
done
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Usunięcie ostatniego pliku
echo "Usunięcie ostatniego pliku.." >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm "długa_nazwa_pliku_64.txt" >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1


# Usunięcie pliku ze środka
echo "Usunięcie pliku ze środka.." >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER rm "długa_nazwa_pliku_32.txt" >> $OUTPUT_FILE 2>&1
echo "$DISK_MANAGER info" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER info >> $OUTPUT_FILE 2>&1


# 12. Usuwanie wirtualnego dysku

echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "Usuwanie wirtualnego dysku.." >> $OUTPUT_FILE_COMMANDS 2>&1
echo "" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1
echo "$DISK_MANAGER unmount" >> $OUTPUT_FILE_COMMANDS 2>&1
$DISK_MANAGER unmount >> $OUTPUT_FILE 2>&1
echo "cat config.txt" >> $OUTPUT_FILE_COMMANDS 2>&1
cat config.txt >> $OUTPUT_FILE 2>&1
echo "--------------------------------------------------" >> $OUTPUT_FILE_COMMANDS 2>&1

# Usunięcie plików tymczasowych

for i in `seq 1 64`
do
    rm "długa_nazwa_pliku_$i.txt"
done
rm ala_ma_kota_old.txt lorem_ipsum_old.txt
