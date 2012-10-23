clear all
close all

%Type can be either 'pareto' or 'exponential'
type = 'exponential';

%The maximum number of replicas
R = 10;

%The maximum expected network size
N = 120;

%The maximum average group size
G = 50;

%Repair rate
MU = 1/180;
mu_step = 1/1800;

%Time to live
TTL = 86400;

%Parameters of a pareto node lifetime distribution (these values are
%ignored if the specified distribution is exponential)
% alpha = 2.4266;
% beta = 632.9519;

%Parameter of an exponential distribution (these values are ignored
%if the specified distribution is pareto)
lambda = 1/1800

%Peer departure rate for a pareto distribution with parameters alpha and
%beta
if strcmp(type, 'pareto')
    theta = (alpha-2)/beta;
else strcmp(type, 'exponential')
    theta = lambda;
end

%Create a four-dimensional array to house all the data.
lifetimes = zeros(N, R, G, MU/mu_step);

x = 1;

for r = 1:R
    for g=1:G
        phi = g*theta/(N-g);
        
        for mu=0:mu_step:MU
            
            expected_lifetimes = object_lifetime(r, N, theta, phi, mu);
            
            %Expected lifetime with initial group size larger than the
            %number of replicas
            lifetimes(:, r, g, round(mu/mu_step+1)) = expected_lifetimes;
            %lifetimes(r, n, round(mu/mu_step+1)) = expected_lifetimes(1);
            
            if mean(expected_lifetimes) > TTL
                lifetime_slice(x,1) = r;
                lifetime_slice(x,2) = g;
                lifetime_slice(x,3) = mu;
                x = x+1;
            end
        end
    end
end

figure
surf(squeeze(lifetimes(:, R, :, MU/mu_step)));
xlabel('Average group size');
ylabel('Initial group size');
zlabel('Expected object lifetime');

figure
scatter3(lifetime_slice(:,1), lifetime_slice(:,2), lifetime_slice(:,3));
xlabel('Number of replicas');
ylabel('Average group size');
zlabel('Repair rate');
title('3D scatter plot of parameters that satisfy required time-to-live.');