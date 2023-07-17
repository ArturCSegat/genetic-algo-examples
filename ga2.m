clear all
% Parametros
tpausa = .3;   % tempo para atualizar o gráfico com a próxima geracao
%fitness = @(x)  2*sin(5*x).*x.^2 + 2*x;  % funcao de avaliacao dos individuos
%fitness = @(x)  1.5*cos(4*x)-.1*x.^2 + .3*x;  % funcao de avaliacao dos individuos
%fitness = @(x)  sin(2*x)-abs(0.5*x) ;  % funcao de avaliacao dos individuos
fitness = @(x)  abs(cos(1.5*x)) - 0.001*x.^2;  % funcao de avaliacao dos individuos
tamCrom = 32;  % comprimento do cromossomo
tamPop = 50;   % tamanho da populacao  (o numero deve ser par)
numGera = 80; % numero de geracoes

%  fria o grafico
x = -2*pi :0.01: 2*pi;

figure(1,"position",get(0,"screensize"))

subplot(1,2,2)
hold on
subplot(1,2,1)
hold on
plot(x,fitness(x))
pause
% Decodificacao do cromossomo
  % parametro utilizado no mapeamento do intervalo de numeros gerados e o dominio da funcao
a = 4*pi/ bin2dec(num2str(ones(1,tamCrom)));
f = @(x,a) a*x - 2*pi;

% Geracao da populacao inicial

pop = [];
avaliaPop = [];

for i = 1 : tamPop
   v  = round(rand(1,tamCrom));
   x = f(bin2dec(num2str(v)),a);
   y = fitness(x);

   avaliaPop(i) = y;
   pop(i,:) = v;
end

for i = 1 : tamPop
   xx(i) = f(bin2dec(num2str(pop(i,:))),a);
end
subplot(1,2,1)
hd = plot(xx,avaliaPop,'.r','markersize',30);
subplot(1,2,2)
plot(0,sum(avaliaPop)/tamPop,'*b');
hold on
pause(tpausa);

for  geracao = 1 : numGera
   % Selecao (por torneio) e recombinação
   filhos = [];
   avaliaFilhos = [];

   % cria roleta
   [ordena,ordena] = sort(avaliaPop);
   minimo = min(avaliaPop);
   if minimo < 0
      minimo = abs(minimo);
   else
      minimo = 0;
   end
   soma = sum((avaliaPop+minimo));
   if soma > 0
      roleta = (avaliaPop(ordena(1))+minimo)/soma;
      roleta
      for i = 2 : tamPop
         roleta(i) = (avaliaPop(ordena(i))+minimo)/soma + roleta(i-1);
      end
   else
      roleta = zeros(size(avaliaPop));
   end
   for i = 1 : tamPop/2

      %selecao  pai 1
      tiro = rand;
      if tiro < roleta(1)
         pai1 = pop(ordena(1),:);
      else
         for j = 2 : tamPop
            if tiro > roleta(j-1) && tiro < roleta(j)
               pai1 = pop(ordena(j),:);
               break;
            end
            j = j + 1;
         end
      end
      %selecao  pai 2
      tiro = rand;
      if tiro < roleta(1)
         pai2 = pop(ordena(1),:);
      else
         for j = 2 : tamPop
            if tiro > roleta(j-1) && tiro < roleta(j)
               pai2 = pop(ordena(j),:);
               break;
            end
            j = j + 1;
         end
      end

      %recombinacao
      ponto = randi(tamCrom-1);

      filho1 = [ pai1(1:ponto)  pai2(ponto+1:end) ];
        filho2 = [ pai2(1:ponto)  pai1(ponto+1:end) ];

      x1 = f(bin2dec(num2str(filho1)),a);
      y1 = fitness(x1);

      x2 = f(bin2dec(num2str(filho2)),a);
      y2 = fitness(x2);

      filhos(end+1,:) = filho1;
      filhos(end+1,:) = filho2;
      avaliaFilhos(end+1) = y1;
      avaliaFilhos(end+1) = y2;
   end

   newPop = [pop ; filhos];
   avaliaNewPop = [ avaliaPop  avaliaFilhos];

   [ordena,ordena] = sort(avaliaNewPop,'descend');

   pop = [ newPop(ordena(1),:); filhos(2:end,:)];

   avaliaPop = [avaliaNewPop(ordena(1)) avaliaFilhos(2:end)] ;

   for i = 1 : tamPop
      xx(i) = f(bin2dec(num2str(pop(i,:))),a);
   end
   subplot(1,2,1)
   delete(hd)
   hd = plot(xx,avaliaPop,'.r','markersize',30);
   subplot(1,2,2)
   plot(geracao,sum(avaliaPop)/tamPop,'*b');
   %avaliaPop(1:10)

   pause(tpausa)
   media = sum(avaliaPop)/length(avaliaPop);
   geracao
   media
end

max(avaliaPop)
