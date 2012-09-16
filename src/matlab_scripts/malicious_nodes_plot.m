close all
clear all

% Retrieval method & Malicious nodes&Reliability&Responsiveness&Group bandwidth&Overlay bandwidth  \\
%     fast         & 0.0            &  0.9970   &      (s)     & in/out (Bps)  &  in/out (Bps)     \\
%     fast         & 0.125          &  0.8767   &              &               &                    \\
%     fast         & 0.25           &  0.7482   &    0.198     &  186/182      &    1186/1199      \\
%     fast         & 0.375          &  0.6248   &              &               &                    \\
%     fast         & 0.5            &  0.5025   &    0.192     &  177/173      &    1180/1194      \\
%     fast         & 0.625          &  0.3722   &              &               &                    \\
%     fast         & 0.75           &  0.2669   &    0.191     &  179/175      &    1179/1192      \\
%     fast         & 0.875          &  0.1324   &              &               &                    \\
%     fast         & 1.0            &  0.0061   &              &               &                    \\

%     safe         & 0.0            &  0.9998
%     safe         & 0.125          &  0.9926   &    0.351     &  724/590      &   1187/1201  \\
%     safe         & 0.25           &  0.9558   &    0.356     &  721/588      &              \\
%     safe         & 0.375          &  0.8609   &    0.295     &  714/583      &   979/993    \\
%     safe         & 0.5            &  0.7134   &    0.346     &  717/587      &              \\
%     safe         & 0.625          &  0.5047   &    0.350     &  716/586      &   1188/1202  \\
%     safe         & 0.75           &  0.3042   &    0.350     &  715/581      &              \\
%     safe         & 0.875          &  0.1135   &    0.354     &  714/582      &   1188/1205  \\
%     safe         & 1.0            &  0.0      &    0.356     &  710/580      &   1183/1197  \\

%With super peer maliciousness included (But whether a group peer is malicious is independant from whether an overlay peer is malicous)
% Retrieval method & Malicious nodes&Reliability&Responsiveness&Group bandwidth&Overlay bandwidth  \\
%     fast         & 0.0            &    0.9140
%     fast         & 0.125          &    0.8583
%     fast         & 0.25           &    0.7178
%     fast         & 0.375          &    0.5230
%     fast         & 0.5            &    0.3464
%     fast         & 0.625          &    0.1991
%     fast         & 0.75           &    0.0758
%     fast         & 0.875          &    0.0182
%     fast         & 1.0            &    0.0
%     &                    \\

safe4_get_rel = [0.9998, 0.9926, 0.9558, 0.8609, 0.7134, 0.5047, 0.3042, 0.1135, 0.0];
safe6_get_rel = [0.9999, 0.9997, 0.9966, 0.9784, 0.9270, 0.8032, 0.6189, 0.3168, 0.0];

fast_get_rel = [0.9970, 0.8767, 0.7482, 0.6248, 0.5025 , 0.3722, 0.2669, 0.1324, 0.0061];

overlay_get_rel = [0.9140,0.8583,0.7178,0.5230,0.3464,0.1991,0.0758,0.0182,0.0];

malicious_nodes =       [0.0, 0.125 , 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1.0];

figure
set(gca, 'FontSize', 24)
plot(malicious_nodes, fast_get_rel, 'ko-', 'LineWidth',2);
hold on
plot(malicious_nodes, safe4_get_rel, 'rs-', 'LineWidth',2);
plot(malicious_nodes, safe6_get_rel, 'bd-', 'LineWidth',2);
plot(malicious_nodes, overlay_get_rel, '+-', 'Color',[0.17,0.51,0.34], 'LineWidth',2);

ylabel('Reliability');
xlabel('Malicious node probability');
hleg = legend('Fast retrieval','Safe retrieval, 4 compares','Safe retrieval, 6 compares', 'Overlay retrieval');
set(hleg, 'Location', 'SouthWest');