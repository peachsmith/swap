# Swap
## What is Swap?
A solution without a problem
## Build
### Linux
```
make
```
### Windows
A Visual Studio solution is provided
### MinGW or Cygwin
Edit the Makefile to use windows file names then run

    make
## Basic Syntax
    import "io";
    
    // hello world
    writeln("Hello, World!");
    
    // structure
    struct Stuff {
        d;
    }
    
    s = new Stuff;
    s.d = 4;
    
    // functions
    function blat(stuff) {
        if (typeof(stuff) == "struct") {
            writeln(stuff.d);
        }
    }
    
    blat(s);
    
