#! /bin/bash
procs=$(ps aux | grep "./$1" | grep -v "grep" | awk '{print $2}' | tr '\n' ' ' | awk '{$1=$1};1')

if [[ -n "$procs" ]]; then
  kill $procs
  echo killed
fi
