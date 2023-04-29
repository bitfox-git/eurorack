import math

inputf = open("./Project/ledlut.h", "w")

inputf.write("#ifndef TWIDDLES_H_ \n")
inputf.write("#define TWIDDLES_H_ \n")
inputf.write("#include \"kiss_fft.h\" \n")
inputf.write("\n")


# fftsize = 512
# inputf.write("const kiss_fft_cpx r_twiddles512[] = { \n")
# valueperline = 8
# for n in range(fftsize//valueperline):
#   for j in range(valueperline):
#     index = n*valueperline+j 
#     phase = (math.pi*(index+1)/fftsize) + .5
#     inputf.write("{" + "{:.25f}".format(math.cos(phase)) + ", " + "{:.25f}".format(math.sin(phase)) + "},")
    
#   inputf.write("\n")
# inputf.write("}; \n")


# fftsize = 512
# inputf.write("const kiss_fft_cpx c_twiddles512[] = { \n")
# for n in range(fftsize//valueperline):
#   for j in range(valueperline): 
#     index = n*valueperline+j
#     phase = -2*math.pi*index/fftsize
#     inputf.write("{" + "{:.25f}".format(math.cos(phase)) + ", " + "{:.25f}".format(math.sin(phase)) + "},")
#   inputf.write("\n")
# inputf.write("}; \n")

# fftsize = 512
# inputf.write("const kiss_fft_cpx ri_twiddles512[] = { \n")
# valueperline = 8
# for n in range(fftsize//valueperline):
#   for j in range(valueperline):
#     index = n*valueperline+j 
#     phase = (math.pi*(index+1)/fftsize) + .5
#     phase *= -1
#     inputf.write("{" + "{:.25f}".format(math.cos(phase)) + ", " + "{:.25f}".format(math.sin(phase)) + "},")
    
#   inputf.write("\n")
# inputf.write("}; \n")

# fftsize = 512
# inputf.write("const kiss_fft_cpx ci_twiddles512[] = { \n")
# for n in range(fftsize//valueperline):
#   for j in range(valueperline): 
#     index = n*valueperline+j
#     phase = -2*math.pi*index/fftsize
#     phase *= -1
#     inputf.write("{" + "{:.25f}".format(math.cos(phase)) + ", " + "{:.25f}".format(math.sin(phase)) + "},")
#   inputf.write("\n")
# inputf.write("}; \n")

#         #   for (i=0;i<nfft;++i) {
#         #     const double pi=3.141592653589793238462643383279502884197169399375105820974944;
#         #     double phase = -2*pi*i / nfft;
#         #     if (st->inverse)
#         #         phase *= -1;
#         #     kf_cexp(st->twiddles+i, phase );
#         # }
#         #   double phase =
#         #     -3.14159265358979323846264338327 * ((double) (i+1) / nfft + .5);
#         # if (inverse_fft)
#         #     phase *= -1;




# Sine LUT
inputf.write("const uint8_t slowledlut[] = { \n")
lutsize = 512
height = 206
valueperline = 16
for n in range( lutsize//valueperline):
  for j in range(valueperline): 
    index = n*valueperline+j
    phase = (height-1)*(math.sin(math.pi*(index)/lutsize)**2)
    inputf.write(str(int(phase)) + ", ")
  inputf.write("\n")
inputf.write("}; \n")

inputf.write("const uint8_t fastledlut[] = { \n")
lutsize = 512
height = 206
valueperline = 16
for n in range( lutsize//valueperline):
  for j in range(valueperline): 
    index = n*valueperline+j
    phase = (height-1)*(math.sin(3*math.pi*(index)/lutsize)**2)
    inputf.write(str(int(phase)) + ", ")
  inputf.write("\n")
inputf.write("}; \n")


# inputf.write("\n")




# inputf.write("const float envelopeexpo[] = { \n")
# exposize = 4096
# for n in range(exposize//16):
#   for j in range(16):
#     x = 5*(n*16+j)/exposize
#     y = 0.03*((1.2**(5.61862017*x))-1)
#     inputf.write(str(y) + ", ")

#   inputf.write("\n")

# inputf.write("5.0f }; \n")



# inputf.write("\n")



# samplerate = 24000
# listtime = []
# currentindex = 8
# for n in range(256):
#   listtime.append((20*(1.0283**n-1)+1)/1000)

# inputf.write("const float envelopetime[] = { \n")
# timingsize = 256
# for n in range(timingsize//16):
#   for j in range(16):
#     samples = listtime[n*16+j]*samplerate
#     time = 5/samples
#     inputf.write(str(time) + ", ")

#   inputf.write("\n")
# inputf.write("}; \n")


inputf.write("\n")
inputf.write("#endif // TWIDDLES_H_")
