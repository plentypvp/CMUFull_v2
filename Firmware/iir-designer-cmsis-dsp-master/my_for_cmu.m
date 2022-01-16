pkg load signal
order=2
f1=15
fs=31250
plot_results=false

design_iir_highpass_cmsis_butter(order,f1,fs,plot_results);

order=6
f1=200
fs=31250
plot_results=false

design_iir_lowpass_cmsis_butter(order,f1,fs,plot_results);

order=6
ripple=2
att=40
f1=600
f2=3000
fs=31250
plot_results=false

design_iir_bandpass_cmsis_elliptical(order,ripple,att,f1,f2,fs,plot_results);

order=6
f1=4500
fs=31250
plot_results=false

design_iir_highpass_cmsis_butter(order,f1,fs,plot_results);
