close all
clear all

load ../Data_traces/GroupSizes_pernode-1.csv
load ../Data_traces/OverlayObjects_zeroes-1.csv
load ../Data_traces/ReplicaObjects-1.csv
load ../Data_traces/RootObjects-1.csv
load ../Data_traces/JoinTime-1.csv

join_time_resolution = 100;

%Construct the vector of overlay objects by group size
OverlayByGroup = [GroupSizes_pernode_1 ; OverlayObjects_zeroes_1];
OverlayByGroup = OverlayByGroup';

%Construct the vector of root + replica objects by group size
RootRepObjects = ReplicaObjects_1 + RootObjects_1;
RootRepByGroup = [GroupSizes_pernode_1 ; RootRepObjects];
RootRepByGroup = RootRepByGroup';

%Construct the vector of overlay objects by join time
OverlayByJoinTime = [JoinTime_1 ; OverlayObjects_zeroes_1];
OverlayByJoinTime = OverlayByJoinTime';

%Construct the vector of root + replica objects by join time
RootRepByJoinTime = [JoinTime_1 ; RootRepObjects];
RootRepByJoinTime = RootRepByJoinTime';

overlay_group_total = zeros(max(OverlayByGroup(:, 1)), 2);
rootrep_group_total = zeros(max(RootRepByGroup(:, 1)), 2);
overlay_time_total = zeros(max(OverlayByJoinTime(:, 1))*join_time_resolution, 2);
rootrep_time_total = zeros(max(RootRepByJoinTime(:, 1)*join_time_resolution), 2);

for i = 1:length(OverlayByGroup)
    if OverlayByGroup(i, 1) ~= 0
        overlay_group_total(OverlayByGroup(i, 1), 1) = overlay_group_total(OverlayByGroup(i, 1), 1) + OverlayByGroup(i, 2);
        overlay_group_total(OverlayByGroup(i, 1), 2) = overlay_group_total(OverlayByGroup(i, 1), 2) + 1;
    end
end
   
for i = 1:length(RootRepByGroup)
    if RootRepByGroup(i, 1) ~= 0
        rootrep_group_total(RootRepByGroup(i, 1), 1) = rootrep_group_total(RootRepByGroup(i, 1), 1) + RootRepByGroup(i, 2);
        rootrep_group_total(RootRepByGroup(i, 1), 2) = rootrep_group_total(RootRepByGroup(i, 1), 2) + 1;
    end
end

for i = 1:length(OverlayByJoinTime)
    if (OverlayByJoinTime(i, 1) ~= 0) && (~isnan(OverlayByJoinTime(i, 1)))
        index = floor(OverlayByJoinTime(i, 1)*join_time_resolution);
        overlay_time_total(index, 1) = overlay_time_total(index, 1) + OverlayByJoinTime(i, 2);
        overlay_time_total(index, 2) = overlay_time_total(index, 2) + 1;
    end
end
   
for i = 1:length(RootRepByJoinTime)
    if (RootRepByJoinTime(i, 1) ~= 0) && (~isnan(OverlayByJoinTime(i, 1)))
        index = floor(RootRepByJoinTime(i, 1)*join_time_resolution);
        rootrep_time_total(index, 1) = rootrep_time_total(index, 1) + RootRepByJoinTime(i, 2);
        rootrep_time_total(index, 2) = rootrep_time_total(index, 2) + 1;
    end
end

%Create a vector with all zeroes removed in a sparse vector format
j = 1;
for i = 1:length(overlay_group_total)
    if overlay_group_total(i) ~= 0
        overlay_group_total_nozeros(j, 1:2) = overlay_group_total(i, 1:2);
        overlay_group_total_nozeros(j, 3) = i;
        j = j + 1;
    end
end

k = 1;
for i = 1:length(rootrep_group_total)
    if rootrep_group_total(i) ~= 0
        rootrep_group_total_nozeros(k, 1:2) = rootrep_group_total(i, 1:2);
        rootrep_group_total_nozeros(k, 3) = i;
        k = k + 1;
    end
end

l = 1;
for i = 1:length(overlay_time_total)
    if overlay_time_total(i) ~= 0
        overlay_time_total_nozeros(l, 1:2) = overlay_time_total(i, 1:2);
        overlay_time_total_nozeros(l, 3) = i;
        l = l + 1;
    end
end

m = 1;
for i = 1:length(rootrep_time_total)
    if rootrep_time_total(i) ~= 0
        rootrep_time_total_nozeros(m, 1:2) = rootrep_time_total(i, 1:2);
        rootrep_time_total_nozeros(m, 3) = i;
        m = m + 1;
    end
end

for i = 1:length(overlay_group_total_nozeros)
   overlay_group_average(i, 1) = overlay_group_total_nozeros(i, 1)/overlay_group_total_nozeros(i, 2); 
   overlay_group_average(i, 2) = overlay_group_total_nozeros(i, 3);
end

for i = 1:length(rootrep_group_total_nozeros)
   rootrep_group_average(i, 1) = rootrep_group_total_nozeros(i, 1)/rootrep_group_total_nozeros(i, 2); 
   rootrep_group_average(i, 2) = rootrep_group_total_nozeros(i, 3);
end

for i = 1:length(overlay_time_total_nozeros)
   overlay_time_average(i, 1) = overlay_time_total_nozeros(i, 1)/overlay_time_total_nozeros(i, 2); 
   overlay_time_average(i, 2) = overlay_time_total_nozeros(i, 3)/join_time_resolution;
end

for i = 1:length(rootrep_time_total_nozeros)
   rootrep_time_average(i, 1) = rootrep_time_total_nozeros(i, 1)/rootrep_time_total_nozeros(i, 2); 
   rootrep_time_average(i, 2) = rootrep_time_total_nozeros(i, 3)/join_time_resolution;
end

%subplot(2,1,1);
subplot('Position',[0.09 0.58 0.88 0.4])
plot(overlay_group_average(:, 2), overlay_group_average(:, 1))
hold on
plot(rootrep_group_average(:, 2), rootrep_group_average(:, 1), 'r')
xlabel('group size', 'fontsize', 20)
ylabel('objects', 'fontsize', 20)
legend('Overlay objects','Root/Replica objects')
set(gca, 'fontsize', 20)


%subplot(2,1,2);
subplot('Position',[0.09 0.08 0.88 0.4])

plot(overlay_time_average(:, 2), overlay_time_average(:, 1))
hold on
plot(rootrep_time_average(:, 2), rootrep_time_average(:, 1), 'r')
xlabel('join time (s)', 'fontsize', 20)
ylabel('objects', 'fontsize', 20)
legend('Overlay objects','Root/Replica objects')
set(gca, 'fontsize', 20)