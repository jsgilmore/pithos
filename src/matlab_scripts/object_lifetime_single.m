clear all
close all

%The maximum number of replicas
r = 10;

%The maximum number of nodes
n = 100;

%Repair rate
mu = 1/500;

%Parameters of a pareto node lifetime distribution
alpha = 2.4147;
beta = 142.5893;

%Peer departure rate for a pareto distribution with parameters alpha and
%beta
%theta = (alpha-2.225)/beta
theta = (alpha-2)/beta

%Peer departure rate as measured in simulation
%theta = 0.00032; %Data measured from Pithos simulation

%Peer arrival rate with the same arrival distribution as departure
%distribution.
phi = theta; %Data measured from Pithos simulation

expected_lifetimes = object_lifetime(r, n, theta, phi, mu);

plot(n:-1:1, expected_lifetimes);