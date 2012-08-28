close all
clear all

folder = '../results/LAN_responsiveness/';

eval(['load ' folder 'group_get.csv']);
eval(['load ' folder 'group_put.csv']);
eval(['load ' folder 'overlay_get.csv']);
eval(['load ' folder 'overlay_put.csv']);
eval(['load ' folder 'overall_get.csv']);
eval(['load ' folder 'overall_put.csv']);

figure('Name','group get')
set(gca, 'FontSize', 24) 
hist(group_get(:,2), 50000);
axis([-0.05 1 0 1.15e+006])
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','group put')
set(gca, 'FontSize', 24) 
hist(group_put(:,2), 1000);
axis([-0.05 10.05 0 17000])
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','overlay get')
set(gca, 'FontSize', 24) 
hist(overlay_get(:,2), 10000);
axis([0 10 0 70000])
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','overlay put')
set(gca, 'FontSize', 24) 
hist(overlay_put(:,2), 100);
axis([0 10 0 6000])
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','overall get')
set(gca, 'FontSize', 24) 
hist(overall_get(:,2), 10000);
axis([-0.05 1 0 1.15e+006])
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','overall put')
set(gca, 'FontSize', 24) 
hist(overall_put(:,2), 100);
axis([0 10.05 0 6000])
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');