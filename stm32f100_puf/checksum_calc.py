import sys
import os
import struct
import binascii

usage = "usage: checksum_calc.py <file>"
 
if len(sys.argv)<1:
    print usage
    exit()
 
f = open(sys.argv[1], "rb")
data = f.read(7*4)
udata = struct.unpack("7I", data)
s = 0
for d in udata:
    s = s + d
s = 0 - s
ret1 = struct.pack("i", s)
ret = struct.unpack("I", ret1)
print "cheksum:", hex(ret[0])
f.seek(0, os.SEEK_SET)
alldata = f.read()
f.close()

print "injecting checksum"
f = open(sys.argv[1], "wb")
alldata = alldata[0:7*4] + ret1 + alldata[8*4:]
f.write(alldata)
f.close()

f = open(sys.argv[1], "rb")
alldata = f.read()
crc = (binascii.crc32(alldata) & 0xFFFFFFFF)
crc_bin = struct.pack("I", crc)
crc1 = struct.unpack("I", crc_bin)
print "CRC32:", hex(crc1[0])
f.close()

print "injecting CRC32"
f = open(sys.argv[1], "wb")
alldata = alldata + crc_bin
f.write(alldata)
f.close()
