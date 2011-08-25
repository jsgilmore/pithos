clear all
close all

price = [299950, 299995, 299995, 294995, 299900, 299900];
miles = [56000, 60000, 19800, 35856, 26585, 44000];

score = price.*miles;

plot(score)

