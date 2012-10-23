%Group consistency measurements
close all
clear all

%First read the super peer's group size vector
super_size = csvread('../results/group_consistency/accurate/groupsize-1.csv');
super_size = [super_size ; [10000 super_size(length(super_size),2)]];

%Load all group vectors from files
%This magic number has to be set to the number of vectors recorded.
for i = 2:251
    eval(['groupsize_' int2str(i) ' = csvread(''../results/group_consistency/accurate/groupsize-' int2str(i) '.csv'');']);
    eval(['groupsize = groupsize_' int2str(i) ';']);
   
    time_min = int32(min(groupsize(:,1)));
    time_max = int32(max(groupsize(:,1)));
    
    %This can be done, since there is a one-one correspondance between the
    %1s intervals measured and the indeces used in the super peer vector.
    super_size_sub = super_size(time_min:time_max,:);
    size_dif = abs(super_size_sub(:,2)-groupsize(:,2));
    
    err_mean(i-1) = mean(size_dif);
    err_std(i-1) = std(size_dif);
end

mean(err_mean)
std(err_mean)
mean(err_std)
std(err_std)