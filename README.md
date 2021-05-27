# Voltage Measurement Lib

This repository contains the code to configure oscilloscopes, signal generators and DC power supplies.

## Supported Devices
- SPD1305X DC Power Supply
- Keysight 33500B Waveform Generator

## Usage
### Connect devices
```c++
#include "kst33500.h" // include right device header file

int main() {
    // provide a IP address, some devices may need an extra port parameter
    KST33500 k = KST33500("xx.xx.xx.xx"); 
    k.connect();
}
```

### Execute commands
####Use exec function to send SCPI commands.

```c++
// set a SIN wave
k.exec("FUNCtion SIN");
```
#### Use wrapped functions
We wrapped some common commands to save your time. Here is an example. 
```c++
k.function("SIN");
k.frequency("+1.0E+03");
k.voltage("+2.0", "HIGH");
k.voltage("+0.0", "LOW");
k.output(true);
k.phase("+90.0");
```
The code above is equivalent to
```
FUNCtion SIN
FREQuency +1.0E+05
VOLTage:HIGH +2.0
VOLTage:LOW +0.0
OUTPut ON
PHASe +90.0
```

## Development
In this lab, we use socket to send SCPI commands to devices in order
control them remotely.   
Check the documents in [Docs](./docs%20and%20specs) for SCPI commands. 

Basically, you can run any commands using exec commands. For saving time,
we wrapped some frequently used commands. But we cannot wrap all the commands.

Here is an instruction for wrapping a command. Let's say you want to wrap a
command of Keysight waveform generator:
```c++
FUNCtion <function>
```
which set a <function> waveform.   
Go to kst33500.h file and add a function signature called "function".
```c++
int function(string fun);
```
Go to skt33500.cpp file and add the implementation of this function.
What you need to do is making an SCPI command and invoke the exec function.
```c++
int KST33500::function(string fun) {
    string msg = "FUNCtion " + fun;
    exec(msg);
    return 0;
}
```
That's it. After that, you can use like:
```c++
k.function("SIN");
```

