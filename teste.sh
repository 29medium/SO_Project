./cliente -e "ls"
sleep 1
./cliente -e "ls -l | wc"
sleep 1
./cliente -e "tail -f /dev/null"
sleep 1
./cliente -e "ls | sleep 5 | wc"
sleep 1
./cliente -i 2
sleep 1
./cliente -e "ls | sleep 3"
sleep 1
./cliente -m 4
sleep 1
./cliente -t 3
sleep 1
./cliente -o 1
sleep 0.5
./cliente -e "ls | sleep 5"
sleep 0.5
./cliente -e "tail -f /dev/null"
sleep 0.5
./cliente -e "cut -f7 -d: /etc/passwd | uniq | wc -l"
sleep 5
./cliente -r
sleep 0.5
./cliente -l
sleep 0.2
./cliente -o 1
sleep 0.2
./cliente -o 2
sleep 0.2
./cliente -o 6
