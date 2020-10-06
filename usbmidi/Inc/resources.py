import math

inputf = open("./Inc/resources.h", "w")

inputf.write("#ifndef RESOURCES_H_ \n")
inputf.write("#define RESOURCES_H_ \n")
inputf.write("\n")

inputf.write("const float sinelut[] = { \n")

# Sine LUT
lutsize = 65536//(4*4*2)
valueperline = 16
for n in range( lutsize//valueperline):
  for j in range(valueperline): 
    inputf.write(str(math.sin(2*math.pi*(n*valueperline+j)/lutsize)) + ", ")

  inputf.write("\n")


inputf.write("}; \n")




inputf.write("\n")




inputf.write("const float envelopeexpo[] = { \n")
exposize = 4096
for n in range(exposize//16):
  for j in range(16):
    x = 5*(n*16+j)/exposize
    y = 0.03*((1.2**(5.61862017*x))-1)
    inputf.write(str(y) + ", ")

  inputf.write("\n")

inputf.write("5.0f }; \n")



inputf.write("\n")



samplerate = 24000
listtime = []
currentindex = 8
for n in range(256):
  listtime.append((20*(1.0283**n-1)+1)/1000)

inputf.write("const float envelopetime[] = { \n")
timingsize = 256
for n in range(timingsize//16):
  for j in range(16):
    samples = listtime[n*16+j]*samplerate
    time = 5/samples
    inputf.write(str(time) + ", ")

  inputf.write("\n")
inputf.write("}; \n")


inputf.write("\n")
inputf.write("#endif // RESOURCES_H_")
