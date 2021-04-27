/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/manager/localbroker.h>
#include <yarp/manager/yarpdevbroker.h>
#include <yarp/manager/scriptbroker.h>
#include <yarp/manager/xmlapploader.h>
#include <yarp/manager/xmlmodloader.h>
#include <yarp/manager/xmlresloader.h>
#include <yarp/manager/xmlappsaver.h>
#include <yarp/manager/singleapploader.h>
#include <yarp/os/impl/NameClient.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RpcServer.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/manager/broker.h>
#include <yarp/manager/manager.h>
#include <yarp/manager/yarpbroker.h>

#include <yarp/manager/execstate.h>
#include <yarp/manager/executable.h>
#include <yarp/os/Time.h>

#include <csignal>
#include <cstring>
#include <iostream> 
#include <cstdio>
#include <ctime>

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::RpcServer;

using namespace std;
using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::manager;

class Printer {
    private:
        yarp::os::Network yarp;
        RpcServer port;
        string command;
        Bottle cmd;
        time_t t;
        
        Manager* pManager;
        Executable* pExecutable;
        FSM::IEventSink* pEventSink;
        YarpBroker* pYarpBroker;
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
            
            cout << pManager << endl;
            cout << pExecutable << endl;
            cout << pEventSink << endl;
            cout << pYarpBroker << endl << endl;
            
            /* ************************************************************************* */
            /* Queste sono le varie prove, dei vari comandi che ho provato ad utilizzare */
            /* ma tutti questi mi danno errore.                                           */
            /* ************************************************************************* */
            //pManager -> ~Manager();
            //Manager *man = new Manager("/usr/share/yarp/modules", 
                //"/usr/share/yarp/applications", "/usr/share/yarp", false);
            //Manager *man = new Manager(false);
            //pManager -> allRunning();
            
            // Starting info
            yInfo() << "Printer is running..."; 
            cout << endl;
  
            port.open(port_name);
            //yarp.connect("/root",port_name);
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
                    cout << "[-] Process '" << process << "' finisched at: " << current_time << endl;
                }
                cout << endl;
            }
            port.close();
        }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Please supply a port name for the server\n");
        return 1;
    }

    Printer printer;
    printer.startMessage(argv[1]);

    return 0;
}
