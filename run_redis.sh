#/bin/bash

trap 'kill $(jobs -p)' SIGINT

workloads="./workloads/workloada.spec ./workloads/workloadb.spec ./workloads/workloadd.spec ./workloads/workloadf.spec"

for file_name in $workloads; do
  echo "Running Redis with for $file_name"
  ./ycsbc -db redis -threads 1 -P $file_name -host 127.0.0.1 -port 6379 -slaves 0 2>>ycsbc.output &
  wait
done

