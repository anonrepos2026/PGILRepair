#!/bin/bash

# fileName & number setting
read -p "name: " NAME
read -p "num: " NUM

for (( i=0; i<NUM ;i++ )); do
  echo "[INFO] Starting Simulation ${i}"
  if (( i % 5 == 0 )); then
    ./restart-dfs.sh
  fi

  LAST_IP="TMP"
  # BW clear
  ./wondershaper_clear.sh
  rm bw_snapshot.txt
  for (( j=0; j<5 ;j++ )); do

    # openec restart
    python script/stop.py
    python script/start.py

    # Selecting Algo
    ALGO="TMP"
    if (( j == 0 )); then
      ALGO="rs_prop_rand"
    elif (( j == 1 )); then
      ALGO="rs_14_10"
    elif (( j == 2 )); then
      ALGO="rs_pipe"
    elif (( j == 3 )); then
      ALGO="rs_ppr"
    else
      ALGO="rs_pivot"
    fi
    # Excute write
    ssh hadoop-slave1 "cd /home/openec/openec-v1.0.0 && ./OECClient write README.md /${NAME}${i}${j} ${ALGO} online 26"

    echo "[INFO] Writing.."
    if (( j == 0 )); then
      # wait for the BW write
      echo "[INFO] Waiting for BW record.. 20s"
      sleep 20

      # limit BW
      OUTPUT=$(./bandwidth_limit.sh)

      python update_bandwidth.py

      # Get Last IP
      #LAST_IP=$(echo "$OUTPUT" | grep -oE '([0-9]{1,3}\.){3}[0-9]{1,3}' | tail -n 1)
      LAST_IP=$(echo "$OUTPUT" | \
        awk '/Block/ {
          block=$3
          ip=$6
          gsub(",", "", ip)
          print block, ip
        }' | sort -nr | head -n 1 | awk '{print $2}')

      if [ -z "$LAST_IP" ]; then
        echo "[ERROR] IP error"
        exit 1
      fi
    else
      sleep 40
    fi

    echo "[INFO] LAST IP: $LAST_IP"

    # Delete packet in LAST_IP's Node
    ssh "${LAST_IP}" '
      cd "/home/openec/hadoop-3.0.0-src/hadoop-dist/target/hadoop-3.0.0/dfs/data/current" || exit 1
      echo "[CHECK] Hadoop path Clear"

      BP_DIR=$(ls -dt BP* 2>/dev/null | head -n 1)
      if [ -z "$BP_DIR" ]; then
        echo "[ERROR] BP* exist X"
        exit 1
      fi
      echo "[CHECK] BP path Clear"

      cd "$BP_DIR/current/finalized" || exit 1
      D1=$(ls -dt */ 2>/dev/null | head -n 1)
      if [ -z "$D1" ]; then
        echo "[ERROR] Finalized X"
        exit 1
      fi
      cd "$D1" || exit 1
      echo "[CHECK] D1 path Clear"

      D2=$(ls -dt */ 2>/dev/null | head -n 1)
      if [ -z "$D2" ]; then
        echo "[ERROR] 2nd directory x"
        exit 1
      fi
      cd "$D2" || exit 1
      echo "[CHECK] D2 path Clear"

      FILE=$(ls -t 2>/dev/null | head -n 1)
      if [ -z "$FILE" ]; then
        echo "[ERROR] File x"
        exit 1
      fi
      echo "[CHECK] File Clear"

      rm -f "$FILE" || { echo "file delete x"; exit 1; }
      cd /home/openec/openec-v1.0.0 || exit 1

      echo "[INFO] Starting Repair"
      ./OECClient startRepair
    '
    echo "[INFO] Waiting for repair... 30s"

    sleep 60

    # Get a repair Time
    LAST_TIME=$(ssh "$LAST_IP" "tail -n 3 /home/openec/openec-v1.0.0/agent_output | head -n 1 | awk '{print \$NF}'")

    if [ -z "$LAST_TIME" ]; then
      echo "[ERROR] writeFileTime x"
      exit 1
    fi

    # Record
    echo "[INFO] Time recorded"
    echo "$NAME $i $j $LAST_IP $LAST_TIME" >> high.log

    if (( i != NUM-1 )); then
      echo "[INFO] Preparing Next Turn.. 20s"
      sleep 40
    fi
  done
done
