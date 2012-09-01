clear all
close all

lifetime =     [100, 200, 400, 600, 800, 1000, 1400, 1800]
lifetime_par = [100,      400, 600, 800,       1400, 1800];

%Enter results from simulation
reliability_none =     [0.2586, 0.5436, 0.8674, 0.9606, 0.9855, 0.9929, 0.9976, 0.9987];
reliability_par =      [0.9999,         0.9999, 0.9999,         0.9999, 0.9999, 0.9999];
reliability_leaving =  [0.8462, 0.8801, 0.9860, 0.9929, 0.9958, 0.9971, 0.9982, 0.9989];
reliability_periodic = [0.5016, 0.9562, 0.9832, 0.9927, 0.9958, 0.9971, 0.9984, 0.9988];

responsiveness_none =     [2.1421, 1.430, 0.6053, 0.3340, 0.2381, 0.200, 0.1676, 0.157];
responsiveness_leaving =  [0.9162, 0.768, 0.2632, 0.2046, 0.1781, 0.167, 0.1555, 0.147];
responsiveness_periodic = [2.0738, 0.470, 0.2767, 0.2092, 0.1823, 0.167, 0.1533, 0.152];

%Data used
data_in_none =      [5, 7, 7, 6, 6, 5, 5, 4];
data_out_none =     [6, 30, 81, 110, 127, 135, 149, 157];

data_in_leaving =   [5, 7, 7, 6, 6, 5, 5, 4];
data_out_leaving =  [19, 49, 93, 114, 127, 136, 150, 158];

data_in_periodic =  [5, 7, 7, 6, 6, 5, 5, 4];
data_out_periodic = [11, 53, 93, 114, 127, 137, 151, 157];

data_in_par =       [5, 7, 6, 6, 5, 4];
data_out_par =      [0.166, 10, 19, 28, 57, 157];

%Group bandwidth
bandwidth_group_in_none =      [54,  133, 183, 200, 199, 198, 197, 191];
bandwidth_group_out_none =     [41,  92,  125, 143, 149, 153, 161, 162];

bandwidth_group_in_par =       [62,       314, 314, 501,      658, 784];
bandwidth_group_out_par =      [48,       227, 427, 379,      525, 735];

bandwidth_group_in_leaving =   [549, 455, 393, 328, 280, 240, 222, 205];
bandwidth_group_out_leaving =  [482, 356, 317, 266, 230, 200, 189, 179];

bandwidth_group_in_periodic =  [187, 588, 365, 312, 273, 251, 226, 223];
bandwidth_group_out_periodic = [147, 485, 289, 251, 223, 209, 193, 194];

%Overlay bandwidth
bandwidth_overlay_in_none =      [295, 876, 1073, 1124, 1147, 1157, 1177, 1182];
bandwidth_overlay_out_none =     [304, 891, 1087, 1139, 1162, 1170, 1191, 1195];

bandwidth_overlay_in_par =       [274,      1079, 1127, 1150,       1183, 1183];
bandwidth_overlay_out_par =      [288,      1096, 1145, 1167,       1199, 1197];

bandwidth_overlay_in_leaving =   [307, 633, 1071, 1123, 1144, 1161, 1176, 1182];
bandwidth_overlay_out_leaving =  [322, 648, 1085, 1138, 1158, 1176, 1190, 1195];

bandwidth_overlay_in_periodic =  [378, 877, 1074, 1124, 1144, 1162, 1178, 1182];
bandwidth_overlay_out_periodic = [397, 892, 1088, 1139, 1160, 1176, 1192, 1194];

%The overall bandwidth is the sum of group and overlay storage bandwidth
bandwidth_overall_in_none = bandwidth_group_in_none + bandwidth_overlay_in_none;
bandwidth_overall_out_none = bandwidth_group_out_none + bandwidth_overlay_out_none;
bandwidth_overall_in_par = bandwidth_group_in_par + bandwidth_overlay_in_par;
bandwidth_overall_out_par = bandwidth_group_out_par + bandwidth_overlay_out_par;
bandwidth_overall_in_leaving = bandwidth_group_in_leaving + bandwidth_overlay_in_leaving;
bandwidth_overall_out_leaving = bandwidth_group_out_leaving + bandwidth_overlay_out_leaving;
bandwidth_overall_in_periodic = bandwidth_group_in_periodic + bandwidth_overlay_in_periodic;
bandwidth_overall_out_periodic = bandwidth_group_out_periodic + bandwidth_overlay_out_periodic;

%We don't need to include both in and out, we can just average the two.
%Nothing interesting happens seperatly.
bandwidth_overall_none = (bandwidth_overall_in_none + bandwidth_overall_out_none)/2;
bandwidth_overall_par = (bandwidth_overall_in_par + bandwidth_overall_out_par)/2;
bandwidth_overall_leaving = (bandwidth_overall_in_leaving + bandwidth_overall_out_leaving)/2;
bandwidth_overall_periodic = (bandwidth_overall_in_periodic + bandwidth_overall_out_periodic)/2;

data_none = data_in_none + data_out_none;
data_leaving = data_in_leaving + data_out_leaving;
data_periodic = data_in_periodic + data_out_periodic;
data_par = data_in_par + data_out_par;

pure_data_none = data_none./reliability_none;
pure_data_leaving = data_leaving./reliability_leaving;
pure_data_periodic = data_periodic./reliability_periodic;
pure_data_par = data_par./reliability_par;

efficiency_none = data_none./bandwidth_overall_none;
efficiency_leaving = data_leaving./bandwidth_overall_leaving;
efficiency_periodic = data_periodic./bandwidth_overall_periodic;
efficiency_par = data_par./bandwidth_overall_par;

bw_efficiency_none = efficiency_none./reliability_none;
bw_efficiency_leaving = efficiency_leaving./reliability_leaving;
bw_efficiency_periodic = efficiency_periodic./reliability_periodic;
bw_efficiency_par = efficiency_par./reliability_par;

figure
set(gca, 'FontSize', 24)
plot(lifetime, reliability_none, 'ko-', 'LineWidth',2);
hold on
plot(lifetime_par, reliability_par, '+-', 'Color',[0.17,0.51,0.34], 'LineWidth',2);
plot(lifetime, reliability_leaving, 'rs-', 'LineWidth',2);
plot(lifetime, reliability_periodic, 'bd-', 'LineWidth',2);
axis([0 1800 0 1.05])
ylabel('Reliability');
xlabel('Lifetime (s)');
hleg = legend('No repair','No repair, parallel retrieval','Leaving repair', 'Periodic repair, 100s');
set(hleg, 'Location', 'SouthEast');

figure
set(gca, 'FontSize', 24) 
plot(lifetime, responsiveness_none, 'ko-', 'LineWidth',2);
hold on
plot(lifetime, responsiveness_leaving, 'rs-', 'LineWidth',2);
plot(lifetime, responsiveness_periodic, 'bd-', 'LineWidth',2);
ylabel('Responsiveness (s)');
xlabel('Lifetime (s)');
hleg = legend('No repair','Leaving repair', 'Periodic repair, 100s');

figure
set(gca, 'FontSize', 24) 
plot(lifetime, bandwidth_overall_in_none, 'ko-', 'LineWidth',2);
hold on
plot(lifetime, bandwidth_overall_out_none, 'k+-', 'LineWidth',2);
plot(lifetime, bandwidth_overall_in_leaving, 'ro-', 'LineWidth',2);
plot(lifetime, bandwidth_overall_out_leaving, 'r+-', 'LineWidth',2);
plot(lifetime, bandwidth_overall_in_periodic, 'bo-', 'LineWidth',2);
plot(lifetime, bandwidth_overall_out_periodic, 'b+-', 'LineWidth',2);
ylabel('Bandwidth (Bps)');
xlabel('Lifetime (s)');

figure
set(gca, 'FontSize', 24) 
plot(lifetime, bandwidth_overall_none, 'ko-', 'LineWidth',2);
hold on
plot(lifetime_par, bandwidth_overall_par, '+-', 'Color',[0.17,0.51,0.34], 'LineWidth',2);
plot(lifetime, bandwidth_overall_leaving, 'rs-', 'LineWidth',2);
plot(lifetime, bandwidth_overall_periodic, 'bd-', 'LineWidth',2);
ylabel('Bandwidth (Bps)');
xlabel('Lifetime (s)');
hleg = legend('No repair','No repair, parallel retrieval','Leaving repair', 'Periodic repair, 100s');
set(hleg, 'Location', 'SouthEast');

figure
set(gca, 'FontSize', 24) 
plot(lifetime, pure_data_none, 'ko-', 'LineWidth',2);
hold on
plot(lifetime_par, pure_data_par, '+-', 'Color',[0.17,0.51,0.34], 'LineWidth',2);
plot(lifetime, pure_data_leaving, 'rs-', 'LineWidth',2);
plot(lifetime, pure_data_periodic, 'bd-', 'LineWidth',2);
ylabel('Pure data (Bps)');
xlabel('Lifetime (s)');
hleg = legend('No repair','No repair, parallel retrieval','Leaving repair', 'Periodic repair, 100s');
set(hleg, 'Location', 'SouthEast');

figure
set(gca, 'FontSize', 24) 
plot(lifetime, efficiency_none, 'ko-', 'LineWidth',2);
hold on
plot(lifetime_par, efficiency_par, '+-', 'Color',[0.17,0.51,0.34], 'LineWidth',2);
plot(lifetime, efficiency_leaving, 'rs-', 'LineWidth',2);
plot(lifetime, efficiency_periodic, 'bd-', 'LineWidth',2);
ylabel('Efficiency (Data/Bandwidth*Reliability)');
xlabel('Lifetime (s)');
hleg = legend('No repair','No repair, parallel retrieval','Leaving repair', 'Periodic repair, 100s');
set(hleg, 'Location', 'SouthEast');

figure
set(gca, 'FontSize', 24) 
plot(lifetime, bw_efficiency_none, 'ko-', 'LineWidth',2);
hold on
plot(lifetime_par, bw_efficiency_par, '+-', 'Color',[0.17,0.51,0.34], 'LineWidth',2);
plot(lifetime, bw_efficiency_leaving, 'rs-', 'LineWidth',2);
plot(lifetime, bw_efficiency_periodic, 'bd-', 'LineWidth',2);
ylabel('Bandwidth efficiency (Data/Bandwidth)');
xlabel('Lifetime (s)');
hleg = legend('No repair','No repair, parallel retrieval','Leaving repair', 'Periodic repair, 100s');
set(hleg, 'Location', 'SouthEast');
