Codes carry out evolutionary learning of neural-network protocols for memory erasure [ref to be added]

I run by compiling taskfarmer

g++ -Wall -o deschamps taskfarmer_info_erasure.cpp -lm -O

and then running

sbatch deschamps.sh

You'll have to change the cluster-specific commands in taskfarmer_info_erasure.c and deschamps.sh to work on your cluster.

The flow of the code info_erasure.c can be understood by starting from the main() function.



Steps:

0. connect to ssh drp9@cedar.computecanada.ca (password: Dharmik@16902) 
1. git clone https://github.com/dharmik16902/Mitacs_gitcodes.git
2. change the trajectory time in file "info_erasure.cpp" (can use vim command on terminal to do it)
3. g++ -Wall -o deschamps taskfarmer_info_erasure.cpp -lm -O
4. sbatch deschamps.sh
