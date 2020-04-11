tau = 0.125;
fs = 44100;

Messer = PegelmesserPrototyp(tau,fs);

for kk = 1:round(44100*tau*8)
    RMS = Messer.updateRMS(1/sqrt(2) + 0.25*randn(1,1));
end