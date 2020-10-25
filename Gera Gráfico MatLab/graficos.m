arqTensao = fopen('T10001.txt', 'r');
medicoesTensao = fscanf(arqTensao,'%f');
fclose(arqTensao);

arqCorrente =  fopen('C10001.txt', 'r');
medicoesCorrente = fscanf(arqCorrente, '%f');
fclose(arqCorrente);

% vetor de tempo para 164Khz
x = 0.0:0.042:100;
% quantidade de amostras
L = 2381;

figure(1);
subplot(2,1,1)
% vetor de tensão para 164Khz
y = medicoesTensao(1:L);
plot(x,y);
title('Tensão Elétrica');
xlabel('Tempo (ms)');
ylabel('Amplitude (V)');

subplot(2,1,2)
% vetor de corrente para 164Khz
y = medicoesCorrente(1:L);
plot(x,y);
title('Corrente Elétrica com Fs=35,02kamostras/s');
xlabel('Tempo (ms)');
ylabel('Amplitude (A)');


