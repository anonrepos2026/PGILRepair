#!/bin/bash
for i in {22..35}; do
  scp -r src 192.168.10.$i:/home/openec/openec-v1.0.0
  #scp -r conf/sysSetting.xml 192.168.10.$i:/home/openec/openec-v1.0.0/conf
done
