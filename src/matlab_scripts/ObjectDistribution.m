close all

subplot('Position',[0.09 0.58 0.88 0.4])

hist(RootRepObjects, bin)

axis([0 1000 0 450]);
xlabel('number of objects', 'fontsize', 20)
ylabel('number of nodes', 'fontsize', 20)
set(gca, 'fontsize', 20)

subplot('Position',[0.09 0.08 0.88 0.4])

hist(OverlayObjects_nozeroes, bin)

axis([0 1000 0 500]);
xlabel('number of objects', 'fontsize', 20)
ylabel('number of nodes', 'fontsize', 20)
set(gca, 'fontsize', 20)

figure
hist(Objects, bin)

axis([0 1000 0 300]);
xlabel('number of objects', 'fontsize', 20)
ylabel('number of nodes', 'fontsize', 20)
set(gca, 'fontsize', 20)