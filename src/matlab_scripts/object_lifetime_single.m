clear all
close all

%The maximum number of replicas
r = 10;

%The maximum number of nodes
n = 17;

%Repair rate
mu = 0;

%Parameters of a pareto node lifetime distribution
alpha = 2.427;
beta = 632.952;

%Peer departure rate for a pareto distribution with parameters alpha and
%beta
theta = (alpha-2)/beta

%Peer departure rate as measured in simulation
%theta = 0.047; %Data measured from Pithos simulation

%Peer arrival rate with the same arrival distribution as departure
%distribution.
phi = theta; %Data measured from Pithos simulation

expected_lifetimes = object_lifetime(r, n, theta, phi, mu);

plot(n:-1:1, expected_lifetimes);