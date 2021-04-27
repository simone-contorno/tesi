/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

//#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RpcServer.h>

#include <yarp/manager/manager.h>

#include <iostream> 
#include <cstdio>
#include <ctime>

//using yarp::os::BufferedPort;
using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::RpcServer;

using namespace std;
using namespace yarp::manager;

class Printer {
    private:
        //BufferedPort<Bottle> port;
        //Bottle* input;
        yarp::os::Network yarp;
        string command;
        RpcServer port;
        Bottle cmd;
        time_t t;
        //Manager man;
        //yarp::manager::Manager man;

    public: 
        void startMessage(string port_name) {
            /*
             * Starting message
             */
            cout << "********************************" << endl;
            cout << "*                              *" << endl;
            cout << "*        Yarp Manager          *" << endl;   
            cout << "*       Message Printer        *" << endl;                  
            cout << "*   Author: Simone Contorno    *" << endl;
            cout << "*                              *" << endl;
            cout << "********************************" << endl << endl;
            
            // Starting info
            yInfo() << "Printer is running..."; 
            cout << endl;
            
            //string cmd = "yarprun --server " + port_name;
            //system(cmd.c_str());
            
            port.open(port_name);
            command = "yarp connect /root " + port_name;
            system(command.c_str());
           
            while (true) {
                // Getting time
                t = time(0);
                tm* temp = localtime(&t);
                string hour = to_string(temp->tm_hour);
                string min = to_string(temp->tm_min);
                string sec = to_string(temp->tm_sec);
                if (hour.length() == 1) hour = "0" + hour;
                if (min.length() == 1) min = "0" + min;
                if (sec.length() == 1) sec = "0" + sec;
                string current_time = hour + ":" + min + ":" + sec;
                
                // Getting process name
                string process = "";
                port.read(cmd,true);
                for (int i=0; i<cmd.toString().length(); i++) {
                    if(cmd.toString().at(i) == 34) {
                        for (int j=i+1; cmd.toString().at(j) != 34; j++) {
                            process += cmd.toString().at(j);
                            i = j+1;
                        }
                    }
                }   

                // Printing message
                if (cmd.toString().find("[add]") == 0) {
                    cout << "[+] Process '" << process << "' started at: " << current_time << endl;
                }
                if (cmd.toString().find("[del]") == 0) {
                    cout << "[+] Process '" << process << "' finisched at: " << current_time << endl;
                }
                //printf("Got: %s\n", cmd.toString().c_str());

                //input = port.read();
                //if (input != nullptr) 
                //if (cmd.toString != "")
                    //yInfo() << "Got: " << cmd.toString().c_str();
                    //yInfo() << "Got: " << input->toString().c_str();
            }
            port.close();
        }
};

int main(int argc, char* argv[]) {
    //YARP_UNUSED(argc);
    //YARP_UNUSED(argv);
    if (argc < 2) {
        fprintf(stderr, "Please supply a port name for the server\n");
        return 1;
    }

    Printer printer;
    printer.startMessage(argv[1]);

    return 0;
}
