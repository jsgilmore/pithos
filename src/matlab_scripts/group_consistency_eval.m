%Group consistency measurements

%Load all group vectors from files
for i = 1:258
    eval(['groupsize_' int2str(i) ' = csvread(''../results/group_consistency/group_size-' int2str(i) '.csv'');']);
    
    eval(['gs_' int2str(i) ' = timeseries(groupsize_' int2str(i) '(:,2), groupsize_' int2str(i) '(:,1)'');']);
end