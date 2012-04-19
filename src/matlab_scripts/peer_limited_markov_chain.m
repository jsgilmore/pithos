close all
clear all

r = 15;
n = 450;

%The parameters of a pareto distribution
alpha = 3;
beta = 2;

%Peer departure rate for a pareto distribution with parameters alpha and
%beta
theta = (alpha-2)/beta

%Peer arrival rate with the same arrival distribution as departure
%distribution.
phi = theta

mu = 2

x = 1;

for i = n:-1:1
    for j = min(r,i):-1:1
        s(x,2) = i;
        s(x,1) = j;
        x = x+1;
    end
end

disp('Finished generating states')

%Initialise the transient rate matrix
Q = zeros(0.5*r*(2*n-r+1), 0.5*r*(2*n-r+1));

%Initialise the row sum vector
rates = zeros(0.5*r*(2*n-r+1), 1);

%Calculate the transient rate matrix
for i=1:0.5*r*(2*n-r+1)
    for j=1:0.5*r*(2*n-r+1)
        %Nodes added to the group (no extra replicas)
        if s(i,1) == s(j,1)
            if s(j,2) == 1+s(i,2)    
                Q(i,j) = phi;
                rates(i) = rates(i) + Q(i,j);
            end
        end
        
        if (s(j, 1) == s(i, 1)-1)
            if (s(j, 2) == s(i, 2)-1)
                Q(i,j) = s(i,1)*theta;
                rates(i) = rates(i) + Q(i,j);
            end
        end
        
        if (s(j, 1) == s(i, 1))
            if (s(j, 2) == s(i, 2)-1)
                Q(i,j) = (s(i,2)-s(i,1))*theta;
                rates(i) = rates(i) + Q(i,j);
            end
        end
        
        if s(i, 1) ~= s(j,1)
            if s(i, 2) == s(j,2)
                if s(j, 1) == min(r, s(j,2))
                    Q(i,j) = mu;
                    rates(i) = rates(i) + Q(i,j);
                end
            end
        end
    end
    
    if s(i,1) == 1
        rates(i) = rates(i) + theta;
    end
end

rates = rates.^(-1);

disp('Finished creating rate matrix and average duration vector')

%Normalised transient rate matrix
Q_norm = zeros(0.5*r*(2*n-r+1), 0.5*r*(2*n-r+1));

%Normalise the transition rate matrix, keeping in mind that states that
%would leak to absorbtion states also have to be taken into account.
for i=1:0.5*r*(2*n-r+1)
    for j=1:0.5*r*(2*n-r+1)
            Q_norm(i,j) = Q(i,j)*rates(i);
    end
    str = sprintf('Completed normalising row %d of %d of the rate matrix.', i, 0.5*r*(2*n-r+1));
    disp(str);
end

disp('Finished normalising the rate matrix')

I = eye(0.5*r*(2*n-r+1));

%Calculate the fundamental matrix
A = I - Q_norm;
N = A^(-1);

disp('Finished calculating the fundamental matrix')

E_T = N*rates;
disp('Expected time to absorbtion calculated')

x = 1;
for i=1:0.5*r*(2*n-r+1)
    if s(i,1) == r
        E_T_fullreps(x) = E_T(i);
        x = x+1;
    end
    
    if s(i,1) < r
        if s(i,1) == s(i,2)
            E_T_fullreps(x) = E_T(i);
            x = x+1;
        end
    end
end

semilogy([n:-1:1], E_T_fullreps)