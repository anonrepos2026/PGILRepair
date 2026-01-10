#!/bin/bash

COOR_LOG="coor_output"
INTERFACE="eth0"

UL=()
DL=()
while read -r line; do
    if [[ "$line" =~ ^UL:\ (.*) ]]; then
        IFS=' ' read -r -a UL <<< "${BASH_REMATCH[1]}"
    fi
    if [[ "$line" =~ ^DL:\ (.*) ]]; then
        IFS=' ' read -r -a DL <<< "${BASH_REMATCH[1]}"
    fi
done < "$COOR_LOG"

declare -A block_ip_map
current_block=""

while read -r line; do
    if [[ "$line" =~ filename:\ .*_oecobj_([0-9]+) ]]; then
        current_block="${BASH_REMATCH[1]}"
    fi
    if [[ "$line" =~ getLocation.return:\ ([0-9.]+) ]]; then
        ip="${BASH_REMATCH[1]}"
        block_ip_map["$current_block"]="$ip"
    fi
done < "$COOR_LOG"

for block in "${!block_ip_map[@]}"; do
    ip="${block_ip_map[$block]}"
    ul="${UL[$block]}"
    dl="${DL[$block]}"
    if [[ -n "$ip" && -n "$ul" && -n "$dl" ]]; then
        ul_kbps=$((ul * 1024))
        dl_kbps=$((dl * 1024))
        echo "[INFO] Block $block -> IP $ip, UL=$ul DL=$dl"
        ssh "$ip" "sudo wondershaper -a $INTERFACE -u $ul_kbps -d $dl_kbps"
    else
        echo "[WARN] Block $block has missing UL/DL/IP mapping"
    fi
done

BW_TXT="bw_snapshot.txt"
> $BW_TXT

for block in $(seq 0 13); do
  echo "UL[$block]=${UL[$block]} DL[$block]=${DL[$block]}" >> $BW_TXT
done
