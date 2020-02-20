/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include "common.h"
#include "FIFOreqchannel.h"
#include <fstream>
#include <string>
#include <sys/wait.h>

using namespace std;


int main(int argc, char *argv[]){
    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
    srand(time_t(NULL));

    int opt;
    int patient = -1;
    int ecg = -1;
    double time = -1;
    string file = "";

	while ((opt = getopt(argc, argv, "p: t: e: f:")) != -1) {
		switch (opt) {
			case 'p':
				patient = atoi (optarg);
                cout << "Patient Input: " << patient << endl;
				break;
            
            case 't':
				time = atof (optarg);
                cout  << "Time Input: " << time <<  endl;
				break;

            case 'e':
                ecg = atoi (optarg);
                cout << "ECG Number: " << ecg << endl;
                break;

            case 'f':
                file = optarg;
                cout << "Inputted Filename: " << file << endl;

            case '?':
                cout << "Unknown input" << endl;
                break;
		}
	}

    char* args[2] = {"./server", NULL};

    if(fork() == 0 )
    {
        execv(args[0], args);
    }

    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);

    // PART 1 ====================================================================================

    //output for a single data point
    if (patient != -1 && time != -1 && ecg != -1 && file == "")
    {
        //getting a single Data point
        char buf[MAX_MESSAGE];
        datamsg d(patient, time, ecg);
        d.mtype = DATA_MSG;

        chan.cwrite(&d, sizeof(d));
        int nbits = chan.cread(buf, MAX_MESSAGE);
        //outputing the single data point received from the server
        cout << *(double*)buf << endl;
    }


    if (patient == -1 && time == -1 && ecg == -1 && file == "")
    {
        datamsg d(patient = 1, time = 0.000, ecg = 1);
        int nbits;
        char buf[MAX_MESSAGE];

        struct timeval start, end;
        //writing all values from patient one to a file
        ofstream file;
        file.open("./received/x1.csv");

        gettimeofday(&start, NULL);
        for (time; time <= 59.996; time+= .004)
        {
            file << time << ",";
            d.seconds = time;
            for(int i = 1; i <= 2; i++)
            {
                d.ecgno = i;
               chan.cwrite(&d, sizeof(d));
               nbits = chan.cread(buf, MAX_MESSAGE);
               if(i == 1)
               {
                   file << *(double*) buf << ",";
                }
                else
             {
                    file << *(double*) buf << endl;
                }
            
            }
        }
        gettimeofday(&end, NULL);
        file.close();

        //output the time it took to process
        double timems = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) * 1e-6);
        cout << "Time taken to copy patient1's info was: " << timems << endl;
    }

    //TASK 2 ==================================================================
    if (file != "")
    {
        struct timeval start, end;
        const char* filechar = file.c_str();

        //to get the length of the file
        filemsg* msg = new filemsg(0, 0);

        //make the buffer
        char* buf = (char *) malloc(sizeof(filemsg) + file.size() + 1);

        memcpy(buf, msg, sizeof(filemsg));
        strcpy(buf + sizeof(filemsg), filechar);

        chan.cwrite(buf, sizeof(filemsg) + file.size() + 1);
        int nbytes = chan.cread(buf, sizeof(__int64_t));
        
        cout << "Length of the file is: " << *(__int64_t*)buf << endl;

        __int64_t size = *(__int64_t*)buf;

        ofstream outputfile;
        outputfile.open("./received/patient" + file, ios::binary);

        int buffsize = 256;
        __int64_t offset = 0;
        for(int i = 0; i <= size/buffsize; i++)
        {
            cout << "The Current Size Left: " << size - offset << endl;

            
        }
    }


    // closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));

    wait(NULL);
}
