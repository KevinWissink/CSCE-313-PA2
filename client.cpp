/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include "common.h"
#include "FIFOreqchannel.h"

using namespace std;


int main(int argc, char *argv[]){
    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
    srand(time_t(NULL));

    int opt;
    int patient = 1, ecg = 1;
    double time = 0;

	while ((opt = getopt(argc, argv, "p: t: e:")) != -1) {
		switch (opt) {
			case 'p':
				patient = atoi (optarg);
                cout << "Patient Input: " << patient << endl;
				break;
            
            case 't':
				time = atoi (optarg);
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

    datamsg data(patient, time, ecg);

    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);



    // sending a non-sense message, you need to change this
    char buf [MAX_MESSAGE];
    char x = 55;
    chan.cwrite (&x, sizeof (x));
    int nbytes = chan.cread (buf, MAX_MESSAGE);

    MESSAGE_TYPE datapt = DATA_MSG;
    chan.cwrite(&datapt, sizeof(MESSAGE_TYPE));
    int Data_point = chan.cread(buf, MAX_MESSAGE);

    cout << Data_point << endl;

    // closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));
}
