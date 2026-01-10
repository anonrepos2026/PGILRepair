#!/bin/bash
stop-dfs.sh
rm -rf $HADOOP_HOME/dfs

for i in {22..35}; do
  ssh 192.168.10.$i "rm -rf $HADOOP_HOME/dfs"
done

hdfs namenode -format
start-dfs.sh
