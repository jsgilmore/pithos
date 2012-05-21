%Pareto distribution parameter estimation
clear all
close all

mean = 443.672;
var = 1119651.834;

alpha = 2/(1 - mean^2/var);
beta = mean*(var + mean^2)/(var - mean^2);
eta = 1/alpha;
sigma = beta/alpha;

theta = (alpha-2)/beta

x = 0:0.1:3000;

f = 1/sigma*(1+eta.*x./sigma).^(-1/eta-1);
F = 1 - (1 + x./beta).^(-alpha);

plot (x, f);
figure
plot (x, F);