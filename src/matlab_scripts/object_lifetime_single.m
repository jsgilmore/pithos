clear all
close all

%The maximum number of replicas
r = 15;

%The maximum number of nodes
n = 100;

%Repair rate
mu = 0;

%Parameters of a pareto node lifetime distribution
alpha = 3;
beta = 2;

%Peer departure rate for a pareto distribution with parameters alpha and
%beta
theta = (alpha-2)/beta

%Peer departure rate as measured in simulation
%theta = 0.001425; %Data measured from Pithos simulation

%Peer arrival rate with the same arrival distribution as departure
%distribution.
phi = theta; %Data measured from Pithos simulation

expected_lifetimes = object_lifetime(r, n, theta, phi, mu);

plot(n:-1:1, expected_lifetimes);