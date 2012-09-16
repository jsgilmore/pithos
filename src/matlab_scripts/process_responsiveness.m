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
hist(group_get(:,2), 5000);
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','group put')
set(gca, 'FontSize', 24) 
hist(group_put(:,2), 500);
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','overlay get')
set(gca, 'FontSize', 24) 
hist(overlay_get(:,2), 500);
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','overlay put')
set(gca, 'FontSize', 24) 
hist(overlay_put(:,2), 100);
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','overall get')
set(gca, 'FontSize', 24) 
hist(overall_get(:,2), 1000);
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');

figure('Name','overall put')
set(gca, 'FontSize', 24) 
hist(overall_put(:,2), 500);
h = findobj(gca,'Type','patch');
set(h,'FaceColor','k','EdgeColor','k')
ylabel('Number of messages');
xlabel('time (s)');