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
<p>yarpmanager-console2 is a program to manage and monitor YARP modules.<br>
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

<p>Now you can use yarpmanager-console2 to manage and monitors your moduled and your connections.</p>
