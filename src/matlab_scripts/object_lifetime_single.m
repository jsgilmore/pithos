clear all
close all

%Type can be either 'pareto' or 'exponential'
type = 'exponential';

%The maximum number of replicas
r = 3;

%The maximum number of nodes in the larger network
n = 7;

group_size_av = 5;

%Repair rate
mu = 0.5;

%Parameters of a pareto node lifetime distribution
% alpha = 0.5239
% beta = 48.4389

%Parameter of an exponential distribution
lambda = 1800

%Peer departure rate for a pareto distribution with parameters alpha and
%beta from the referenced research paper. (This formula cannot be verified
%and makes little sense, since a Pareto distribution has a variable failure
%rate.
if strcmp(type, 'pareto')
    %This is an approximation that was made in the literature and has not
    %been found to be accurate in practise. Actually, the object lifetimes
    %are just too sensitive to small values of change of the departure rate
    %parameter.
    theta = (alpha-2)/beta;
else
    theta = 1/lambda;
end

%Peer arrival rate with the same arrival distribution as departure
%distribution.
%phi = theta;
phi = group_size_av*theta/(n - group_size_av)

expected_lifetimes = object_lifetime(r, n, theta, phi, mu);

plot(expected_lifetimes);