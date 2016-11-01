clear all
close all
clc

% porta serial
s = serial('COM5');

try
    fopen(s);
catch err
    fclose(instrfind);
    fopen(s)
end

sprintf('Conexão aberta');

% sampling time
Tmax = 50;
Ts = 0.0000001;
figure,
grid on,
xlabel ('Tempo (ms)'), ylabel('Velocidade (RPM)'),
i = 0;
flushinput(s)
fscanf(s);
tic
legend('RPM','Referência','Controle','Erro');
while toc <= Tmax
    
    i = i + 1;
    try
       data_set(i) = str2num(fscanf(s));
       data(i) = str2num(fscanf(s));
       tempo(i) = str2num(fscanf(s));
       
    catch err
        flushinput(s)
        fscanf(s);
        sprintf('Erro na medição')
        data_set(i) = str2num(fscanf(s));
        data(i) = str2num(fscanf(s));
        tempo(i) = str2num(fscanf(s));
    end
    
    t(i) = toc;
    if i > 1
        T = toc - t(i-1);
        while T < Ts
            T = toc - t(i-1);
        end
    end
    
    t(i) = toc;
    
    if i > 1
        line([tempo(i-1) tempo(i)],[data(i-1) data(i)], 'linewidth',1.8,'Color',[0,0.7,0.9])
        line([tempo(i-1) tempo(i)],[data_set(i-1) data_set(i)],'linewidth',1.8,'Color',[0.9,0.2,0.3])
        
        drawnow
    end
    
end

fclose(s);