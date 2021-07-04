<h1>YARP - Yet Another Robot Platform</h1>
<h2>Runtime monitors of YARP modules</h2>

### Introduction
[Go to Introduction](#intro)

### How it works
[Go to How it works](#how)

### Installation, Configuration and Execution
[Go to Installation, Configuration and Execution](#installation)

<p>
 <br><br><br>
 Bachelor degree in Computer engineering thesis by Simone Contorno.<br>
 Documentation about yarpmanager-console2 program.
</p>

<a name="intro"></a>
<h3>Introduction</h3>
<p>
 YARP supports building a robot control system as a collection of programs communicating in a peer-to-peer way, with an extensible family of connection types (tcp, udp, multicast, ...).<br>
 YARP offers a lot of command line tools like yarpmanager-console; this is a program to run and manage multiple programs on a set of machines; you can do it managing: 
 
 <ul>
  <li>Module: an executable binary file (program).</li>
  <li>Application: a collection of modules, connections and other applications.</li>
  <li>Resources: refers to any physical or logical resources which are needed by module.</li>
 </ul>

It is possible control the state of a module or an application

 <ul> 
  <li>RUNNING: the module or the application is in execution.</li>
  <li>STOPPED: the module or the application is not in execution.</li>
 </ul>

and of a connection:

 <ul> 
   <li>CONNECTED</li>
   <li>DISCONNECTED</li>
  </ul>
  
yarpmanager-console2 implement a new function that works as a separate thread during the execution of the program: monitors modules and connections and reports if one of them terminates unexpectedly.<br>
<br>
It looks like this at start:</p>

![start](https://github.com/simone-contorno/yarpmanager-console2/blob/main/images/schermata_iniziale.png)

<a name="how"></a>
<h3>How it works</h3>
<p>yarpmanager-console2 allows many commands to launch:
 
 <ul>
  <li>help:                   show help.</li>
  <li>exit:                   exit from the program.</li>
  <li>list mod:               list available modules.</li>
  <li>list app:               list available applications.</li>
  <li>list res:               list available resources.</li>
  <li>add mod [filename]:     add a module from its description file.</li>
  <li>add app [filename]:     add an application from its description file.</li>
  <li>add res [filename]:     add a resource from its description file.</li>
  <li>load app [application]: load an application to run.</li>
  <li>run [IDs]:              run an application or a module indicated by IDs.</li>
  <li>stop [IDs]:             stop running application or modules indicated by IDs.</li>
  <li>kill [IDs]:             kill running application or modules indicated by IDs</li>
  <li>connect [IDs]:          establish all connections or just one connection indicated by IDs.</li>
  <li>disconnect [IDs]:       remove all connections or just one connection indicated by IDs.</li>
  <li>which:                  list loaded modules, connection and resource dependencies.</li>
  <li>check dep:              check for all resource dependencies.</li>
  <li>check state [id]:       check for running state of application or a module indicated by id.</li>
  <li>check con [id]:         check for all connections state or just one connection indicated by id.</li>
  <li>set <option> [value]:   set value to an option. To see all available options type 'set --help'.</li>
  <li>export [filename]:      export application's graph ad Graphviz dot format.</li>
  <li>assign hosts:           automatically assign modules to proper nodes using load balancer.</li>
  </ul>
</p>

<p>It appears like this in the program: </p>

![start](https://github.com/simone-contorno/yarpmanager-console2/blob/main/images/help.png)

<p>
 When you launch the program it starts with the main class; if you did not set the 'ymc2-config.ini' file, you need to insert the folder's paths that contain your applications, modules and resources (in this last one case, only if you have them, otherwise you can digit 'n' when the program will ask you, or set it in the 'ymc2-config.ini' file).<br>
 After, the 'main' class starts the separate thread, that will check all modules and connections each 5 seconds (to default, but you can set it in the 'ymc2-config.ini' file), and calls the function 'messageConsole' by the 'Console' class; it starts a 'while' cicle that calls the function 'getInput', so you can start to use all the commands shown above; when you digit one of them, it starts a control with a conditional structure 'if-else if-else' to select the correct operation and call the correct functions.<br>
 When you have finished to use the program, you need to ditig 'exit' to close it, because there is a control in the 'main' class to catch the 'CTRL+C' signal; in this way the program will close all connections and running modules before the end.
</p>

<a name="installation"></a>
<h3>Installation, Configuration and Execution</h3>
<p>Open the terminal and go in the folder where you want to download yarpmanager-console2 and digit:</p>

<pre>
 <code>
  git clone https://github.com/simone-contorno/yarpmanager-console2 
 </code>
</pre>

<p>You can also download the zip code:</p>

![start](https://github.com/simone-contorno/yarpmanager-console2/blob/main/images/download.png)

<p>Go in the folder where you downloaded the zip and extract it:</p>

<pre>
 <code>
  unzip tesi-main.zip 
 </code>
</pre>

<p>Enter in the folder:</p>

<pre>
 <code>
  cd yarpmanager-console2-zip 
 </code>
</pre>

<p>Open the ymc2-config.ini file:</p>

<pre>
 <code>
  nano ymn2-config.ini
 </code>
</pre>

<p>Set:
 
<ul>
  <li><b>appPath</b>: path of your applications files.</li>
  <li><b>modPath</b>: path of your modules files.</li>
  <li><b>resPath</b>: path of your resources files.</li>
  <li><b>anyRes</b>: write 'y' if you have any resource file, otherwise write 'n'.</li>
  <li><b>sleepTimer</b>: timer to check modules and connections states; it is '5000' (milliseconds) to deafult.</li>
 </ul>
 
Save typing "CTRL+X" and "y" to confirm.
</p>

<p>Create a folder named 'build':</p>

<pre>
 <code>
  mkdir build
 </code>
</pre>

<p>Enter in the 'build' folder:</p>

<pre>
 <code>
  cd build
 </code>
</pre>

<p>Generates the 'Makefile':</p>

<pre>
 <code>
  cmake ..
 </code>
</pre>

<p>Build the program:</p>

<pre>
 <code>
  make
 </code>
</pre>

<p>Run the program:</p>

<pre>
 <code>
  ./yarpmanager-console2
 </code>
</pre>

<p>Now you can use yarpmanager-console2 to manage and monitors your modules, applications and connections.</p>
