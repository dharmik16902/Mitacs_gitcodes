
#include <unistd.h>
# include <stdio.h>                             /* C standard input and output */
# include <stdlib.h>                            /* standard library */
# include <math.h>                              /* math library */
# include <iostream>
# include <iomanip>
# include <time.h>

#include <fstream>

using namespace std;
using std::cout;
using std::endl;
using std::cin;

#include <fstream>
using std::ifstream;
#include <cstdlib>


char st[256];
long int rn_seed;
long ltime = time(NULL);
long int rn_seed2 = ltime;
long int time_last_launch; // DP steve safeguard changes
long int time_limit = 24 * 60 * 60;  // DP steve safeguard changes

// DP change
const int n_traj=50;
const int parents=5;
// const int n_traj=10;
// const int parents=2;

int parent_list[parents];
int generation=0;

double phi_value[n_traj];

//functions void
void setup(void);
void relaunch(int status);
void jobcomplete(int i);
void determine_parents(void);

//functions int
int q_jobs_done(void);

//functions bool
bool q_file_exist(const char *fileName);

bool fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

int main(void){

//seed RN generator
srand48(rn_seed2);

//setup
bool ifFileExists = false; 
ifFileExists = fileExists("iteration_0/input_parameters.dat");

    
if(ifFileExists == false){
setup();
}
    
else{
    
sprintf(st,"iteration_0/input_parameters.dat");
ifstream input_params(st, ios::in);
int temp_seed, latest_generation_to_finish;
input_params >> temp_seed >> latest_generation_to_finish; 
    
generation = latest_generation_to_finish;
relaunch(1);
}
 
//GA
while(2>1){

if(q_jobs_done()==1){
determine_parents();
relaunch(0);
}

sleep(30);

}
}


void setup(void){

int n;

long int rn_seed;
 
//store data
sprintf(st,"mkdir genome");
system(st);

//loop through parameter cases
for(n=0;n<n_traj;n++){

//directory
sprintf(st,"mkdir iteration_%d",n);
system(st);

//parameters for etna
sprintf(st,"swarm_%d.sh",n);
ofstream soutput(st,ios::out);

soutput << "#!/usr/bin/env bash" << endl;
sprintf(st,"#SBATCH --job-name=swarm_%d",n);
soutput << st << endl;
// soutput << "#SBATCH --partition=def-dsivak" << endl;
// soutput << "#SBATCH --partition=etna" << endl;
soutput << "#SBATCH --account=def-dsivak" << endl;
soutput << "#SBATCH --qos=normal" << endl;
sprintf(st,"#SBATCH --nodes=%d",1);
soutput << st << endl;
soutput << "#SBATCH --time=24:00:00" << endl;
soutput << "#SBATCH --mem=1000mb" << endl;
sprintf(st,"./swarm_%d",n);
soutput << st << endl;
soutput.close();
 
//copy executable
sprintf(st,"mv swarm_%d.sh iteration_%d",n,n);
system(st);

rn_seed = (int) (drand48()*2000000);

ofstream output("input_parameters.dat",ios::out);
output << rn_seed << " " << generation << endl;

sprintf(st,"cp input_parameters.dat iteration_%d",n);
system(st);

//copy code
// sprintf(st,"cp info_erasure.c iteration_%d",n); // DP
sprintf(st,"cp info_erasure.cpp iteration_%d",n);
system(st);

//copy file
jobcomplete(0);
sprintf(st,"mv jobcomplete.dat iteration_%d",n);
system(st);

//compile code
// sprintf(st,"cd iteration_%d; g++ -Wall -mcmodel=medium -o swarm_%d info_erasure.c -lm -O",n,n); system(st); DP
sprintf(st,"cd iteration_%d; g++ -Wall -o swarm_%d info_erasure.cpp -lm -O",n,n); system(st);

cout << st << endl;                                                               
 
sprintf(st,"cd iteration_%d; sbatch swarm_%d.sh",n,n); system(st); 
// sprintf(st,"cd iteration_%d; ./swarm_%d",n,n); system(st);
// DP steve safeguard changes, pause
sleep(2);
}
time_last_launch=time(NULL);
}



void jobcomplete(int i){

 //sprintf(st,"rm jobcomplete.dat");
 //system(st);
 
 sprintf(st,"jobcomplete.dat");
 ofstream output_job(st,ios::out);
 output_job << i << endl;
 output_job.close();

}


int q_jobs_done(void){

int i;
int q1=0;
int flag=0;
int counter=0;

for(i=0;i<n_traj;i++){

sprintf(st,"iteration_%d/jobcomplete.dat",i);
ifstream infile(st, ios::in);
infile >> flag;

if(flag==1){counter++;}
flag=0;

}

// cout << " gen " << generation << " counter " << counter << endl;
cout << " gen " << generation << " counter " << counter << " time elapsed " << time(NULL)-time_last_launch << endl; // DP steve safeguard changes 
if(counter==n_traj){q1=1;}
else{q1=0;}

//DP steve safeguard changes 
if((time(NULL)-time_last_launch>time_limit) && (counter>n_traj/2)){cout << " time limit exceeded " << endl;q1=1;}


return (q1);

}


bool q_file_exist(const char *fileName){
  
  std::ifstream infile(fileName);
  return infile.good();
 
 }

void determine_parents(void){

int i;
int j;
int p1;
int top_parent=-1;

double phi_min=1e5;
double phi_dummy=1e5;
double mean_p[n_traj];
double mean_wd[n_traj];
double mean_he[n_traj];

//output files

sprintf(st,"output_phi_min.dat");
ofstream output_phi_min(st,ios::app);

sprintf(st,"output_mean_p.dat");
ofstream output_mean_p(st,ios::app);

sprintf(st,"output_mean_wd.dat");
ofstream output_mean_wd(st,ios::app);

sprintf(st,"output_mean_he.dat");
ofstream output_mean_he(st,ios::app);

sprintf(st,"output_phi_list_gen_%d.dat",generation);
ofstream output_phi_list(st,ios::app);

//read phi values
for(i=0;i<n_traj;i++){

//input file 1
sprintf(st,"iteration_%d/report_phi_gen_%d.dat",i,generation);
if(q_file_exist(st)==1){
ifstream infile(st, ios::in);
while (!infile.eof()){infile >> phi_value[i];}}

//input file 2
sprintf(st,"iteration_%d/report_order_parameters_gen_%d.dat",i,generation);
if(q_file_exist(st)==1){
ifstream infile2(st, ios::in);
while (!infile2.eof()){infile2 >> mean_p[i] >> mean_wd[i] >> mean_he[i];}}

}

//compute ordered list of parents
for(j=0;j<parents;j++){
for(i=0;i<n_traj;i++){

if(i==0){phi_min=phi_value[i];p1=i;}
else{
if(phi_value[i]<=phi_min){phi_min=phi_value[i];p1=i;}}
}

//log and output
parent_list[j]=p1;
if(j==0){
top_parent=p1;
output_phi_min << generation << " " << phi_min << endl;
output_mean_p << generation << " " << mean_p[p1] << endl;
output_mean_wd << generation << " " << mean_wd[p1] << endl;
output_mean_he << generation << " " << mean_he[p1] << endl;
}
output_phi_list << p1 << " " << phi_value[p1] << endl;
phi_value[p1]=phi_dummy;

}

//record data from top parent
sprintf(st,"cp iteration_%d/report*gen_%d* genome/.",top_parent,generation);
system(st);

//network used to produce config
sprintf(st,"cp iteration_%d/net_out_gen_%d.dat genome/.",top_parent,generation);
system(st);

//increment generation counter
generation++;

}


void relaunch(int status){
    
    int i;
    int parent_choice;
    
    long int rn_seed;
    
    //loop through parameter cases
    for(i=0;i<n_traj;i++){
            
        rn_seed = (int) (drand48()*2000000);

        if(status == 0){
         
            if(i==0){parent_choice=parent_list[0];}
            else{
                parent_choice= (int) (drand48()*parents);
                parent_choice=parent_list[parent_choice];
            }
             
            //input parameters file
            ofstream output("input_parameters.dat",ios::out);
            output << rn_seed << " " << generation << endl;
            
            //copy file
            sprintf(st,"cp input_parameters.dat iteration_%d",i);
            system(st);
            
            //copy net
            sprintf(st,"cp iteration_%d/net_out_gen_%d.dat iteration_%d/net_in_gen_%d.dat",parent_choice,generation-1,i,generation);
            system(st);
            cout << st << endl;
            
            
            
        }

        //copy file
        jobcomplete(0);
        sprintf(st,"mv jobcomplete.dat iteration_%d",i);
        system(st);
        
        sprintf(st,"cd iteration_%d; sbatch swarm_%d.sh",i,i); system(st);
        // sprintf(st,"cd iteration_%d; ./swarm_%d",i,i); system(st);
        
        //pause
        sleep(2);
    
    }
    
    time_last_launch=time(NULL);
}
