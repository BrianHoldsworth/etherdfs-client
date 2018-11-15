# etherdfs-client

Original project migrated from Sourceforge. MIT License.

_Copyright © 2017, 2018 Mateusz Vistelink_

### EtherDFS - The Ethernet DOS File System

EtherDFS is a client/server filesystem that allows a modern Linux host (the server) to easily share files with an old PC running DOS. The client in this repository is a __TSR for DOS__. The necessary server code for Linux is also [available on Github](https://github.com/BrianHoldsworth/etherdfs-server). The basic functionality is to map a drive from the server to a local DOS drive letter, using raw Ethernet frames to communicate. It can be used in place of obsolete DOS-to-DOS file sharing applications like [Laplink](https://books.google.com/books?id=kggOZ4-YEKUC&pg=PA92#v=onepage&q&f=false).

### Requirements

EtherDFS hardware/software requirements:

 - An 8086/8088 compatible CPU
 - MS-DOS 5.0+ or compatible
 - 8 KiB of available conventional memory (can be loaded high)
 - An Ethernet interface and its packet driver

### Client Build

The client code for EtherDFS contained in this repository can be built in an MS-DOS environment, such as [Dosbox](https://www.dosbox.com/download.php?main=1), or an actual DOS PC, using the [Open Watcom C compiler V1.9](https://sourceforge.net/projects/openwatcom/files/open-watcom-1.9/open-watcom-c-dos-1.9.exe/download). Build with the included [MAKEFILE](src/MAKEFILE) by simply typing `make` in the cloned Github directory.

For convenience, a pre-built binary for MS-DOS 5+ is included for [download](bin/ETHERDFS.EXE).

### Client Usage

First, launch the server on your Linux host that is connected to the client via Ethernet LAN. The server will show the MAC address of the interface in use on the command line. You can use this same MAC address when launching the client, or just rely on the auto-discovery feature.

Next, you will need to locate and launch a suitable packet driver for the NIC installed in your DOS PC. Virtually all legacy NIC's included suitable packet drivers with their installation disks. A good source of [legacy packet drivers](http://www.georgpotthast.de/sioux/packet.htm) can be found online. The ETHERDFS client TSR relies on the packet driver to send and receive all packets to your LAN. Since EtherDFS runs over raw Ethernet, it doesn't need any TCP/IP stack. The limitation of using raw Ethernet frames is that EtherDFS is, by design, unable to communicate outside of a single LAN.

Finally, launch the client to connect to the server and map the shared directories to appropriate drive letters.
```
C:\> ETHERDFS.EXE 0A:0B:0C:0D:0E:0F C-W D-X
```
Will connect to the supplied MAC address and map the two shared directories to the W: and X: drives locally. To use the automatic server discovery, just substitute `::` for the MAC address, and your server on the same LAN should be found.

To summarize the syntax:

```
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
```

_EtherDFS arguments are case-insensitive, and can be passed in any order._

### Description of EtherDFS Protocol

The ethernet communication between the client and the server is very simple:
for every INT 2F query, the client (EtherDFS) sends a single ethernet frame to
the server (ethersrv), using the following format:
```
DOEEpppVSDLxxx

where:

offs|field| description
----+-----+-------------------------------------------------------------------
 0  | D   | destination MAC address
 6  | O   | origin (source) MAC address
 12 | EE  | EtherType value (0xEDF5)
 14 | ppp | padding: 38 bytes of garbage space. used to make sure every frame
    |     | respects the minimum ethernet payload length of 46 bytes. could
    |     | also be used in the future to fill in fake IP/UDP headers for
    |     | router traversal and such.
 52 | ss  | size, in bytes, of the entire frame (optional, can be zero)
 54 | cc  | 16-bit BSD checksum, covers payload that follows (if CKS flag set)
 56 | V   | the etherdfs protocol version (7 bits) and CKS flag (highest bit)
 57 | S   | a single byte with a "sequence" value. Each query is supposed to
    |     | use a different sequence, to avoid the client getting confused if
    |     | it receives an answer relating to a different query than it
    |     | expects.
 58 | D   | a single byte representing the numeric value of the destination
    |     | (server-side) drive (A=0, B=1, C=2, etc) in its 5 lowest bits,
    |     | and flags in its highest 3 bits (flags are undefined yet).
 59 | L   | the AL value of the original INT 2F query, used by the server to
    |     | identify the exact "subfunction" that is being called.
 60 | xxx | a variable-length payload of the request, it highly depends on the
    |     | type subfunction being called.
```
For each request sent, the client expects to receive exactly one answer. The
client might (and is encouraged to) repeat the query if no valid answer comes
back within a reasonable period of time (several milliseconds at least).

An EDF5 answer has the following format:
```
DOEEpppssccVSAAxxx

where:
 DOEEpppssccVS = same as in query (but with D and O reversed)
 AA            = the 16-bit value of the AX register (0 for success)
 xxx           = an optional payload

Note: All numeric values are transmitted in the native x86 format (that is,
      "little endian"), with the obvious exception of the EtherType which
      must be transmitted in network byte order (big endian).

==============================================================================
RMDIR (0x01), MKDIR (0x03) and CHDIR (0x05)

Request: SSS...

SSS... = Variable length, contains the full path of the directory to create,
         remove or verify existence (like "\THIS\DIR").

Answer: -

Note: The returned value of AX is 0 on success.
==============================================================================
CLOSEFILE (0x06)

Request: SS

SS = starting sector of the open file (ie. its 16-bit identifier)

Answer: -

Note: The returned value of AX is 0 on success.
==============================================================================
READFILE (0x08)

Request: OOOOSSLL

OOOO = offset of the file (where the read must start), 32-bits
SS   = starting sector of the open file (ie. its 16-bit identifier)
LL   = length of data to read

Answer: DDD...

DDD... = binary data of the read file

Note: AX is set to non-zero on error. Be warned that although LL can be set
      as high as 65535, the unerlying Ethernet network is unlikely to be able
      to accomodate such amounts of data.
==============================================================================
WRITEFILE (0x09)

Request: OOOOSSDDD...

OOOO = offset of the file (where the read must start), 32-bits
SS   = starting sector of the open file (ie. its 16-bit identifier)
DDD... = binary data that has to be written (variable lenght)

Answer: LL

LL = amounts of data (in bytes) actually written.

Note: AX is set to non-zero on error.
==============================================================================
LOCK/UNLOCK FILE REGION (LOCK = 0x0A, UNLOCK = 0x0B)

Request: NNSSOOOOZZZZ[OOOOZZZZ]*

NN   = number of lock/unlock regions (16 bit)
SS   = starting sector of the open file (ie. its 16-bit identifier)
OOOO = offset of the file where the lock/unlock starts
ZZZZ = size of the lock/unlock region

Answer: -

Note: AX is set to non-zero on error.
==============================================================================
DISKSPACE (0x0C)

Request: -

Answer: BBCCDD
  BB = BX value
  CC = CX value
  DD = DX value

Note: The AX value is already handled in the protocol's header, no need to
      transmit it a second time here.
==============================================================================
SETATTR (0x0E)

Request: Afff...
  A      = attributes to set on file
  fff... = path/file name

Answer: -

Note: AX is set to non-zero on error.
==============================================================================
GETATTR (0x0F)

Request: fff...
  fff... = path/file name

Answer: ttddssssA
  tt = time of file (word)
  dd = date of file (word)
  ssss = file size (dword)
  A = single byte with the attributes of the file

Note: AX is set to non-zero on error.
==============================================================================
RENAME (0x11)

Request: LSSS...DDD...
  L      = length of the source file name, in bytes
  SSS... = source file name and path
  DDD... = destination file name and path

Answer: -

Note: AX is set to non-zero on error.
==============================================================================
DELETE (0x13)

Request: fff...
  fff... = path/file name (may contain wildcards)

Answer: - (AX = 0 on success)
==============================================================================
OPEN (0x16) and CREATE (0x17) and SPOPNFIL (0x2E)

Request: SSCCMMfff...
  SS = word from the stack (attributes for created/truncated file, see RBIL)
  CC = "action code" (see RBIL for details) - only relevant for SPOPNFIL
  MM = "open mode" (see RBIL for details) - only relevant for SPOPNFIL
  fff... = path/file name

Answer: AfffffffffffttddssssCCRRo (25 bytes)
  A  = single byte with the attributes of the file
  fff... = filename in FCB format (always 11 bytes, "FILE0000TXT")
  tt = time of file (word)
  dd = date of file (word)
  ssss = file size (dword)
  CC = start cluster of the file (16 bits)
  RR = CX result: 1=opened, 2=created, 3=truncated (used with SPOPNFIL only)
  o  = access and open mode, as defined by INT 21h/AH=3Dh

Note: Returns AX != 0 on error.
==============================================================================
FINDFIRST (0x1B)

Request: Affffffff...
  A = single byte with attributes we look for
  ffff... = path/file mask (eg. X:\DIR\FILE????.???), variable length (up to
            the end of the ethernet frame)

Answer: AfffffffffffttddssssCCpp (24 bytes)
  A = single byte with the attributes we look for
  fff... = filename in FCB format (always 11 bytes, "FILE0000TXT")
  tt = time of file (word)
  dd = date of file (word)
  ssss = file size (dword)
  CC = "cluster" of the directory (its 16-bit identifier)
  pp = position of the file within the directory
==============================================================================
FINDNEXT (0x1C)

Request: CCppAfffffffffff
  CC = "cluster" of the searched directory (its 16-bit identifier)
  pp = the position of the last file within the directory
  A  = a single byte with attributes we look for
  ffff... = an 11-bytes file search template (eg. FILE????.???)

Answer: exactly the same as for FindFirst
==============================================================================
SEEKFROMEND (0x21)

The EDF5 protocol doesn't really need any 'seek' function. This is rather used
by applications to detect changes of file sizes, by translating a 'seek from
end' offset into a 'seek from start' offset.

Request: ooooSS
  oooo = offset (in bytes) from end of file
  SS   = the 'starting sector' (or 16-bit id) of the open file

Answer: oooo
  oooo = offset (in bytes) from start of file
==============================================================================
SETFILETIMESTAMP (0x24)

Request: ttddSS
  tt = new time to be set on the file (FAT format, 16 bits)
  dd = new date to be set on the file (FAT format, 16 bits)
  SS = the 'starting sector' (or 16-bit id) of the open file

Answer: - (AX zero on success, non-zero otherwise)

Note: The INT 2Fh interface provides no method to set a file's timestamp. This
      call is supported by the EDF5 protocol, but client application must get
      creative if such support is required. This would typically involve
      catching INT 21h,AX=5701h queries.
==============================================================================
```

### Known Issues

#### Displayed "total" and "available" disk space information is not correct.

You may notice that EtherDFS shows "total" and "available" disk space
information that is not exactly the same as what the drive on your server
shows. This is expected, and it is due to two causes:

1. EtherDFS limits the maximum disk space to slightly under 2 GiB, otherwise
   most versions of MS-DOS would be confused.

2. Even below 2 GiB, the amount of disk space won't always be exactly
   reported, because the method of disk space computation is likely to be
   different between DOS and your remote host's system. EtherDFS translates
   one into another, but there still will be minor differences where EtherDFS
   will show a few kilobytes less than expected.

In both cases, the limitation is purely cosmetic and doesn't have any impact
on the amount of data that can effectively be read from or written to the
remote drive.

#### Copied files loose their timestamps.

When you copy a file to an EtherDFS drive, this file will loose its original
timestamp and appear as being created "now". This is not really an EtherDFS
bug, but rather a limitation of MS-DOS. I might work around this in some
future versions.

#### The game "Heretic" doesn't start from an EtherDFS drive with RTSPKT

When the RTSPKT packet driver is used with a Realtek 8139 network card, the
game "Heretic" is unable to start when launched from within an EtherDFS drive.
Not sure whether this is due to a bug in EtherDFS, the RTSPKT driver or both.
