close all
clear all

malicious_nodes =      [0.0,      0.125,    0.25,     0.375,    0.5,      0.625,    0.75,     0.875];

%Reliability
safe4_get_rel =        [0.9994,   0.9919,   0.9487,   0.8517,   0.6776,   0.4735,   0.2662,   0.0775,   0.0];
safe6_get_rel =        [0.9985,   0.9983,   0.9958,   0.9733,   0.9044,   0.7631,   0.4734,   0.1685,   0.0];
fast_get_rel =         [0.9970,   0.8767,   0.7482,   0.6248,   0.5025,   0.3722,   0.2669,   0.1324,   0.0];
overlay_get_rel =      [0.9140,   0.8557,   0.7056,   0.5156,   0.3437,   0.1921,   0.0797,   0.0166,   0.0];

%Responsiveness
safe4_get_resp =       [0.214,    0.215,    0.207,    0.206,    0.206,    0.201,    0.204,    0.199,    0.0];
safe6_get_resp =       [0.608,    0.613,    0.590,    0.539,    0.491,    0.431,    0.368,    0.329,    0.0];
fast_get_resp =        [0.196,    0.197,    0.197,    0.203,    0.217,    0.213,    0.220,    0.218,    0.165];
overlay_get_resp =     [1.760,    1.805,    1.841,    1.825,    1.785,    1.758,    1.692,    1.680,    1.680];

%Data used
safe4_get_data_in =    [4,        4,        4,        4,        4,        4,        4,        4,        4];
safe6_get_data_in =    [4,        4,        4,        4,        4,        4,        4,        4,        4];
fast_get_data_in =     [4,        4,        4,        4,        4,        4,        4,        4,        4];
overlay_get_data_in =  [4,        4,        4,        4,        4,        4,        4,        4,        4];

safe4_get_data_out =   [122,      120,      117,      105,      84,       58,       32,       9.4      0.0];
safe6_get_data_out =   [108,      108,      103,      95,       83,       65,       38,       13,      0.0];
fast_get_data_out =    [157,      157,      156,      156,      154,      154,      154,      154,     144];
overlay_get_data_out = [157,      157,      156,      156,      154,      154,      154,      154,     144];

%Group bandwidth
safe4_get_group_in =   [778,      777,      789,      793,      794,      791,      794,      785,      44];
safe6_get_group_in =   [784,      802,      794,      790,      792,      787,      802,      798,      43];
fast_get_group_in =    [186,      179,      180,      174,      178,      176,      175,      174,      165];
overlay_get_group_in = [0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ];

safe4_get_group_out =  [696,      693,      703,      707,      706,      707,      707,      699,      49];
safe6_get_group_out =  [702,      717,      712,      708,      705,      706,      718,      716,      49];
fast_get_group_out =   [182,      175,      177,      171,      174,      174,      172,      170,      182];
overlay_get_group_out= [0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ];

%Overlay bandwidth
safe4_get_overlay_in=  [1184,     1184,     1181,     1181,     1182,     1181,     1188,     1178,     0];
safe6_get_overlay_in=  [1178,     1180,     1180,     1181,     1186,     1184,     1185,     1185,     0];
fast_get_overlay_in =  [1180,     1183,     1182,     1185      1183,     1173      1174,     1161,     1160];
overlay_get_overlay_in=[1180,     1183,     1182,     1185,     1183,     1173,     1174,     1161,     1160];

safe4_get_overlay_out= [1200,     1199,     1196,     1196,     1198,     1195,     1204,     1193,     0];
safe6_get_overlay_out= [1192,     1194,     1194,     1196,     1202,     1200,     1201,     1201,     0];
fast_get_overlay_out = [1192,     1196,     1193,     1197,     1197,     1186,     1187,     1176,     1176];
overlay_get_overlay_out=[1182,    1196,     1193,     1197,     1197,     1186,     1187,     1176,     1176];

safe4_get_in = safe4_get_group_in + safe4_get_overlay_in;
safe6_get_in = safe6_get_group_in + safe6_get_overlay_in;
fast_get_in = fast_get_group_in + fast_get_overlay_in;
overlay_get_in = overlay_get_group_in + overlay_get_overlay_in;

safe4_get_out = safe4_get_group_out + safe4_get_overlay_out;
safe6_get_out = safe6_get_group_out + safe6_get_overlay_out;
fast_get_out = fast_get_group_out + fast_get_overlay_out;
overlay_get_out = overlay_get_group_out + overlay_get_overlay_out;

safe4_get = (safe4_get_in + safe4_get_out)/2;
safe6_get = (safe6_get_in + safe6_get_out)/2;
fast_get =  (fast_get_in + fast_get_out)/2;
overlay_get =  (overlay_get_in + overlay_get_out)/2;

safe4_get_data = safe4_get_data_in + safe4_get_data_out;
safe6_get_data = safe6_get_data_in + safe6_get_data_out;
fast_get_data = fast_get_data_in + fast_get_data_out;
overlay_get_data = overlay_get_data_in + overlay_get_data_out;

figure
set(gca, 'FontSize', 24)
plot(malicious_nodes, fast_get_rel(1:8), 'ko-', 'LineWidth',2);
hold on
plot(malicious_nodes, safe4_get_rel(1:8), 'rs-', 'LineWidth',2);
plot(malicious_nodes, safe6_get_rel(1:8), 'bd-', 'LineWidth',2);
plot(malicious_nodes, overlay_get_rel(1:8), '+-', 'Color',[0.17,0.51,0.34],'MarkerSize',12, 'LineWidth',2);
ylabel('Reliability');
xlabel('Malicious node probability');
hleg = legend('Fast retrieval','Safe retrieval, 4 compares','Safe retrieval, 6 compares', 'Overlay retrieval');
set(hleg, 'Location', 'SouthWest');

figure
set(gca, 'FontSize', 24)
plot(malicious_nodes, fast_get_resp(1:8), 'ko-', 'LineWidth',2);
hold on
plot(malicious_nodes, safe4_get_resp(1:8), 'rs-', 'LineWidth',2);
plot(malicious_nodes, safe6_get_resp(1:8), 'bd-', 'LineWidth',2);
plot(malicious_nodes, overlay_get_resp(1:8), '+-', 'Color',[0.17,0.51,0.34],'MarkerSize',12, 'LineWidth',2);
ylabel('Latency (s)');
xlabel('Malicious node probability');
hleg = legend('Fast retrieval','Safe retrieval, 4 compares','Safe retrieval, 6 compares', 'Overlay retrieval');
set(hleg, 'Location', 'SouthWest');

figure
set(gca, 'FontSize', 24)
plot(malicious_nodes, fast_get(1:8), 'ko-', 'LineWidth',2);
hold on
plot(malicious_nodes, safe4_get(1:8), 'rs-', 'LineWidth',2);
plot(malicious_nodes, safe6_get(1:8), 'bd-', 'LineWidth',2);
plot(malicious_nodes, overlay_get(1:8), '+-', 'Color',[0.17,0.51,0.34],'MarkerSize',12, 'LineWidth',2);
plot(malicious_nodes, fast_get_data(1:8), 'd-', 'Color',[0.48,0.06,0.89], 'LineWidth',2);
ylabel('Bandwidth (Bps)');
xlabel('Malicious node probability');
hleg = legend('Fast retrieval','Safe retrieval, 4 compares','Safe retrieval, 6 compares', 'Overlay retrieval', 'Higher layer');
set(hleg, 'Location', 'SouthWest');