%clear all
%close all

SP_number = 1500;
Files_number = 313848;

SPs = rand([SP_number,1]);
Files = rand([Files_number,1]);

for j = 1:1:Files_number
    nearest_dist(j) = 100;
    place(j) = -1;

    for i = 1:1:SP_number

        distance(i) = abs(SPs(i)-Files(j));

        if (distance(i) < nearest_dist(j))
            nearest_dist(j) = distance(i);
            place(j) = i;
        end
    end
end

figure
hist(place, [1:SP_number]);
group_sizes = hist(place, [1:SP_number]);

figure
hist(group_sizes, 50);
group_size_dist = hist(group_sizes, 50);
