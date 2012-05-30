clear all
close all

%Type can be either 'pareto' or 'exponential'
type = 'exponential';

%The maximum number of replicas
R = 10;

%The maximum number of nodes
N = 15;

%Time to live
TTL = 172800;

%Repair rate
MU = 1/500;
mu_step = 1/5000;

%Parameters of a pareto node lifetime distribution (these values are
%ignored if the specified distribution is exponential)
alpha = 2.4266;
beta = 632.9519;

%Parameter of an exponential distribution (these values are ignored
%if the specified distribution is pareto)
lambda = 100.8

%Peer departure rate for a pareto distribution with parameters alpha and
%beta
if strcmp(type, 'pareto')
    theta = (alpha-2)/beta;
else strcmp(type, 'exponential')
    theta = 1/lambda;
end

%Peer departure rate as measured in simulation
%theta = 0.046518; %Data measured from Pithos simulation

%Peer arrival rate with the same arrival distribution as departure
%distribution.
phi = theta; %Data measured from Pithos simulation

lifetimes = zeros(R, N, MU/mu_step);

x = 1;

for r = 1:R
    for n=r:N
        for mu=0:mu_step:MU
            expected_lifetimes = object_lifetime(r, n, theta, phi, mu);
            
            %Expected lifetime with initial group size larger than the
            %number of replicas
            lifetimes(r, n, round(mu/mu_step+1)) = expected_lifetimes(1);
            
            if expected_lifetimes(1) > TTL
                lifetime_slice(x,1) = r;
                lifetime_slice(x,2) = n;
                lifetime_slice(x,3) = mu;
                x = x+1;
            end
        end
    end
end

surf(lifetimes(:, :, MU/mu_step));
figure
surf(squeeze(lifetimes(:, N, :)));
figure
surf(squeeze(lifetimes(R, :, :)));
figure
scatter3(lifetime_slice(:,1), lifetime_slice(:,2), lifetime_slice(:,3));