#!/usr/bin/python

import RPi.GPIO as gpio
from time import sleep

latchPin = 11
clockPin = 13
dataPin = 15
a0Pin = 7
ePin = 10
resPin = 12

gpio.setmode(gpio.BOARD)
gpio.setup(latchPin, gpio.OUT)
gpio.setup(clockPin, gpio.OUT)
gpio.setup(dataPin, gpio.OUT)
gpio.setup(a0Pin, gpio.OUT)
gpio.setup(ePin, gpio.OUT)
gpio.setup(resPin, gpio.OUT)

gpio.output(clockPin, False)

row1 = [0xF8,0xFC,0xFE,0xFB,0xEF,0x7F,0x1F,0x0F,0x0F,0x3F,
        0x1F,0x0F,0xBE,0x0C,0x18,0xE0,0x00,0x00,0x08,0x88,
        0xE8,0xE8,0xE8,0x08,0x88,0xE8,0xE8,0xE8,0x08,0x08,
        0xC8,0xE8,0xE8,0x68,0x68,0x68,0x68,0xE8,0xC8,0xC8,
        0x88,0x08,0x08,0x08,0x08,0x88,0xC8,0xE8,0xE8,0xE8,
        0x68,0x68,0x68,0x68,0xE8,0xE8,0xE8,0x68,0x68,0x68,
        0x68]
row2 = [0x0F,0x18,0x27,0x4F,0x43,0x80,0x80,0x80,0x80,0x80,
        0x80,0x40,0x40,0xE8,0xFC,0xFF,0x7C,0x1E,0x0F,0x07,
        0xFF,0xFF,0xFF,0x1F,0x0F,0xFF,0xFF,0xFF,0x00,0x20,
        0x70,0x70,0xE6,0xCE,0xCE,0xCE,0xCE,0xE4,0x71,0x3F,
        0xFF,0xF0,0xF8,0x7C,0x3F,0x0F,0x07,0xFF,0xFF,0xFF,
        0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
        0x00]

models = []
models.append([0, 60, 66, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 66, 60, 0]) #0
models.append([0, 4, 12, 20, 36, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0]) #1
models.append([0, 60, 66, 129, 129, 1, 1, 2, 4, 8, 16, 32, 64, 128, 255, 0]) #2
models.append([0, 60, 66, 129, 1, 1, 2, 12, 2, 1, 1, 1, 129, 66, 60, 0]) #3
models.append([0, 6, 10, 10, 18, 18, 34, 34, 66, 66, 130, 255, 2, 2, 2, 0]) #4
models.append([0, 254, 128, 128, 128, 128, 188, 194, 1, 1, 1, 129, 129, 66, 60, 0]) #5
models.append([0, 60, 66, 129, 128, 128, 188, 194, 129, 129, 129, 129, 129, 66, 60, 0]) #6
models.append([0, 255, 1, 2, 2, 4, 4, 8, 8, 16, 16, 32, 32, 64, 64, 0]) #7
models.append([0, 60, 66, 129, 129, 129, 66, 60, 66, 129, 129, 129, 129, 66, 60, 0]) #8
models.append([0, 60, 66, 129, 129, 129, 129, 129, 67, 61, 1, 1, 129, 66, 60, 0]) #9

def modelToRow1(model):
    ret = [0, 0, 0, 0, 0, 0, 0, 0]
    for i in xrange(8):
        for j in xrange(8):
            ret[7 - j] += ((model[i] >> j) & 1) * (1 << i)
    return ret;

def modelToRow2(model):
    ret = [0, 0, 0, 0, 0, 0, 0, 0]
    for i in xrange(8):
        for j in xrange(8):
            ret[7 - j] += ((model[i + 8] >> j) & 1) * (1 << i)
    return ret;

def shiftOut(i):
    gpio.output(latchPin, False)
    for j in xrange(8):
        if i & (1 << j) == 0:
            gpio.output(dataPin, False)
        else:
            gpio.output(dataPin, True)
        gpio.output(clockPin, True)
        gpio.output(clockPin, False)
    gpio.output(latchPin, True)

def waitReady():
    sleep(0.001)

def writeByte(b, cd):
    waitReady()
    gpio.output(a0Pin, cd)
    shiftOut(b)
    sleep(0.001)
    gpio.output(ePin, True)
    sleep(0.001)
    gpio.output(ePin, False)
    sleep(0.001)

def writeCode(b):
    writeByte(b, 0)

def writeData(b):
    writeByte(b, 1)

def lcdInit():
    gpio.output(ePin, False)
    gpio.output(resPin, False)
    sleep(0.001)
    gpio.output(resPin, True)
    sleep(0.005)
    writeCode(0xE2) # Reset
    writeCode(0xEE) # ReadModifyWrite off
    writeCode(0xA4) # Usualy mode
    writeCode(0xA8) # Multiplex 1/16
    writeCode(0xC0) # Top row to 0
    writeCode(0xA0) # NonInvert scan RAM
    writeCode(0xAF) # Display on
    
lcdInit()

row11 = modelToRow1(models[1])
row12 = modelToRow2(models[1])
row21 = modelToRow1(models[2])
row22 = modelToRow2(models[2])
row31 = modelToRow1(models[8])
row32 = modelToRow2(models[8])
row41 = modelToRow1(models[4])
row42 = modelToRow2(models[4])
row51 = modelToRow1(models[5])
row52 = modelToRow2(models[5])
row61 = modelToRow1(models[9])
row62 = modelToRow2(models[9])
row71 = modelToRow1(models[7])
row72 = modelToRow2(models[7])

writeCode(0xB8)
writeCode(0x00)
for c in range(0, 61):
    writeData(0)
writeCode(0xB9)
writeCode(0x00)
for c in range(0, 61):
    writeData(0)


writeCode(0xB8)
writeCode(0x00)
for c in range(0, 8):
    writeData(row11[c])
for c in range(0, 8):
    writeData(row21[c])
writeData(0)
for c in range(0, 8):
    writeData(row31[c])
writeData(0)
for c in range(0, 8):
    writeData(row41[c])
writeData(0)
for c in range(0, 8):
    writeData(row51[c])
writeData(0)
for c in range(0, 8):
    writeData(row61[c])
writeData(0)
for c in range(0, 8):
    writeData(row71[c])
writeCode(0xB9)
writeCode(0x00)
for c in range(0, 8):
    writeData(row12[c])
for c in range(0, 8):
    writeData(row22[c])
writeData(0)
for c in range(0, 8):
    writeData(row32[c])
writeData(0)
for c in range(0, 8):
    writeData(row42[c])
writeData(0)
for c in range(0, 8):
    writeData(row52[c])
writeData(0)
for c in range(0, 8):
    writeData(row62[c])
writeData(0)
for c in range(0, 8):
    writeData(row72[c])

gpio.cleanup()
