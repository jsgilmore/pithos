%Pareto distribution parameter estimation
clear all
close all

%The folder to load the data from
% folder = 'none_10_small_single_3';
% 
% node_lifetime_str = sprintf('%s/node_lifetime.csv', folder);
% eval(['node_lifetime = csvread(''' node_lifetime_str ''');']);

node_lifetime_vs_time = csvread('node_lifetimes.csv');
node_lifetime = node_lifetime_vs_time(:,2);

hist(node_lifetime,1000);
[n,x] = hist(node_lifetime,1000);
f_sim = n/sum(n)/diff(x(1:2));

figure
plot(x,f_sim)
figure
plot(x,f_sim)

%Mean
m = mean(node_lifetime);
%Variance
v = var(node_lifetime);

alpha = 2/(1 - m^2/v)
%alpha = 0.5239
beta = m*(v + m^2)/(v - m^2)
%beta = 48.4389
eta = 1/alpha;
sigma = beta/alpha;

theta = (alpha-2.175)/beta

f = 1/sigma*(1+eta.*(x)./sigma).^(-1/eta-1);
F = 1 - (1 + x./beta).^(-alpha);

hazard_rate = f./(1-F);
mean(hazard_rate)

error = abs(f_sim-f);

hold on
plot (x, f, 'k');
plot (x, error, 'r');
figure
plot (x, F);