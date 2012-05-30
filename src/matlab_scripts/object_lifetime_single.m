clear all
close all

%Type can be either 'pareto' or 'exponential'
type = 'exponential';

%The maximum number of replicas
r = 10;

%The maximum number of nodes
n = 7;

%Repair rate
mu = 1/500;

%Parameters of a pareto node lifetime distribution
alpha = 0.5239
beta = 48.4389

%Parameter of an exponential distribution
lambda = 100.8

%Peer departure rate for a pareto distribution with parameters alpha and
%beta from the referenced research paper. (This formula cannot be verified
%and makes little sense, since a Pareto distribution has a variable failure
%rate.
if strcmp(type, 'pareto')
    theta = (alpha-2)/beta;
else
    theta = 1/lambda;
end

%Peer arrival rate with the same arrival distribution as departure
%distribution.
phi = theta; %Data measured from Pithos simulation

expected_lifetimes = object_lifetime(r, n, theta, phi, mu);

plot(n:-1:1, expected_lifetimes);