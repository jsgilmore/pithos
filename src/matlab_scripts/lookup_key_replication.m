close all
clear all

%The parameters of a pareto distribution
alpha = 3;
beta = 2;

%The departure rate of peers under steady state
theta = (alpha-2)/beta;

%Replication rate
mu = 0.00000000000000000000000000000000000000000000000000000000000000001;
    
%Replication factor sweep
for r = 1:30
    %Calculate the determinant of A = I - Q
    deta = 1;

    for k = 1:r-1
        deta = deta*k*theta/(k*theta + mu);
    end

    lifetime_rep(r) = 1/deta*(1/r/theta + (1 - deta)/mu);
    
    lifetime_norep(r) = 0;
    for i = 1:r
        lifetime_norep(r) = lifetime_norep(r) + factorial(r)/(factorial(i)*factorial(r-i))*(-1)^i * (beta/(i * (1-alpha) + 1));
    end
end

semilogy(lifetime_rep);
hold on
semilogy(lifetime_norep, 'r');

figure
plot(lifetime_rep);
hold on
plot(lifetime_norep, 'r');