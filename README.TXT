
                   ETHERDFS - THE ETHERNET DOS FILE SYSTEM
                       http://etherdfs.sourceforge.net


EtherDFS is an 'installable filesystem' TSR for DOS. It maps a drive from a
remote computer (typically Linux-based) to a local drive letter, using raw
ethernet frames to communicate. For years, I was using LapLink to transfer
files between my various "retro" computers. It works, yes, but it's also
annoyingly slow and requires constant attention. One day I thought, "Wouldn't
it be amazing if all my DOS PCs could share a common network drive, similarly
to how NFS operates in the *nix world?". This day EtherDFS was born. I clearly
didn't invent anything - the concept has been around almost as long as the
first IBM PC, and several commercial products addressed that need in the past.
I am not aware, however, of any free and open-source solution. Besides, all
the commercial solutions I know require to set up a pretty complex network
environment first, while EtherDFS doesn't need anything more than just a
packet driver.

EtherDFS is the "client" part, ie. the TSR running on the client DOS computer.
The client requires an EtherSRV instance to communicate with. Currently, an
implementation of EtherSRV exists only for Linux (ethersrv-linux).

Since EtherDFS runs over raw ethernet, it doesn't need any TCP/IP stack. It
only requires a working packet driver. The catch of using raw ethernet frames
is that EtherDFS is, by design, unable to communicate outside of a single LAN.
In some context this might actually be a strenght, you don't need to worry
that your EtherDFS transfer will somehow leak outside your LAN - it's simply
not possible.

Syntax:
  etherdfs SRVMAC rdrv1-ldrv1 [rdrv2-ldrv2] [rdrvX-ldrvX] [options]
  etherdfs /u [/q]

  where:
  SRVMAC  is the MAC address of the file server EtherDFS will connect to. You
          can also use '::' so EtherDFS will try to auto-discover the server
          present in your LAN.
  rdrv    is the remote drive you want to access on the EtherSRV server.
  ldrv    is a local drive letter where the remote filesystem will be mapped.

Available options:
  /p=XX   use the network packet driver XX (autodetected in the range 60h..80h
          if not specified)
  /n      disable EtherDFS cksum - use only if you are 100% that your network
          hardware is working right and you really need to squeeze out some
          additional performance (this doesn't disable Ethernet CRC)
  /q      quiet mode: print nothing on screen if loaded/unloaded successfully
  /u      unload EtherDFS from memory

Examples:
  etherdfs 6d:4f:4a:4d:49:52 C-F /q
  etherdfs :: C-X D-Y E-Z /p=80

Note: EtherDFS arguments are case-insensitive, and can be passed in any order.


===[ Requirements ]===========================================================

EtherDFS hardware/software requirements:
 - An 8086/8088 compatible CPU
 - MS-DOS 5.0+ or compatible
 - 8 KiB of available conventional memory (can be loaded high)
 - An Ethernet interface and its packet driver

ethersrv, on the other hand, requires a reasonably modern Linux system.
Future versions will probably provide a DOS version of ethersrv, too.


===[ Can I use other networking software while EtherDFS is loaded? ]==========

EtherDFS provides low-level I/O disk connectivity through networking. As such,
it might be adviseable to dedicate a single network interface on your PC
solely for the purpose of handling EtherDFS communications - this would
provide the best possible performance and reliability. However, if your PC has
only one ethernet interface, you still can use EtherDFS simulteanously with
other networking applications - EtherDFS will happily share access to the
packet driver with any other applications, but in such case performance might
not be optimal. Take note that, for such network sharing to be possible, your
other networking software must be written in a way that does not require
exclusive control over the packet driver.


===[ Why Ethernet ]===========================================================

This is the first question I get whenever I introduce EtherDFS to someone:
"Why do you use stupid Ethernet instead of IP/UDP/IPv6/IPX/whatever ?"

I thought about the pros and cons of the multiple technical possibilities, and
the advantages of using raw Ethernet frames vastly outweighted the
constraints. It is to be noted that Ethernet is ubiquitious, has been for the
past 30 years, and is likely to stay here for a while. Here below I paste my
"pros & cons" list, so my reasons should become clear.

Pros:
 - Very simple to parse and process = faster, lower memory footprint,
 - No dependencies: doesn't require any TCP/IP stack, only a packet driver,
 - Allows EtherDFS to share a single NIC with other DOS networking software,
 - IP-agnostic: the network can operate IPv4, IPv6, IPX, or anything else,
 - Makes it possible to auto-discover an EtherSRV server on the LAN.

Cons:
 - Unrouteable, meaning that it cannot work outside of a single LAN (running
   EtherDFS over a long-distance network would be unadviseable anyway for
   reliability and latency reasons).


===[ Is it secure? ]==========================================================

Shortly said: no. EtherDFS is designed by a hobbyist, for hobbyists. It is not
meant to be used in an environment where security is required. The EtherDFS
protocol transfers all data as plain text, and doesn't have any provisions for
authentication nor access control. It is worth noting that a potentially
desirable side effect of using raw Ethernet for communication is the fact that
EtherDFS data won't ever make it outside your LAN, simply because it is
unrouteable.


===[ But is it still safe for my data? ]======================================

Should be. I did my best to ensure that EtherDFS does not lead to any
corruption of data, or any other troubles. You have to keep in mind though,
that EtherDFS is a program that takes root deeply in the (often undocumented)
internals of DOS, and as such, is a quite complex beast. Besides, every
program has bugs, this one is surely not an exception. I can only recommend
that you perform periodic backups of your data, just in case.


===[ Contact the author ]=====================================================

I'm always happy to get feedback about my software: bug reports, feature
requests, or simply knowing that you use it and it works for you.
You won't get my e-mail address here, but you should find contact pointers to
me on my website: http://mateusz.viste.fr.


===[ License ]================================================================

EtherDFS is distributed under the terms of the MIT License, as listed below.

Copyright (C) 2017, 2018 Mateusz Viste

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
