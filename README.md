# Instrument Control Lib

<img src="docs_and_specs/figures/Logo.svg" alt="">


This repository contains the code to configure oscilloscopes, signal generators, DC Power-supplies and other measurement devices.
<br><br>
It provides following functionality:
<br>
- Establishing a network connection and identifying the different types of devices
  <br><br>
  
- Execution of VISA commands on the devices. E.g. setting the voltage on a DC power-supply, Setting voltage limits on SMUs or to gather the current data of an oscilloscope
  <br><br>
  
- CSV export function to directly store the output of measure devices in an *.csv file.
  <br><br>
- Environment to set up test cases, configured by an XML file.
  <br><br>
- Platform independent implementation (tested on Windows, MAC OS and Ubuntu)

## Supported Devices
#### &nbsp;&nbsp;&nbsp;&nbsp;- SPD1305X DC Power Supply
#### &nbsp;&nbsp;&nbsp;- Keysight 33500B Waveform Generator
#### &nbsp;&nbsp;&nbsp;- Keysight InfiniiVision 3000A X-Series Oscilloscopes
#### &nbsp;&nbsp;&nbsp;- Tektronix 2600B Series SMU


## Supported Commands per device

#### SPD1305X DC Power Supply
| **Function** | **Parameters**   | **Description**                                          | **Value Range** |
|--------------|------------------|----------------------------------------------------------|-----------------|
| setCurrent() | channel, current | Sets the current of the DC power-supply.                 | See manual.     |
| getCurrent() | channel          | Get the currently adjusted current of a certain channel. | See manual.     |
| setVoltage() | channel, voltage | Sets the voltage on the DC power-supply.                 | See manual      |
| getVoltage() | channel          | Get the currently adjusted voltage of a certain channel. | See manual      |

<br><br>
#### Keysight 33500B Waveform Generator

| **Function**        | **Parameters**        | **Description**                                                                                                                                               | **Value Range**                                                |
|---------------------|-----------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------|
| run()               | -                     | Start the acquisition on the instrument.                                                                                                                      | -                                                              |
| stop()              | -                     | Stop the acquisition on the instrument.                                                                                                                       | -                                                              |
| single()            | -                     | Wait for the first signal which is triggered and stop the measurement.                                                                                        | -                                                              |
| autoScale()         | -                     | The oscilloscope tries to find the best vertical channel, timebase and trigger setting for the current experiment.                                            | -                                                              |
| setTimeRange()      | timeRange             | Sets the full-cale horizontal time in seconds for the main window.                                                                                            | 10x current time-per-devision setting                          |
| setChannelOffset()  | channel, offset, unit | Set the offset of a channel in volts or milli volts.                                                                                                          | When amplitude is below 400 mV the offset is limited to 500 mV |
| setChannelScale()   | channel, scale        | Sets the vertical scale or units per division of a specified channel.                                                                                         | See manual                                                     |
| setChannelRange()   | channel, range        | Defines the full-scale vertical axis on the selected channel                                                                                                  | 8 mV - 40 V                                                    |
| getWaveFormData()   | data                  | Return the currently captured waveform, which is visualized on the oscilloscopes screen. The data is formated in IEEE 488.2 arbitrary block data format.      | -                                                              |
| getRealData()       | double **result       | Captures data from the oscilloscope and transforms the data from binary array to a double array.                                                              | -                                                              |
| digitize()          | -                     | Specialized run command. Captures waveforms according to the settings of the :ACQuire format. (Can only be executed with :TIMbased:MODE being MAIN or WINDow) | -                                                              |
| getSystemSetup()    | -                     | Outputs the current oscilloscope setup as string.                                                                                                             | -                                                              |
| setDisplayMode()    | mode                  | Sets the mode of the oscilloscope either to normal mode, time based mode, XY or roll mode.                                                                    | NORMAL, TIME_BASED; XY, ROLL                                   |
| setChannelDisplay() | channel               | Turns the output of a channel on or off.                                                                                                                      | ON / OFF                                                       |

<br><br>
#### Tektronix 2600B Series SMU

| **Function**              | **Parameters** | **Description**                                                                  | **Value Range** |
|---------------------------|----------------|----------------------------------------------------------------------------------|-----------------|
| turnOn()                  | channel        | Enable a channel on the SMU.                                                     | See manual      |
| turnOff()                 | channel        | Disable a channel on the SMU.                                                    | See manual      |
| measure()                 | channel, unit  | Measure a certain unit (voltage, current, power) and return the measure results. | See manual      |
| setLevel()                | channel        | TODO?                                                                            | See manual      |
| setLimit()                | channel        | TODO?                                                                            | See manual      |
| enableMeasureAutoRange()  | unit, channel  | TODO?                                                                            | See manual      |
| disableMeasureAutoRange() | unit, channel  | TODO?                                                                            | See manual      |
| enableSourceAutoRange()   | unit, channel  | TODO?                                                                            | See manual      |
| disableSourceAutoRange()  | unit, channel  | TODO?                                                                            | See manual      |
| setMeasureRange()         | unit, channel  | TODO?                                                                            | See manual      |
| setSourceRange()          | unit, channel  | TODO?                                                                            | See manual      |
| selectLocalSense()        | unit, channel  | TODO?                                                                            | See manual      |
| setRemoteSense()          | unit, channel  | TODO?                                                                            | See manual      |
| enableBeep()              | -              | Enable the execution of a beep sound on the oscilloscope.                        | See manual      |
| enableBeep()              | -              | Disable the execution of a beep sound on the oscilloscope.                       | See manual      |
| beep()                    | -              | Send a beep signal to the SMU.                                                   | See manual.     |

<br><br>
#### Keysight 33500B Waveform Generator

| **Function**        | **Parameters**     | **Description**                                                                                                                     | **Value Range** |
|---------------------|--------------------|-------------------------------------------------------------------------------------------------------------------------------------|-----------------|
| turnOn()            | channel            | Turn on the channel (A or B) on the function generator.                                                                             | -               |
| turnOff()           | channel            | Turn off the channel (A or B) on the function generator.                                                                            | -               |
| setFrequency()      | channel, frequency | Set a certain frequency on a channel of the function generator.                                                                     | See manual      |
| setAmplitude()      | channel, amplitude | Set a certain amplitude on a channel of the function generator.                                                                     | See manual      |
| setOffset()         | channel, offset    | Set a voltage offset on a certain channel of the function generator.                                                                | See manual      |
| setPhase()          | channel, phase     | Adjust the phase on a certain function generator channel.                                                                           | See manual      |
| setFunction()       | funcType           | Set the function currently used (allowed: sinus, square, ramp, negative ramp, triangle, noise, pseudo random bit stream, arbitrary) | See manual      |
| display()           | -                  | -                                                                                                                                   |                 |
| displayConnection() | -                  | -                                                                                                                                   | -               |

## Continuous integration

|       OS       |                                                          Status                                                          | 
|:--------------:|:------------------------------------------------------------------------------------------------------------------------:|
|Windows Latest  | ![Windows latest](https://github.com/FlorianFrank/instrument_control_lib/actions/workflows/windows_latest.yml/badge.svg) |
|Mac OS Latest   |  ![MAC OS latest](https://github.com/FlorianFrank/instrument_control_lib/actions/workflows/mac_os_latest.yml/badge.svg)  |
|Ubuntu Latest   |  ![Ubuntu Latest](https://github.com/FlorianFrank/instrument_control_lib/actions/workflows/ubuntu_latest.yml/badge.svg)  |

## Build

### 1. Build from master branch

#### 1.1 Checkout repository

```
git clone git@github.com:FlorianFrank/instrument_control_lib.git
```

#### 1.2 Configure build

Adjust the configuration files according to your needs.

```
option(INSTRUMENT_LIB_BUILD_STATIC "Build instrument lib as static library"     ON)

# Definitions for the platform independent abstraction layer.
option(PIL_COMMUNICATION    "Enable PIL Sockets"          ON)
option(PIL_THREADING        "Enable PIL Threads"          ON)
option(PIL_LOGGING          "Enable Logging support"      ON)
option(PIL_SHARED           "BUILD PIL as shared library" OFF)
option(PIL_STATIC           "BUILD PIL as static library" ON)
option(PIL_CXX              "Enable PIL C++ support"      ON)

# PugiXML options
option(BUILD_SHARED_LIBS    "Build pugiXML as shared lib" OFF)
set(PUGI_XML_VERSION 1.12)
```

#### 1.3 Build the library

On Linux and MAC OS run:
```bash
./build.sh
```

On Windows run:

````powershell
.\build.bat
````

<br><br>
#### 1.4 Exporting the library 

You can find the library in the **build** folder.
```
out
├── include          Here you can find the device and include fines for the project
├── ĺib              Here you can find the libraries to link to your project.
├── bin              In this folder you can find sample test programs, like a command line interface.
├── doc              This folder contains the doxygen documentation.
```

### 2. Download pre-installed packages

#### 2.1 Download a released version from github.

Go to:<br>
[Github Packages](https://github.com/FlorianFrank/instrument_control_lib/tags) and download the preferred version.

#### 2.2 Install the package

#### 2.2.1 On Debian based systems

```
dpkg -i instrument_control_lib.deb
```

The files are copied into the **/usr/bin, /usr/lib/,** and **/usr/include** folders and can be linked and included in any program.

#### 2.2.2 On Windows systems


## Examples

##

### Connect devices
```c++
#include "kst33500.h" // include right device header file

int main() {
    // provide a IP address, some devices may need an extra port parameter
    KST33500 k("xx.xx.xx.xx"); 
    k.Connect();
}
```

### Execute commands
#### Use Exec function to send SCPI commands.

```c++
// set a SIN wave
k.Exec("FUNCtion SIN");
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

Basically, you can run any commands using Exec commands. For saving time,
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
What you need to do is making an SCPI command and invoke the Exec function.
```c++
int KST33500::function(string fun) {
    string msg = "FUNCtion " + fun;
    Exec(msg);
    return 0;
}
```
That's it. After that, you can use like:
```c++
k.function("SIN");
```

