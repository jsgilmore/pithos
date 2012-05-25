close all
clear all

%Which group's data to load
%Set to zero for only a single group with no numbered postfix
number = 0;
%The folder to load the data from
folder = '500_10_small_100_1';
%0 for old naming scheme and 1 for new naming scheme
scheme = 1;

%--------------------------------------------------------------------------
%Generate the file names to load
%--------------------------------------------------------------------------
if number ~= 0
    initial_groupsize_str = sprintf('%s/initial_groupsize_none_10.csv', folder, number);
    max_groupsize_str = sprintf('%s/max_groupsize_none_10.csv', folder, number);
    average_groupsize_str = sprintf('%s/average_groupsize_none_10.csv', folder, number);
    lifetime_str = sprintf('%s/lifetime_none_10.csv', folder, number);
    starve_time_str = sprintf('%s/starve_time_none_10.csv', folder, number);
    creation_time_str = sprintf('%s/creation_time_none_10.csv', folder, number);
else
    if scheme == 0
        initial_groupsize_str = sprintf('%s/initial_groupsize_none_10.csv', folder);
        max_groupsize_str = sprintf('%s/max_groupsize_none_10.csv', folder);
        average_groupsize_str = sprintf('%s/average_groupsize_none_10.csv', folder);
        lifetime_str = sprintf('%s/lifetime_none_10.csv', folder);
        starve_time_str = sprintf('%s/starve_time_none_10.csv', folder);
        creation_time_str = sprintf('%s/creation_time_none_10.csv', folder);
    else
        initial_groupsize_str = sprintf('%s/initial_groupsize.csv', folder);
        max_groupsize_str = sprintf('%s/max_groupsize.csv', folder);
        average_groupsize_str = sprintf('%s/average_groupsize.csv', folder);
        lifetime_str = sprintf('%s/lifetime.csv', folder);
        starve_time_str = sprintf('%s/starve_time.csv', folder);
        creation_time_str = sprintf('%s/creation_time.csv', folder);
    end
end

%--------------------------------------------------------------------------
%If the filename is still in the Omnet format, change it to Matlab stadard
%format
%--------------------------------------------------------------------------
if number ~= 0
    if exist(sprintf('%s/initial_groupsize_none_10-%d.csv', folder, number), 'file')
        movefile(sprintf('%s/initial_groupsize_none_10-%d.csv', folder, number), initial_groupsize_str);
    end
    if exist(sprintf('%s/max_groupsize_none_10-%d.csv', folder, number), 'file')
        movefile(sprintf('%s/max_groupsize_none_10-%d.csv', folder, number), max_groupsize_str);
    end
    if exist(sprintf('%s/average_groupsize_none_10-%d.csv', folder, number), 'file')
        movefile(sprintf('%s/average_groupsize_none_10-%d.csv', folder, number), average_groupsize_str);
    end
    if exist(sprintf('%s/lifetime_none_10-%d.csv', folder, number), 'file')
        movefile(sprintf('%s/lifetime_none_10-%d.csv', folder, number), lifetime_str);
    end
    if exist(sprintf('%s/starve_time_none_10-%d.csv', folder, number), 'file')
        movefile(sprintf('%s/starve_time_none_10-%d.csv', folder, number), starve_time_str);
    end
    if exist(sprintf('%s/creation_time_none_10-%d.csv', folder, number), 'file')
        movefile(sprintf('%s/creation_time_none_10-%d.csv', folder, number), creation_time_str);
    end
end


%--------------------------------------------------------------------------
%Load all data files
%--------------------------------------------------------------------------
%initial_groupsize = csvread('initial_groupsize_none_10_X.csv');
eval(['initial_groupsize = csvread(''' initial_groupsize_str ''');']);
%max_groupsize = csvread('max_groupsize_none_10_X.csv');
eval(['max_groupsize = csvread(''' max_groupsize_str ''');']);
if exist(average_groupsize_str, 'file')
    %average_groupsize = csvread('average_groupsize_none_10_X.csv');
    eval(['average_groupsize = csvread(''' average_groupsize_str ''');']);
end
%lifetime = csvread('lifetime_none_10_X.csv');
eval(['lifetime = csvread(''' lifetime_str ''');']);
if exist(creation_time_str, 'file')
    %creation_time = csvread('creation_time_none_10_X.csv');
    eval(['creation_time = csvread(''' creation_time_str ''');']);
end
if exist(starve_time_str, 'file')
    %starve_time = csvread('starve_time_none_10_X.csv');
    eval(['starve_time = csvread(''' starve_time_str ''');']);
end

bin_siz = 2;

initial_size_objects = [initial_groupsize(:,2) , lifetime(:, 2)];
initial_size_lifetime_counts = zeros(max(initial_groupsize(:,2)), 2);

max_size_objects = [max_groupsize(:, 2), lifetime(:, 2)];
max_size_lifetime_counts = zeros(max(max_groupsize(:, 2)), 2);

if exist(average_groupsize_str, 'file')
    average_size_objects = [average_groupsize(:, 2), lifetime(:, 2)];
end

bins = ceil(length(initial_size_lifetime_counts(:, 1))/bin_siz);
initial_size_lifetime_aggr = zeros(bins, 2);
max_size_lifetime_aggr = zeros(bins, 2);

for i=1:length(initial_size_objects(:,1))
    initial_size_lifetime_counts(initial_size_objects(i, 1), 1) = initial_size_lifetime_counts(initial_size_objects(i, 1), 1) + initial_size_objects(i, 2);
    initial_size_lifetime_counts(initial_size_objects(i, 1), 2) = initial_size_lifetime_counts(initial_size_objects(i, 1), 2) + 1;
    
    max_size_lifetime_counts(max_size_objects(i, 1), 1) = max_size_lifetime_counts(max_size_objects(i, 1), 1) + max_size_objects(i, 2);
    max_size_lifetime_counts(max_size_objects(i, 1), 2) = max_size_lifetime_counts(max_size_objects(i, 1), 2) + 1;
end

% aggr_pos = 1;
% for i=0:length(initial_size_lifetime_counts(:,1))-1
%     aggr_pos = floor(i/bin_siz) + 1;
%     initial_size_lifetime_aggr(aggr_pos, 1) = initial_size_lifetime_aggr(aggr_pos, 1) + initial_size_lifetime_counts(i+1, 1);
%     initial_size_lifetime_aggr(aggr_pos, 2) = initial_size_lifetime_aggr(aggr_pos, 2) + initial_size_lifetime_counts(i+1, 2);
%     
%     max_size_lifetime_aggr(aggr_pos, 1) = max_size_lifetime_aggr(aggr_pos, 1) + max_size_lifetime_aggr(i+1, 1);
%     max_size_lifetime_aggr(aggr_pos, 2) = max_size_lifetime_aggr(aggr_pos, 2) + max_size_lifetime_aggr(i+1, 2);
% end

initial_size_lifetime_count_av = initial_size_lifetime_counts(:, 1)./initial_size_lifetime_counts(:,2);
% initial_size_lifetime_aggr = initial_size_lifetime_aggr(:,1)./initial_size_lifetime_aggr(:,2);
max_size_lifetime_count_av = max_size_lifetime_counts(:, 1)./max_size_lifetime_counts(:,2);
% max_size_lifetime_aggr = max_size_lifetime_aggr(:,1)./max_size_lifetime_aggr(:,2);

scatter3(initial_size_objects(:,1), max_size_objects(:,1), initial_size_objects(:, 2));

% if exist(average_groupsize_str, 'file')
%     figure
%     scatter3(initial_size_objects(:,1), average_size_objects(:,1), initial_size_objects(:, 2));
%     figure
%     scatter3(max_size_objects(:,1), average_size_objects(:,1), initial_size_objects(:, 2));
%     figure
%     scatter3(creation_time(:,2), average_size_objects(:,1), initial_size_objects(:, 2));
%     figure
%     scatter3(starve_time(:,2), average_size_objects(:,1), initial_size_objects(:, 2));
%     figure
% end

%This only works if one has Matlab's statistical toolbox
% boxplot(initial_size_objects(:, 2), initial_size_objects(:,1));
% figure
% boxplot(max_size_objects(:, 2), max_size_objects(:,1));
% figure

figure
plot(initial_size_lifetime_count_av, 'o');
figure
hist(initial_size_objects(:, 2), 50);

figure
plot(max_size_lifetime_count_av, 'o');
