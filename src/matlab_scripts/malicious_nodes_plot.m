close all
clear all

% Retrieval method & Malicious nodes&Reliability&Responsiveness&Group bandwidth&Overlay bandwidth  \\
%                  &                &           &      (s)     & in/out (Bps)  &  in/out (Bps)     \\
%     fast         & 0.125          &           &              &               &                    \\
%     fast         & 0.25           &  0.7482   &    0.198     &  186/182      &    1186/1199      \\
%     fast         & 0.375          &           &              &               &                    \\
%     fast         & 0.5            &  0.5025   &    0.192     &  177/173      &    1180/1194      \\
%     fast         & 0.125          &           &              &               &                    \\
%     fast         & 0.75           &  0.2669   &    0.191     &  179/175      &    1179/1192      \\
%     fast         & 0.125          &           &              &               &                    \\

%     safe         & 0.125          &  0.9926   &    0.351     &  724/590      &   1187/1201  \\
%     safe         & 0.25           &  0.9558   &    0.356     &  721/588      &              \\
%     safe         & 0.375          &  0.8609   &    0.295     &  714/583      &   979/993    \\
%     safe         & 0.5            &  0.7134   &    0.346     &  717/587      &              \\
%     safe         & 0.625          &  0.5047   &    0.350     &  716/586      &   1188/1202  \\
%     safe         & 0.75           &  0.3042   &    0.350     &  715/581      &              \\
%     safe         & 0.875          &  0.1135   &    0.354     &  714/582      &   1188/1205  \\
%     safe         & 1.0            &  0.0      &    0.356     &  710/580      &   1183/1197  \\

safe_get_rel = [0.9998, 0.9926 , 0.9558 , 0.8609 , 0.7134 , 0.5047 , 0.3042 , 0.1135 , 0.0];

fast_get_rel = [0.9970, 0.7482, 0.5025 , 0.2669 , 0.0];

malicious_nodes_fast = [0.0, 0.25, 0.5, 0.75, 1.0];
malicious_nodes_safe = [0.0, 0.125 , 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1.0];

plot(malicious_nodes_safe, safe_get_rel, 'ko-');
hold on
plot(malicious_nodes_fast, fast_get_rel, 'ko-');