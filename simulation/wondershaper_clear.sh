#!/bin/bash
for i in {22..35}; do
  ssh 192.168.10.$i wondershaper -c -a eth0
done
