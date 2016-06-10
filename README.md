# code_music
This is for composing music in assembly on MAC 2.4 GHz Intel Core i5 with nasm

For a nice wave generation example see the templates

For 2 instrument examples go to [portaudio - FAQ: two sounds at same time](http://www.portaudio.com/faq.html#two) 

## .c dependencies:
* portaudio - just `brew install portaudio`

you need to compile with `gcc` and link to portaudio like so:

`gcc sourcemusicfile.c -o outputfile.x -lportaudio`

## .asm dependencies:
* nasm - just `brew install nasm`
* ld - installed with OSX developer tools
