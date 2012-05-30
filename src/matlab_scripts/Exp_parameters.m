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

lambda = mle(node_lifetime, 'distribution', 'exponential')

f = exppdf(x, lambda);
F = expcdf(x, lambda);

error = abs(f_sim-f);

figure
plot(x,f_sim)
hold on
plot (x, f, 'k');
plot (x, error, 'r');

figure
plot (x, F);

hazard_rate = f./(1-F);
figure
plot(hazard_rate);