#Stegosaur

##About

Stegosaur is a command line tool for hiding and extracting messages in images.
Stegosaur uses a plain least-significant-bit steganographic technique to hide 
messages in bitmap images. *Note:* **Stegosaur is NOT encryption!** - the messages
you hide are hidden to the human eye, but anyone who suspects a message to be 
hidden in the image could easily extract it.

##Example

This README has been hidden inside this picture of the world:

![Hello, World; read me!](earth.steg.bmp)

##Installation

To install, simply

```shell
make steg
```

##Usage:
To hide the contents of "messagefilename" in the bitmap image "bitmapfilename":

```shell
stegosaur messagefilename bitmapfilename
```

To extract a hidden message from the bitmap image "bitmapfilename": 

```shell
stegosaur bitmapfilename
```
