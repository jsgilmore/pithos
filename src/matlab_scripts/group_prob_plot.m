close all
clear all

%Information from simulation
% Pithos & 0(0.0)    &    0.6172  &  2.0646  &
% Pithos & 10(9.98)  &    0.6458  &  1.9102  &
% Pithos & 20(19.74) &    0.6818  &  1.6942  &
% Pithos & 30(29.61) &    0.7186  &  1.5172  &
% Pithos & 40(39.46) &    0.760   &  1.3318  &   401/416  \\
% Pithos & 50(49.29) &    0.7992  &  1.1348  &
% Pithos & 60(59.16) &    0.835   &  0.9480  &   424/438  \\
% Pithos & 70(69.10) &    0.8770  &  0.7636  &
% Pithos & 80(78.93) &    0.915   &  0.5661  &   450/461  \\
% Pithos & 90(88.60) &    0.9531  &  0.3853  &
% Pithos & 100(97.83)     &    0.9998  &  0.196   &
% 
% Group & 0(0.0)    &  N/A    &  N/A      &     53/59
% Group & 10(9.98)  &  0.993  &  0.01567  &     57/63
% Group & 20(19.74) &  0.993  &  0.03037  &     72/77
% Group & 30(29.61) &  0.995  &  0.04618  &     85/89
% Group & 40(39.46) &  0.996  &  0.0623   &     102/104  \\
% Group & 50(49.29) &  0.996  &  0.0759   &     110/112
% Group & 60(59.16) &  0.997  &  0.0917   &     126/127  \\
% Group & 70(69.10) &  0.998  &  0.1066   &     136/137
% Group & 80(78.93) &  0.998  &  0.1209   &     151/150  \\
% Group & 90(88.60) &  0.999  &  0.1356   &     165/164
% Group & 100(97.83)     &  0.999  &  0.1342    &     187/183
% 
% Overlay & 0(0.0)    &  0.619   &  2.0646  &   311/322
% Overlay & 10(9.98)  &  0.609   &  2.1007  &   309/321
% Overlay & 20(19.74) &  0.607   &  2.0690  &   309/320
% Overlay & 30(29.61) &  0.604   &  2.0839  &   308/321
% Overlay & 40(39.46) &  0.608   &   2.0849 &   299/312  \\
% Overlay & 50(49.29) &  0.610   &  2.0711  &   309/321
% Overlay & 60(59.16) &  0.604   &   2.0798 &   298/311  \\
% Overlay & 70(69.10) &  0.612   &   2.0945 &   309/320
% Overlay & 80(78.93) &  0.609   &   2.0642 &   299/311  \\
% Overlay & 90(88.60) &  0.615   &   2.0746 &   310/321
% Overlay & 100(97.83) &  0.615   &   2.0746 &   301/314

% 0(0.0)        &  4  &  98   \\
% 10(9.98)      &  4  &  101  \\
% 20(19.74)     &  4  &  107  \\
% 30(29.61)     &  4  &  113  \\
% 40(39.46)     &  4  &  119  \\
% 50(49.29)     &  4  &  124  \\
% 60(59.16)     &  4  &  131  \\
% 70(69.10)     &  4  &  138  \\
% 80(78.93)     &  4  &  145  \\
% 90(88.60)     &  4  &  151  \\
% 100(97.83)    &  4  &  157  \\

group_prob = [0.0,9.98,19.74,29.61,39.46,49.29,59.16,69.10,78.93,88.60,97.83];

group_rel = [0.993, 0.993, 0.995, 0.996, 0.996, 0.997, 0.998, 0.998, 0.999, 0.999];
group_resp = [0.01567, 0.03037, 0.04618, 0.0623, 0.0759, 0.0917, 0.1066, 0.1209, 0.1356, 0.1342];
group_resp_succ = [0.1362,0.1330,0.1326,0.1325, 0.1337, 0.1344, 0.1338,0.1340,0.1330,0.1337];
group_in = [53,57,72,85,102,110,126,136,151,165,187];
group_out = [59,63,77,89,104,112,127,137,150,164,183];
group_av = (group_in+group_out)/2;

overlay_rel = [0.619,0.609,0.607,0.604,0.608,0.610,0.604,0.612,0.609,0.615,0.615];
overlay_resp = [2.0646, 2.1007, 2.0690, 2.0839, 2.0849, 2.0711, 2.0798, 2.0945, 2.0642, 2.0746, 2.0746];
overlay_in = [311,309,309,308,299,309,298,309,299,310,301];
overlay_out = [322,321,320,321,312,321,311,320,311,321,314];
overlay_av = (overlay_in+overlay_out)/2;

pithos_rel = [0.6172, 0.6458, 0.6818, 0.7186, 0.760, 0.7992, 0.835, 0.8770, 0.915, 0.9531, 0.9998];
pithos_resp = [2.0646, 1.9102, 1.6942, 1.5172, 1.3318, 1.1348, 0.9480, 0.7636, 0.5661, 0.3853, 0.196];
pithos_in = group_in + overlay_in;
pithos_out = group_out + overlay_out;
pithos_av = (pithos_in+pithos_out)/2;

data_in = [4,4,4,4,4,4,4,4,4,4,4];
data_out = [98,101,107,113,119,124,131,138,145,151,157];

figure
set(gca, 'FontSize', 24)
plot(group_prob(2:length(group_prob)), group_rel, 'bo-', 'LineWidth',2);
hold on
plot(group_prob, pithos_rel, 'ks-', 'LineWidth',2);
plot(group_prob, overlay_rel, 'rd-', 'LineWidth',2);
ylabel('Reliability');
xlabel('Group probability (%)');
hleg = legend('Group storage', 'Pithos overall', 'Overlay storage');

figure 
set(gca, 'FontSize', 24)
plot(group_prob(2:length(group_prob)), group_resp, 'bo-', 'LineWidth',2);
hold on
plot(group_prob(2:length(group_prob)), group_resp_succ, 'x-', 'Color',[0.48,0.06,0.89], 'MarkerSize',12, 'LineWidth',2);
plot(group_prob, pithos_resp, 'ks-', 'LineWidth',2);
plot(group_prob, overlay_resp, 'rd-', 'LineWidth',2);
ylabel('Latency (s)');
xlabel('Group probability (%)');
hleg = legend('Group', 'Group success', 'Pithos', 'Overlay');

figure
set(gca, 'FontSize', 24)
plot(group_prob, group_av, 'bo-', 'LineWidth',2);
hold on
plot(group_prob, pithos_av, 'ks-', 'LineWidth',2);
plot(group_prob, overlay_av, 'rd-', 'LineWidth',2);
plot(group_prob, data_in+data_out, '+-', 'Color',[0.87,0.49,0], 'LineWidth',2, 'MarkerSize',12);
ylabel('Bandwidth (Bps)');
xlabel('Group probability (%)');
hleg = legend('Group', 'Pithos', 'Overlay', 'Higher layer');

