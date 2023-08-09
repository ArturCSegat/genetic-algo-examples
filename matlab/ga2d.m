clear all
% Parametros
tpausa = .2;   % tempo para atualizar o gráfico com a próxima geracao
%fitness = @(x,y)  .1*(x.^4 + y.^4) + sin(3*x) + cos(3*y) +  (-x.^2 - y.^2);  % funcao de avaliacao dos individuos
fitness = @(x,y)  sin(5*x)+cos(5*y)-0.6*(x.^2+y.^2);  % funcao de avaliacao dos individuos
%fitness = @(x,y)  2*sin(5*x).*sin(7*y) + 10+-(x.^2+y.^2);  % funcao de avaliacao dos individuos
%fitness = @(x,y)   + 10+-(x.^2+y.^2);  % funcao de avaliacao dos individuos
tamCrom = 16;  % comprimento do cromossomo
tamPop = 20;   % tamanho da populacao  (o numero deve ser par)
numGera = 80; % numero de geracoes

%  cria o grafico
x = -2 :0.05: 2;
y = x;
%figure(1,"position",get(0,"screensize"))

[X,Y] = meshgrid(x,y);
Z=fitness(X,Y);
mesh(X, Y, Z)
hold on
pause

% Decodificacao do cromossomo
  % parametro utilizado no mapeamento do intervalo de numeros gerados e o dominio da funcao
a = 4/(bin2dec(num2str(ones(1,tamCrom))));
f = @(x,a) a*x - 2;

% Geracao da populacao inicial

pop = [];
avaliaPop = [];

for i = 1 : tamPop
   v  = round(rand(1,2*tamCrom));
   x = f(bin2dec(num2str(v(1:tamCrom))),a);
   y = f(bin2dec(num2str(v(tamCrom+1:end))),a);
   z = fitness(x,y);

   avaliaPop(i) = z;
   pop(i,:) = v;
end

for i = 1 : tamPop
   xx(i) = f(bin2dec(num2str(pop(i,1:tamCrom))),a);
   yy(i) = f(bin2dec(num2str(pop(i,tamCrom+1:end))),a);
end
hd = plot3(xx,yy,avaliaPop,'.r','markersize',20);
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

      filho1x = [ pai1(1:ponto)  pai2(ponto+1:tamCrom) ];
      filho1y = [ pai1(tamCrom+1:ponto+tamCrom)  pai2(tamCrom+1+ponto:end) ];
      filho1 = [ filho1x filho1y];

      filho2x = [ pai2(1:ponto)  pai1(ponto+1:tamCrom) ];
      filho2y = [ pai2(tamCrom+1:ponto+tamCrom)  pai1(tamCrom+1+ponto:end) ];
      filho2 = [ filho2x filho2y];

      x1 = f(bin2dec(num2str(filho1(1:tamCrom))),a);
      y1 = f(bin2dec(num2str(filho1(tamCrom+1:end))),a);
      z1 = fitness(x1,y1);

      x2 = f(bin2dec(num2str(filho2(1:tamCrom))),a);
      y2 = f(bin2dec(num2str(filho2(tamCrom+1:end))),a);
      z2 = fitness(x2,y2);

      filhos(end+1,:) = filho1;
      filhos(end+1,:) = filho2;
      avaliaFilhos(end+1) = z1;
      avaliaFilhos(end+1) = z2;
   end

   newPop = [pop ; filhos];
   avaliaNewPop = [ avaliaPop  avaliaFilhos];

   [ordena,ordena] = sort(avaliaNewPop,'descend');

   pop = [ newPop(ordena(1),:); filhos(2:end,:)];

   avaliaPop = [avaliaNewPop(ordena(1)) avaliaFilhos(2:end)] ;

   for i = 1 : tamPop
      xx(i) = f(bin2dec(num2str(pop(i,1:tamCrom))),a);
      yy(i) = f(bin2dec(num2str(pop(i,tamCrom+1:end))),a);
   end
   delete(hd)
   hd = plot3(xx,yy,avaliaPop,'.r','markersize',30);
   pause(tpausa)
   media = sum(avaliaPop)/length(avaliaPop);
   geracao
   media
end

max(avaliaPop)
