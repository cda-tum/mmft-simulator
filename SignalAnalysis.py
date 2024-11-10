import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

signal1 = np.genfromtxt('build/Errors_baseline_1a.csv', delimiter=',')
signal2 = np.genfromtxt('build/Errors_0.01_1a.csv', delimiter=',')
signal3 = np.genfromtxt('build/Errors_0.001_1a.csv', delimiter=',')
signal4 = np.genfromtxt('build/Errors.csv', delimiter=',')

### All power spectral densities of baseline ###

#for i in range(1,13):
    #freq1 = np.fft.fft(signal1[1:300,i])*np.hanning(signal1[1:300,1].size)
    #plt.plot(freq1)

#plt.show()

### Power spectral density of baseline and comparison error ###

#freq1 = np.fft.fft(signal1[1:300,2])*np.hanning(signal1[1:300,1].size)
#freq2 = np.fft.fft(signal2[1:300,2])*np.hanning(signal2[1:300,1].size)

#plt.axhline(y=0.0, color='black', linestyle='-')
#plt.plot(freq1, 'r')
#plt.plot(freq2, 'b')
#plt.show()

### Nyquist plot

#freq1 = np.fft.fft(signal1[1:300,2])*np.hanning(signal1[1:300,1].size)
#Real = np.real(freq1)
#Imag = np.imag(freq1)

#plt.plot(Real, Imag, 'bx')
#plt.show()

### Frequency Response Function ###

freq1 = signal1[0:400,1]
freq2 = signal1[0:400,2]
freq3 = signal1[0:400,3]
freq4 = signal1[0:400,4]

w21, h21 =  signal.freqz(freq2, freq1)
w31, h31 =  signal.freqz(freq3, freq1)
w41, h41 =  signal.freqz(freq4, freq1)
w42, h42 =  signal.freqz(freq4, freq2)

freq1_d = signal2[0:2000,1]
freq2_d = signal2[0:2000,2]
freq3_d = signal2[0:2000,3]
freq4_d = signal2[0:2000,4]

w21_d, h21_d =  signal.freqz(freq2_d, freq1_d)
w41_d, h41_d =  signal.freqz(freq4_d, freq1_d)
w42_d, h42_d =  signal.freqz(freq4_d, freq2_d)

freq1_dd = signal3[0:2000,1]
freq2_dd = signal3[0:2000,2]
freq3_dd = signal3[0:2000,3]
freq4_dd = signal3[0:2000,4]

w21_dd, h21_dd =  signal.freqz(freq2_dd, freq1_dd)
w41_dd, h41_dd =  signal.freqz(freq4_dd, freq1_dd)
w42_dd, h42_dd =  signal.freqz(freq4_dd, freq2_dd)

freq1_opt = signal4[0:350,1]
freq2_opt = signal4[0:350,2]
freq3_opt = signal4[0:350,3]
freq4_opt = signal4[0:350,4]

w21_opt, h21_opt =  signal.freqz(freq2_opt, freq1_opt)
w41_opt, h41_opt =  signal.freqz(freq4_opt, freq1_opt)
w42_opt, h42_opt =  signal.freqz(freq4_opt, freq2_opt)

plt.plot(np.abs(h41), label="41 - alpha 1e-1")
plt.plot(np.abs(h41_d), label="41 - alpha 1e-2")
plt.plot(np.abs(h41_dd), label="41 - alpha 1e-3")
plt.plot(np.abs(h41_opt), label="41 - opt")
#plt.plot(np.abs(h31))
#plt.plot(np.abs(h42), label="42 - alpha 1e-1")
#plt.plot(np.abs(h42_d), label="42 - alpha 1e-2")
#plt.plot(np.abs(h42_dd), label="42 - alpha 1e-3")
#plt.plot(np.abs(h42_opt), label="42 - opt")
# plt.plot(np.abs(h71))
# plt.plot(np.abs(h81))
# plt.plot(np.abs(h91))
# plt.plot(np.abs(h101))
# plt.plot(np.abs(h111))
# plt.plot(np.abs(h121))

#plt.plot(np.abs(h21_d))

plt.yscale('log')

#Real = np.real(h31*1j*np.arange())
#Imag = np.imag(h31*1j)
#plt.plot(Real, Imag)
plt.legend()
plt.show()