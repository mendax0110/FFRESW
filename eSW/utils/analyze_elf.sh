#!/bin/bash

# Usage:
# ./analyze_elf.sh /path/to/file.elf /path/to/output_dir [--open]

ELF_FILE="$1"
OUTPUT_DIR="$2"
OPEN_RESULT="$3"

AVR_BIN_GITBASH="C:/Sloeber/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin"
AVR_BIN_WSL="/mnt/c/Sloeber/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin"

if grep -qi microsoft /proc/version 2>/dev/null; then
    AVR_BIN="$AVR_BIN_WSL"
    ELF_FILE_WIN=$(wslpath -w "$ELF_FILE")
else
    AVR_BIN="$AVR_BIN_GITBASH"
    ELF_FILE_WIN="$ELF_FILE"
fi

if [ ! -f "$ELF_FILE" ]; then
  echo "[ERROR] ELF file not found: $ELF_FILE"
  exit 1
fi

BASE_NAME=$(basename "$ELF_FILE" .elf)
DUMP_FILE="$OUTPUT_DIR/${BASE_NAME}_dump.txt"
SECTIONS_FILE="$OUTPUT_DIR/${BASE_NAME}_sections.txt"
SYMBOLS_FILE="$OUTPUT_DIR/${BASE_NAME}_symbol_sizes.txt"
USAGE_FILE="$OUTPUT_DIR/${BASE_NAME}_usage_summary.txt"

mkdir -p "$OUTPUT_DIR"

"$AVR_BIN/avr-objdump.exe" -h -S "$ELF_FILE_WIN" > "$DUMP_FILE"
"$AVR_BIN/avr-objdump.exe" -h "$ELF_FILE_WIN" > "$SECTIONS_FILE"
"$AVR_BIN/avr-nm.exe" --size-sort --numeric-sort --print-size "$ELF_FILE_WIN" > "$SYMBOLS_FILE"

get_section_size() {
    grep -E "^\s*[0-9]+\s+$1" "$SECTIONS_FILE" | awk '{printf "%d", "0x"$2}'
}

TEXT_SIZE=$(get_section_size ".text")
DATA_SIZE=$(get_section_size ".data")
BSS_SIZE=$(get_section_size ".bss")
NOINIT_SIZE=$(get_section_size ".noinit")

FLASH_USAGE=$((TEXT_SIZE + DATA_SIZE))
RAM_USAGE=$((DATA_SIZE + BSS_SIZE + NOINIT_SIZE))

{
  echo "Flash usage (.text + .data): ${FLASH_USAGE} bytes"
  echo "RAM usage (.data + .bss + .noinit): ${RAM_USAGE} bytes"
  echo "  .data: ${DATA_SIZE} bytes"
  echo "  .bss: ${BSS_SIZE} bytes"
  echo "  .noinit: ${NOINIT_SIZE} bytes"
} > "$USAGE_FILE"

echo "[SUCCESS] Analysis complete!"
echo "  Disassembly: $DUMP_FILE"
echo "  Sections: $SECTIONS_FILE"
echo "  Symbol sizes: $SYMBOLS_FILE"
echo "  Usage summary: $USAGE_FILE"

if [ "$OPEN_RESULT" == "--open" ]; then
  explorer.exe "$(wslpath -w "$OUTPUT_DIR")"
fi
