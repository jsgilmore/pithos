close all
clear all

load ../Data_traces/StoreTimes.mat
load ../Data_traces/PastryLarge_1024.csv
load ../Data_traces/Overlay_store_time_2_nozeroes-2.csv
clear Store_time_rootrep_mean-1
load ../Data_traces/Store_time_rootrep_mean-1.csv

%subplot(2,1,1);
subplot('Position',[0.1 0.58 0.88 0.4])
hist(Store_time_rootrep_mean_1, bin);

axis([0.01 0.7 0 2500]);
h = findobj(gca,'Type','patch');
set(h,'FaceColor','r','EdgeColor','r')
hold on
bin2 = [0:0.012:1.38];
hist(Overlay_store_time_2_nozeroes_2, bin2);

xlabel('time (s)', 'fontsize', 20)
ylabel('objects', 'fontsize', 20)
legend('Group objects', 'Overlay objects')
set(gca, 'fontsize', 20)


%subplot(2,1,2);
subplot('Position',[0.1 0.08 0.88 0.4])

hist(PastryLarge_1024(:, 2), bin)

axis([0 0.7 0 25000]);
xlabel('time (s)', 'fontsize', 20)
ylabel('objects', 'fontsize', 20)
set(gca, 'fontsize', 20)