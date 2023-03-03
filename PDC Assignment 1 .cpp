#include <string.h>
#include <stdio.h>
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

int main(int argc, char** argv) {

	MPI_Init(&argc, &argv);
	int myrank, nprocs, namelen;
	char processorName[10];
	char data[12];
	int count=0;

	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Get_processor_name(processorName, &namelen);
	
	char input_data[]="1357924680";
	//SAME LENGTH ARRAYS FOR BOTH PROCESSES
	//FIRST INDEX HAS NUMBER TO SEARCH AND ITTERATIONS WILL START FROM THE NEXT NUMBER
	char P1_data[] = "013579";
	char P2_data[] = "024680";
	char abort = 'A';

	if (myrank == 0) { 

		
		cout<<"Master : The Number to search is 0."<<endl;
		cout<<"Process 0 has input data = "<<input_data<<endl;
		cout<<"Process 1 has local data = "<<P1_data<<endl;
		cout<<"Process 2 has local data = "<<P2_data<<endl;
		MPI_Send(P1_data, 6, MPI_CHAR, 1, 999, MPI_COMM_WORLD); //TAG 999
		MPI_Send(P2_data, 6, MPI_CHAR, 2, 999, MPI_COMM_WORLD);

		MPI_Recv(data, 6, MPI_CHAR, 1, 800, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//TAG 800 FOR REPLY
		MPI_Recv(data, 6, MPI_CHAR, 2, 800, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//TAG 800 FOR REPLY
		if(data[0]=='1'){ // 1 Has found the number, send stop to 2
			
			cout<<"Master : Process 1 has found the Number!."<<endl;
			cout<<"Master : Informing all Process to abort."<<endl;
			MPI_Send(strcpy(data, "A"), 6, MPI_CHAR, 2, 999, MPI_COMM_WORLD);
		
		}
		if(data[0]=='2'){
	
			cout<<"Master : Process 2 has found the Number!."<<endl;
			cout<<"Master : Informing all Process to abort."<<endl;
			MPI_Send(strcpy(data, "A"), 6, MPI_CHAR, 1, 999, MPI_COMM_WORLD);
		
		}
	}
	else {
		pragma omp parallel num_threads(1) 
		{
		MPI_Recv(data, 6, MPI_CHAR, 0, 999, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		cout<<"My rank"<<myrank<<endl;
		if(data[0]=='A')
		cout<<"Process "<<myrank<<": Aborting Search"<<endl;

		//#pragma omp for schedule(static, 3) //Divide the iterations between threads
		for(int x = 1; x < 6; x++){ //Iterating from the second Number Because First has the Number to be Searched
		
			if (data[0]==data[x])
				MPI_Send(&myrank, 1, MPI_CHAR, 0, 800, MPI_COMM_WORLD);
				
		}
	
		}
	}
    
    MPI_Finalize();
    return 0;
}
