#/bin/bash

repeat_num=3
db_names=(
  "lock_stl"
  "tbb_rand"
  "tbb_scan"
)

trap 'kill $(jobs -p)' SIGINT

if [ $# -ne 1 ]; then
  echo "Usage: $0 [dir of workload specs]"
  exit 1
fi

workload_dir=$1

for file_name in $workload_dir/workload*.spec; do
  for ((tn=1; tn<=8; tn=tn*2)); do
    for db_name in ${db_names[@]}; do
      for ((i=1; i<=repeat_num; ++i)); do
        echo "Running $db_name with $tn threads for $file_name"
        ./ycsbc -db $db_name -threads $tn -P $file_name 2>>ycsbc.output &
        wait
      done
    done
  done
done

