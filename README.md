<a name="readme-top"></a>
# FEUP-rcom-lab3

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#overview">Overview</a></li>
    <li><a href="#main-goal">Main goal</a></li>
    <li><a href="#development-environment">Development Environment</a></li>
    <li><a href="#setup">Setup</a></li>
    <li><a href="#disclaimer">Disclaimer</a></li>
  </ol>
</details>

## Overview
This is the third project for the Computer Networks course unit at FEUP (2nd semester of 2022/23).

## Main goal
Implement and test a data communication protocol involving:
* Control frames vs data frames
* Frame synchronisation, error robustness, error correction and more
* Stop-and-wait ARQ protocol
* Modularity and layering
* Performance evaluation
	
## Development Environment
This project was developed with:
* PCs with Linux 
* Programming language: C 
* Serial ports: RS-232 (asynchronous communication)
	
## Setup
#1
If you don't have a serial port you can still implement the protocol and application.

On Windows (you need cygwin):
* https://www.cygwin.com/
* http://www.eltima.com/products/vspdxp/ (trial version)
* http://www.virtual-serial-port.com/ (trial version)

On Linux:
* https://tibbo.com/support/downloads/vspdl.html (VSPDL)

#2
Or:
```sh
sudo apt install socat
```
```sh
sudo socat -d -d PTY,link=/dev/ttyS10,mode=777 PTY,link=/dev/ttyS11,mode=777
```
#3

1. Download, compile, and run the virtual cable program "cable.c".
```sh
gcc cable.c -o cable
```
```sh
sudo ./cable
```
2. Connect the transmitter to /dev/ttyS10 and the receiver to /dev/ttS11 (or the other way around).

3. Type on and off in the cable program to plug or unplug the virtual cable.
<p align="right">(courtesy of professor Manuel Alberto Pereira Ricardo)

## Disclaimer
This repository is for educational use only. 

<p align="right">(<a href="#readme-top">back to top</a>)</p>
