# Game of Life

###Description
C++ implementation of the [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)

The implementation is inspired by : 

- [lucarin91](https://github.com/lucarin91)
- [LapoFrati](https://github.com/LapoFrati)

##Usage
<b>Usage:</B> ./game_of_life [options]

| Option | Description |
|:------:|:-----------|
| --width __NUM__ | width of the matrix |
| --height __NUM__ | height of the matrix |
| --input __FILE__ | file where to retrieve the matrix |
| --steps __NUM__| number of steps |
| --thread __NUM__ | number of threads |
| --graphic | activate the graphic mode |
| --output __FILE__ | file where to save the generated video |
| --help | shows this help view |


###Compile
If you have installed <b>cmake</b>, you can simple give the following commands:

```bash
mkdir build
cmake ..
make
```

Otherwise, you can use the <b>makefile</b> I wrote:
```bash
mkdir build
make
```

Bellow you can see a frame of the generated video giving to the program the following parameters:

```bash
./game_of_life -g -w 500 -h 300 -s 10
```

<img src="./demo.png">

###License
Apache License

Copyright (c) 2015 Federico Conte

http://www.apache.org/licenses/LICENSE-2.0
