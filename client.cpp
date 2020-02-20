/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include "common.h"
#include "FIFOreqchannel.h"
#include <fstream>

using namespace std;


int main(int argc, char *argv[]){
    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
    srand(time_t(NULL));

    int opt;
    int patient = 1, ecg = 1;
    double time = 0.00;

	while ((opt = getopt(argc, argv, "p: t: e:")) != -1) {
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

            case '?':
                cout << "Unknown input" << endl;
                break;
		}
	}

    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);

    //getting a single Data point
    char buf[MAX_MESSAGE];
    datamsg d(patient, time, ecg);
    d.mtype = DATA_MSG;

    chan.cwrite(&d, sizeof(d));
    int nbits = chan.cread(buf, MAX_MESSAGE);
    //outputing the single data point received from the server
    cout << *(double*)buf << endl;

    //writing all values from patient one to a file
    ofstream file;
    file.open("x1.csv");
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
    file.close();

    // closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));
}
