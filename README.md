# BytePass
Lightweight P2P File Transfer

> ⚠️ **Development Status**: This project is currently in development, and usage is at your own risk as features, stability, and security may change without notice.

[![Platforms](https://img.shields.io/badge/Platforms-Linux-blue)]()
[![Coverage Status](https://coveralls.io/repos/github/IdanKoblik/BytePass/badge.svg)](https://coveralls.io/github/IdanKoblik/BytePass)

### Getting started
Read the following guide to install bytepass on your machine:

#### Official binary
**TODO**

#### Compiling from source

First you will need to clone the project and build it and then run the executable.
```bash
git clone https://github.com/IdanKoblik/bytepass.git
cd bytepass

make

./echo-ft
```

### Usage
There is 2 main options to communicate with the program.

#### Dialog 

```bash
./echo-ft
```

This will lead you to a dialog with the program to handle receiving and sending data.

#### Flags

```bash
# Receiver Flags
./echo-ft --mode=receiver --port=8080 --outputDir=/path/to/output

# Sender Flags
./echo-ft --mode=sender --addr=0.0.0.0:8080 --filename=test.txt
```

In this way you providing the program the parameters it needs to operate.
