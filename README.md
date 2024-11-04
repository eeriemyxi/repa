# Repa

![](https://files.catbox.moe/u641g4.png)

A simple C program to view top N frequent words and total amount of lines & words in files.

# Usage
```
myxi :: coding/c/repa ‹main*› » ./bin/repa -h
Usage: ./bin/repa [-h] [-t n] [file...]
(help): -h -- show this message.
(help): -a -- don't append working directory when finding files.
(help): -t N -- show top N results. can be 0.
```

# Install
> [!CAUTION]
> Only works on x86_64 Linux. It may work on 32 bit systems, but I haven't tested.

> [!NOTE]
> This project was mainly built due to my desire to implement and make a practical application of a hash map/table in C all on my own.
> I cannot promise to maintain this project. I am very new to C so there most likely are bugs and security vulnerabilities in the program.

```
git clone --depth <REPO_URL>@main
make RELEASE=1
./bin/repa -h
```
> [!IMPORTANT]
> You need a C compiler. I used GCC (Ubuntu 11.4.0-1ubuntu1~22.04 11.4.0) to build it.

> [!TIP]
> You can just do `make` to make debug builds.

# Command-line Arguments
```
Usage: ./bin/repa [-h] [-t n] [file...]
note: [file] can be - to read stdin
help: -h   -- show this message.
help: -a   -- don't append working directory when finding files.
help: -t N -- show top N results. can be 0.
```
