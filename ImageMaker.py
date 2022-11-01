import sys
import struct


def adjustInSectorSize(sourceData):
    sizeToAdjust=len(sourceData)%512
    if sizeToAdjust==0:
        sizeToAdjust=0
    else:
        sizeToAdjust=512-sizeToAdjust
    return bytearray(bytearray(sourceData)+(bytearray(sizeToAdjust)))

if len(sys.argv)<4:
    sys.stderr.write('[Error] ImageMaker BootLoader.bin Kernel32.bin\n')
    sys.exit(-1)


sourceFile=open(sys.argv[2], 'r')
srcData=sourceFile.read()
sourceFile.close()

bootLoaderData=adjustInSectorSize(srcData)

print("The size of %s is adjusted from %d to %d bytes"%(sys.argv[2], len(srcData), len(bootLoaderData)))

sourceFile=open(sys.argv[3], 'r')
srcData=sourceFile.read()
sourceFile.close()

Kernel32Data=adjustInSectorSize(srcData)
Kernel32sectorCount=len(Kernel32Data)/512
print("The size of %s is adjusted from %d to %d bytes and sector count is %d"%(sys.argv[3], len(srcData), len(Kernel32Data), Kernel32sectorCount))

(sectorCountRaed,)=struct.unpack('<H',bootLoaderData[7:9])
bootLoaderData[7:9]=struct.pack('<H', Kernel32sectorCount)

print("The Kernel32 sector count becomes to %d from %d"%(Kernel32sectorCount, sectorCountRaed))

Kernel64Data=bytearray();
if len(sys.argv)>=5:
    sourceFile=open(sys.argv[4], 'r')
    srcData=sourceFile.read()
    sourceFile.close()
    Kernel64Data=adjustInSectorSize(srcData)
    Kernel64SectorCount=len(Kernel64Data)/512
    print("The size of %s is adjusted from %d to %d bytes and sector count is %d"%(sys.argv[4], len(srcData), len(Kernel64Data), Kernel64SectorCount))

    totalSectorCount=Kernel32sectorCount+Kernel64SectorCount
    (sectorCountRaed,)=struct.unpack('<H',bootLoaderData[5:7])
    bootLoaderData[5:7]=struct.pack('<H', totalSectorCount)

    print("The Total sector count becomes to %d from %d"%(totalSectorCount, sectorCountRaed))    


targetFile=open(sys.argv[1], 'w')
targetFile.write(bytes(bootLoaderData+Kernel32Data+Kernel64Data))
targetFile.close()



