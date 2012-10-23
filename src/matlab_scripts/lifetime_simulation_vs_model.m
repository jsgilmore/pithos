close all
clear all

%Which group's data to load
%The folder to load the data from
folder = '500_10_size15_100_exp_1';

%Number of replications in the simulation
r = 10;

%Repair rate as set in the simulation
repair_rate = 1/500;

%Repair success factor as measured after the simulation
repair_factor = 0.7;

mu = repair_factor*repair_rate;

%A very large number of nodes, which the model approaches in the limit
N = 100;

%--------------------------------------------------------------------------
%Generate the file names to load
%--------------------------------------------------------------------------
initial_groupsize_str = sprintf('%s/initial_groupsize.csv', folder);
max_groupsize_str = sprintf('%s/max_groupsize.csv', folder);
average_groupsize_str = sprintf('%s/average_groupsize.csv', folder);
lifetime_str = sprintf('%s/lifetime.csv', folder);
starve_time_str = sprintf('%s/starve_time.csv', folder);
creation_time_str = sprintf('%s/creation_time.csv', folder);

%--------------------------------------------------------------------------
%Load all data files
%--------------------------------------------------------------------------
%initial_groupsize = csvread('initial_groupsize_none_10_X.csv');
eval(['initial_groupsize = csvread(''' initial_groupsize_str ''');']);
%average_groupsize = csvread('average_groupsize_none_10_X.csv');
eval(['average_groupsize = csvread(''' average_groupsize_str ''');']);
%lifetime = csvread('lifetime_none_10_X.csv');
eval(['lifetime = csvread(''' lifetime_str ''');']);
node_lifetimes = csvread('node_lifetimes.csv');


%--------------------------------------------------------------------------
%Create the required data structured beforehand to speed up runtime
%--------------------------------------------------------------------------
initial_size_objects = [initial_groupsize(:,2) , lifetime(:, 2)];
average_size_objects = [average_groupsize(:, 2), lifetime(:, 2)];

group_size_av = round(mean(average_groupsize(:, 2)));

%This ddeparture rate is only valid when exponential lifetimes are used for the nodes in
%the simulation
theta = 1/mean(node_lifetimes(:,2));

%This value of phi ensure the required average group size
phi = group_size_av*theta/(N - group_size_av);

means = grpstats(initial_size_objects(:, 2), initial_size_objects(:,1));

expected_lifetimes = object_lifetime(r, N, theta, phi, mu);
expected_lifetimes_compared = expected_lifetimes(min(initial_groupsize(:, 2)):max(initial_groupsize(:, 2)));

mean_err = abs(mean(expected_lifetimes_compared-means'))
stdd_err = std(expected_lifetimes_compared-means')
mean_err_percent = abs(mean((expected_lifetimes_compared-means')./means'))*100

%--------------------------------------------------------------------------
%Draw some cool looking graphs
%--------------------------------------------------------------------------
%This only works if one has Matlab's statistical toolbox
set(gca, 'FontSize', 20) 
box = boxplot(initial_size_objects(:, 2), initial_size_objects(:,1), 'notch', 'on', 'whisker', 10);
ylabel('Object lifetime (s)');
xlabel('Initial network size');

%Adjust x-axis sizes and positions of the stupid boxplot object
text_h = findobj(gca, 'Type', 'text');
for cnt = 1:length(text_h)
        set(text_h(cnt), 'FontSize', 20);
        yshift=get(text_h(cnt), 'Position');
        yshift(2)=-35;
        set(text_h(cnt), 'Position', yshift);
end

hold on
plot(means, 'rx');
plot(expected_lifetimes(min(initial_groupsize(:, 2)):max(initial_groupsize(:, 2))), 'k');