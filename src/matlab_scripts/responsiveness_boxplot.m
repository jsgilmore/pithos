close all
clear all

load ../results/responsiveness_boxplot/resp-1.csv
load ../results/responsiveness_boxplot/resp-2.csv
load ../results/responsiveness_boxplot/resp-3.csv
load ../results/responsiveness_boxplot/resp-4.csv
load ../results/responsiveness_boxplot/resp-5.csv
load ../results/responsiveness_boxplot/resp-6.csv
load ../results/responsiveness_boxplot/resp-7.csv
load ../results/responsiveness_boxplot/resp-8.csv
load ../results/responsiveness_boxplot/resp-9.csv

resp = [resp_1(:,2); resp_2(:,2); resp_3(:,2); resp_4(:,2); resp_5(:,2); resp_6(:,2); resp_7(:,2); resp_8(:,2); resp_9(:,2)];
group = [repmat(0.0, length(resp_1), 1) ; repmat(0.125, length(resp_2), 1) ; repmat(0.25, length(resp_3), 1) ;repmat(0.375, length(resp_4), 1) ;repmat(0.5, length(resp_5), 1) ;repmat(0.625, length(resp_6), 1) ;repmat(0.75, length(resp_7), 1) ;repmat(0.875, length(resp_8), 1) ;repmat(1.0, length(resp_9), 1)];

boxplot(resp, group, 'notch', 'on');

%Adjust x-axis sizes and positions of the stupid boxplot object
text_h = findobj(gca, 'Type', 'text');
for cnt = 1:length(text_h)
        set(text_h(cnt), 'FontSize', 24);
        yshift=get(text_h(cnt), 'Position');
        yshift(2)=-35;
        set(text_h(cnt), 'Position', yshift);
end

% hold on
% plot(0, mean(resp_1(:,2)), 'rx');
% hold on
% plot(0.125, mean(resp_2(:,2)), 'rx');
% plot(0.25, mean(resp_3(:,2)), 'rx');
% plot(0.375, mean(resp_4(:,2)), 'rx');
% plot(0.5, mean(resp_5(:,2)), 'rx');
% plot(0.625, mean(resp_6(:,2)), 'rx');
% plot(0.75, mean(resp_7(:,2)), 'rx');
% plot(0.875, mean(resp_8(:,2)), 'rx');
% plot(1, mean(resp_9(:,2)), 'rx');