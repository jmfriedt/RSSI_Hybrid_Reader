df=0.25   % frequency step in MHz
T=1/df    % measurement duration in us
filelist=dir('./output*txt');
for filenum=1:length(filelist)
   d=load(filelist(filenum).name);
   d=d(1:length(d)/2)*j+d(length(d)/2+1:end); % Im then Re
   plot(linspace(0,T,length(d)),20*log10(abs(ifft(d))))
   hold on
end
xlim([0 5]);xlabel('time (microsecond)')
ylabel('returned signal (dB)')
