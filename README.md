# Table of contents
   * [Description](#description)
   * [Usage](#usage)
   * [License](#license)


# Description
Single Cycle MIPS Processor Simulator in C++. Every one word in MIPS processor is assumed 32 bits. It uses ```sampleCode.txt``` file as MIPS machine code input and execute that machine code at single cycle duration. Its schematic can be viewed at this picture:
![MIPS Processor Single Cycle Schematic](https://raw.githubusercontent.com/TechieForFun/mipsimulator/master/single-cycle-mips.jpg)


# Usage
First clone the repository by:
```
git clone https://github.com/TechieForFun/mipsimulator.git
```
or by downloading it. In linux at the project directory execute
```
mipsimulator
```
as a previous compiled executable file, or for compilation use
```
gcc -o mipsimulator mipsimulator.cpp -lstdc++
```
then run the ```mipsimulator``` output file. This is a sample result from default ```sampleCode.txt``` file.
![Sample Result](https://raw.githubusercontent.com/TechieForFun/mipsimulator/master/sample-result.gif)
For using a custom MIPS machine code just edit ```sampleCode.txt```. More information about MIPS instructions can be found at [MIPS Instruction Reference](http://www.mrc.uidaho.edu/mrc/people/jff/digital/MIPSir.html).


# License
The project is licensed under [The Unlicense](http://unlicense.org).
