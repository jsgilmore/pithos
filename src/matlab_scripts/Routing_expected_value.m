close all
clear all

x = 4;          %Fan-out degree of the DHT ring

%This is just the solution to the problem of how many nodes are required in
%a group, for the hierarchical routing to perform better then pure overlay
%routing. (Where is log(n) = log(n/b) + 2)
group_boundary = 2^x^2

b = 50;         %Average group size

%Average number of hops for various probabilities of group messages
p_g = 0:0.01:1;
n = 100;

lines = zeros(1,101);
lines = lines + 1;

while n < 100001
    
    %This is to use hops and draw multiple lines according to Pastry's
    %order complexity
    X_simple = p_g + (1 - p_g)*(log(n/b)/log(2^x) + 2);
    X_pastry = lines.*(log(n)/log(2^x));
    
    %This is for working with measured expected values
    %X_simple = p_g*0.0878 + (1 - p_g)*0.3284;
    %X_pastry = lines.*(0.3284);     %This number should still be measured in Pastry
    
    n = n*10;
    
    %plot(p_g, X);
    hold on;
    plot(p_g, X_simple, 'r');
    plot(p_g, X_pastry, 'k');
end;

%Average number of hops against number of nodes in the network.
figure
n = 1:100000;

for p_g = 0:0.2:1
    %X = p_g + (1 - p_g)/b*((log(n/b)/log(2^x) + 1)*(b-1) + log(n/b)/log(2^x));
    X_simple = p_g + (1 - p_g)*(log(n/b)/log(2^x) + 2);
    
    semilogx(n, X_simple, 'r');
    hold on;
end;

X_pastry = log(n)/log(2^x);
semilogx(n, X_pastry, 'k');