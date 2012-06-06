close all
clear all

%Which group's data to load
%Set to zero for only a single group with no numbered postfix
number = 0;
%The folder to load the data from
folder = '20_10_size15_100_exp_1';
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


%--------------------------------------------------------------------------
%Create the required data structured beforehand to speed up runtime
%--------------------------------------------------------------------------
initial_size_objects = [initial_groupsize(:,2) , lifetime(:, 2)];
initial_size_lifetime_counts = zeros(max(initial_groupsize(:,2)), 2);

max_size_objects = [max_groupsize(:, 2), lifetime(:, 2)];
max_size_lifetime_counts = zeros(max(max_groupsize(:, 2)), 2);

size_lifetime_counts = zeros(max(initial_groupsize(:,2)), max(max_groupsize(:, 2)), 2);

if exist(average_groupsize_str, 'file')
    average_size_objects = [average_groupsize(:, 2), lifetime(:, 2)];
    average_size_lifetime_counts = zeros(max(round(average_groupsize(:, 2))), 2);
end

x = 1;
y = 1;

boxplot_max_group = round(mean(average_size_objects(:, 1)));
%boxplot_max_group = 10;
boxplot_init_group = round(mean(initial_size_objects(:, 1)));
%boxplot_init_group = 10;

%--------------------------------------------------------------------------
%Total the object lifetimes for every maximum group size and every initial
%group size and count how many measurements were made for each.
%--------------------------------------------------------------------------
for i=1:length(initial_size_objects(:,1))
    if round(average_size_objects(i, 1)) == boxplot_max_group
        initial_size_objects_forMeanAv(x, 1) = initial_size_objects(i,1);
        initial_size_objects_forMeanAv(x, 2) = initial_size_objects(i,2);
        x = x+1;
    end

    if initial_size_objects(i, 1) == boxplot_init_group
        average_size_objects_forMeanInit(y, 1) = max_size_objects(i,1);
        average_size_objects_forMeanInit(y, 2) = max_size_objects(i,2);
        y = y+1;
    end

    initial_size_lifetime_counts(initial_size_objects(i, 1), 1) = initial_size_lifetime_counts(initial_size_objects(i, 1), 1) + initial_size_objects(i, 2);
    initial_size_lifetime_counts(initial_size_objects(i, 1), 2) = initial_size_lifetime_counts(initial_size_objects(i, 1), 2) + 1;

    average_size_lifetime_counts(round(average_size_objects(i, 1)), 1) = average_size_lifetime_counts(round(average_size_objects(i, 1)), 1) + average_size_objects(i, 2);
    average_size_lifetime_counts(round(average_size_objects(i, 1)), 2) = average_size_lifetime_counts(round(average_size_objects(i, 1)), 2) + 1;

    size_lifetime_counts(initial_size_objects(i, 1), round(average_size_objects(i, 1)), 1) = size_lifetime_counts(initial_size_objects(i, 1), round(average_size_objects(i, 1)), 1) + average_size_objects(i, 2);
    size_lifetime_counts(initial_size_objects(i, 1), round(average_size_objects(i, 1)), 2) = size_lifetime_counts(initial_size_objects(i, 1), round(average_size_objects(i, 1)), 2) + 1;
end

%--------------------------------------------------------------------------
%Use the totals and counts computed in the previous step to compute average
%lifetime values.
%--------------------------------------------------------------------------
initial_size_lifetime_count_av = initial_size_lifetime_counts(:, 1)./initial_size_lifetime_counts(:,2);
average_size_lifetime_count_av = average_size_lifetime_counts(:, 1)./average_size_lifetime_counts(:,2);
size_lifetime_count_av = size_lifetime_counts(:, :, 1)./size_lifetime_counts(:, :,2);

%--------------------------------------------------------------------------
%Draw some cool looking graphs
%--------------------------------------------------------------------------

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
% 
% scatter3(initial_size_objects(:,1), max_size_objects(:,1), initial_size_objects(:, 2));
% xlabel('Initial group size');
% ylabel('Maximum group size');
% zlabel('Object lifetime (s)');
% title('Scatter plot of measured object lifetimes vs. initial group size and maximum group size.');

%This only works if one has Matlab's statistical toolbox
figure
boxplot(initial_size_objects(:, 2), initial_size_objects(:,1), 'notch', 'on');
ylabel('Object lifetime (s)');
xlabel('Initial group size');
title('Box plot of measured object lifetimes vs. initial group size for any maximum group size.');

figure
boxplot(average_size_objects(:, 2), round(average_size_objects(:,1)), 'notch', 'on');
ylabel('Object lifetime (s)');
xlabel('Average group size');
title('Box plot of measured object lifetimes vs. maximum group size for any initial group size.');

figure
plot(initial_size_lifetime_count_av, 'o');
ylabel('Object lifetime (s)');
xlabel('Initial group size');
title('Average object lifetimes vs. initial group size for any maximum group size.');

figure
hist(initial_size_objects(:, 2), 500);
xlabel('Object lifetime (s)');

figure
plot(average_size_lifetime_count_av, 'o');
ylabel('Object lifetime (s)');
xlabel('Average group size');
title('Average object lifetimes vs. maximum group size for any initial group size.');

figure
surf(size_lifetime_counts(:,:,2));
xlabel('Average group size');
ylabel('Initial group size');
zlabel('Number of lifetime measurements');
title('Surface plot of the number of lifetime measurements vs. maximum group size and initial group size.');

figure
surf(size_lifetime_count_av);
xlabel('Maximum group size');
ylabel('Initial group size');
zlabel('Object lifetime (s)');
title('Surface plot of average object lifetimes vs. maximum group size and initial group size.');

figure
boxplot(initial_size_objects_forMeanAv(:, 2), initial_size_objects_forMeanAv(:, 1), 'notch', 'on');
ylabel('Object lifetime (s)');
xlabel('Initial group size');
title('Box plot of measured object lifetimes vs. initial group size for a specific maximum group size.');

figure
boxplot(average_size_objects_forMeanInit(:, 2), average_size_objects_forMeanInit(:, 1), 'notch', 'on');
ylabel('Object lifetime (s)');
xlabel('Maximum group size');
title('Box plot of measured object lifetimes vs. maximum group size for a specific initial group size.');