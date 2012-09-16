close all
clear all

folder = '../results/responsiveness/';

timeout = 10;

responsiveness = csvread([folder 'overall_get_sp.csv']);

responsiveness_trim = zeros(length(responsiveness), 1);

x = 1;
for i = 1:length(responsiveness)
    if responsiveness(i, 2) ~= timeout
        responsiveness_trim(x) = responsiveness(i, 2);
        x = x+1;
    end
end

original_mean = mean(responsiveness(:, 2))
trimmed_mean = mean(responsiveness_trim(1:x-1))