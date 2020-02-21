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
    //int n = 100;    // default number of requests per "patient"
	//int p = 15;		// number of patients
    srand(time_t(NULL));

    int opt;
    int patient = -1;
    int ecg = -1;
    double time = -1;
    string file = "";
    int c = -1;
    int n = 256;

	while ((opt = getopt(argc, argv, "p: t: e: f: c:")) != -1) {
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
                break;

            case 'c':
                c = atoi(optarg);
                cout << "Inputted New Channel Requested" << endl;
                break;   

            case 'n':
                n = atoi(optarg);
                cout << "Inputted New Max Memory Size: " << n << endl;
                break;         

            case '?':
                cout << "Unknown input" << endl;
                break;
		}
	}

    // PART 4 ===================================================================================
    char* args[2] = {"./server", NULL};

    if(fork() == 0 )
    {
        
        execv(args[0], args);
    }

    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);

    // PART 1 ====================================================================================

    //output for a single data point
    if (patient != -1 && time != -1 && ecg != -1 && file == "" && c == -1)
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


    if (patient != -1 && time == -1 && ecg == -1 && file == "" && c == -1)
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

        string fileName = file;
        const char* fileString = fileName.c_str();

        char* rA = new char[MAX_MESSAGE];

        filemsg* msg = new filemsg(0,0);

        memcpy(rA, msg, sizeof(filemsg));
        strcpy(rA + sizeof(filemsg), fileString);

        chan.cwrite(rA, sizeof(filemsg) + fileName.length() + 1);

        int response = chan.cread(rA, sizeof(filemsg) + fileName.length() + 1);

        //cout << *(__int64_t*)rA << endl;

        string writefile = "./received/patient" + file;
        ofstream oFile(writefile);

        __int64_t size = *(__int64_t*)rA;

        int packetSize = 256;
        int iters = size/ packetSize;
        int endCase = size % packetSize;

/*
        msg->length = 256, msg->offset = 0;
        memcpy(rA, msg, sizeof(filemsg));
        strcpy(rA + sizeof(filemsg), fileString);
        chan.cwrite(rA, sizeof(filemsg) + fileName.length() + 1);
        response = chan.cread(rA, 256);
        cout << rA << endl;
        oFile.write(rA, 256);

        msg->length = 256, msg->offset = 256;
        memcpy(rA, msg, sizeof(filemsg));
        strcpy(rA + sizeof(filemsg), fileString);
        chan.cwrite(rA, sizeof(filemsg) + fileName.length() + 1);
        response = chan.cread(rA, 256);
        cout << rA << endl;
        oFile.write(rA, 256);
*/


        gettimeofday(&start, NULL);
        for (int i = 0; i <= iters; i++)
        {
            if(size <= i*packetSize)
            {
                break;
            }
            if(size % packetSize !=0 && i == iters)
            {
                msg->length = endCase, msg->offset=i*packetSize;
            }
            else
            {
                msg->length = packetSize, msg->offset=i*packetSize;
            }

            memcpy(rA, msg, sizeof(filemsg));
            strcpy(rA + sizeof(filemsg), fileString);
            chan.cwrite(rA, sizeof(filemsg) + fileName.length() + 1);

            response = chan.cread(rA, 256);
            //cout<<rA<<endl;

            if(size % packetSize != 0 && i == iters)
            {
                oFile.write(rA, endCase);
            }
            else
            {
                oFile.write(rA, packetSize);
            }


            
        }
        gettimeofday(&end, NULL);
        oFile.close();

        double timems = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) * 1e-6);
        cout << "Time taken to copy data was: " << timems << endl;
        
        /*
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
        outputfile.open("./received/patient" + file);

        int buffsize = 256;
        __int64_t offset = 0;

        //testing
        msg = new filemsg(0,256);
        memcpy(buf, msg, sizeof(filemsg));
        //cout << "Buffer before the server responds" << buf << endl;
        chan.cwrite(buf, sizeof(filemsg) + file.size() + 1);
        nbytes = chan.cread(buf, sizeof(filemsg) + file.size() + 1);
        //cout << "Server Returned: " << nbytes << endl;
        //cout << "Server Returned: " << buf << endl;
        outputfile.write(buf, sizeof(filemsg) + file.size() + 1);

        
        for(int i = 0; i <= size/buffsize; i++)
        {
            cout << "The Current Size Left: " << size - offset << endl;

            
        }
        
       delete msg;
       free(buf);
       */
    }
    // PART 3 ===============================================
    if (c != -1)
    {
        char buff[MAX_MESSAGE];    

        MESSAGE_TYPE new_Channel = NEWCHANNEL_MSG;
        chan.cwrite(&new_Channel, sizeof(MESSAGE_TYPE));

        int nbytes = chan.cread(buff, sizeof(MESSAGE_TYPE));
        cout << (string) buff << endl;
        FIFORequestChannel chan2 ("data1_",FIFORequestChannel::CLIENT_SIDE);

        cout << "Testing with Datamsg(10, 59, 2): ";

        datamsg d(10,59,2);
        chan2.cwrite(&d, sizeof(d));
        int nbits = chan2.cread(buff, MAX_MESSAGE);
        //outputing the single data point received from the server
        cout << *(double*)buff << endl;

        MESSAGE_TYPE m = QUIT_MSG;
        chan2.cwrite (&m, sizeof (MESSAGE_TYPE));
    }

    // closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));

    wait(NULL);
}
