# AviKernel

A i686-elf-gcc cross compiler should be installed at /opt/cross. You can run everything on the provided Docker container instead.
On MacOS, just install the cross compiler with:
`brew install i686-elf-gcc`

Nasm should be installed as well. On MacOS:
`brew install nasm`

You should have grub2 as well to build the ISO file. On macos, just run `brew install i686-elf-grub`. (They couldn't tell me there was a brew for that BEFORE I tried for hours to compile it manually???)

If you're on linux, run as well:
`sudo apt-get install grub-pc-bin`
