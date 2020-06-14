./argus -e "ls"
sleep 1
./argus -e "ls -l | wc"
sleep 1
./argus -e "tail -f /dev/null"
sleep 1
./argus -e "ls | sleep 5 | wc"
sleep 1
./argus -i 2
sleep 1
./argus -e "ls | sleep 3"
sleep 1
./argus -m 4
sleep 1
./argus -t 3
sleep 1
./argus -o 1
sleep 0.5
./argus -e "ls | sleep 5"
sleep 0.5
./argus -e "tail -f /dev/null"
sleep 0.5
./argus -e "cut -f7 -d: /etc/passwd | uniq | wc -l"
sleep 5
./argus -r
sleep 0.5
./argus -l
sleep 0.2
./argus -o 1
sleep 0.2
./argus -o 2
sleep 0.2
./argus -o 8
