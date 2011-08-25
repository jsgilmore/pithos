clear all
close all

SP_number = 10;
Peer_number = 20000;

SPs = rand([SP_number,2]);
peers = rand([Peer_number,2]);

scatter(peers(:,1), peers(:,2), 'b+');
hold on
scatter(SPs(:,1), SPs(:,2), 'k');

for j = 1:1:Peer_number
    nearest_dist(j) = 100;
    place(j) = -1;
    
    Peer_lat = peers(j,1);
    Peer_long = peers(j,2);

    for i = 1:1:SP_number

        SP_lat = SPs(i,1);
        SP_long = SPs(i,2);

        distance(i) = sqrt((SP_lat - Peer_lat)^2 + (SP_long - Peer_long)^2);

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
