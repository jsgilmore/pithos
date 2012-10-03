close all
clear all

malicious_nodes =      [0.0,      0.125,    0.25,     0.375,    0.5,      0.625,    0.75,     0.875,    1.0];

%Reliability
safe4_get_rel =        [0.9998,   0.9926,   0.9558,   0.8609,   0.7134,   0.5047,   0.3042,   0.1135,   0.0];
safe6_get_rel =        [0.9999,   0.9997,   0.9966,   0.9784,   0.9270,   0.8032,   0.6189,   0.3168,   0.0];
fast_get_rel =         [0.9970,   0.8767,   0.7482,   0.6248,   0.5025,   0.3722,   0.2669,   0.1324,   0.0];
overlay_get_rel =      [0.9140,   0.8557,   0.7056,   0.5156,   0.3437,   0.1921,   0.0797,   0.0166,   0.0];

%Responsiveness
safe4_get_resp =       [0.354,    0.351,    0.356,    0.353,    0.346,    0.350,    0.350,    0.354,    0.356];
safe6_get_resp =       [0.788,    0.789,    0.793,    0.789,    0.785,    0.780,    0.778,    0.789,    0.800];
fast_get_resp =        [0.196,    0.197,    0.197,    0.203,    0.217,    0.213,    0.220,    0.218,    0.165];
overlay_get_resp =     [1.760,    1.805,    1.841,    1.825,    1.785,    1.758,    1.692,    1.680,    1.680];

%Data used
safe4_get_data_in =    [4,        4,        4,        4,        4,        4,        4,        4,        4];
safe6_get_data_in =    [4,        4,        4,        4,        4,        4,        4,        4,        4];
fast_get_data_in =     [4,        4,        4,        4,        4,        4,        4,        4,        4];
overlay_get_data_in =  [4,        4,        4,        4,        4,        4,        4,        4,        4];

safe4_get_data_out =   [157,      121,      116,      98,       87,       61,       37,       14,      0  ];
safe6_get_data_out =   [72,       73,       71,       54,       66,       57,       44,       22,      0  ];
fast_get_data_out =    [157,      157,      156,      156,      154,      154,      154,      154,     144];
overlay_get_data_out = [157,      157,      156,      156,      154,      154,      154,      154,     144];

%Group bandwidth
safe4_get_group_in =   [725,      724,      721,      714,      717,      716,      715,      714,      710];
safe6_get_group_in =   [723,      724,      716,      714,      713,      716,      712,      714,      710];
fast_get_group_in =    [186,      179,      180,      174,      178,      176,      175,      174,      165];
overlay_get_group_in = [0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ];

safe4_get_group_out =  [593,      590,      588,      583,      587,      586,      581,      582,      580];
safe6_get_group_out =  [593,      590,      586,      583,      580,      586,      583,      582,      580];
fast_get_group_out =   [182,      175,      177,      171,      174,      174,      172,      170,      182];
overlay_get_group_out= [0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ,      0  ];

%Overlay bandwidth
safe4_get_overlay_in=  [1187,     1187,     1192,     1187,     1185,     1188,     1188,     1188,     1183];
safe6_get_overlay_in=  [1186,     1187,     1187,     1184,     1186,     1188,     1188,     1188,     1183];
fast_get_overlay_in =  [1180,     1183,     1182,     1185      1183,     1173      1174,     1161,     1160];
overlay_get_overlay_in=[1180,     1183,     1182,     1185,     1183,     1173,     1174,     1161,     1160];

safe4_get_overlay_out= [1201,     1201,     1208,     1203,      1199,     1202,     1203,     1205,    1197];
safe6_get_overlay_out= [1200,     1201,     1202,     1199,      1200,     1202,     1202,     1205,    1197];
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
plot(malicious_nodes, fast_get_rel, 'ko-', 'LineWidth',2);
hold on
plot(malicious_nodes, safe4_get_rel, 'rs-', 'LineWidth',2);
plot(malicious_nodes, safe6_get_rel, 'bd-', 'LineWidth',2);
plot(malicious_nodes, overlay_get_rel, '+-', 'Color',[0.17,0.51,0.34],'MarkerSize',12, 'LineWidth',2);
ylabel('Reliability');
xlabel('Malicious node probability');
hleg = legend('Fast retrieval','Safe retrieval, 4 compares','Safe retrieval, 6 compares', 'Overlay retrieval');
set(hleg, 'Location', 'SouthWest');

figure
set(gca, 'FontSize', 24)
plot(malicious_nodes, fast_get_resp, 'ko-', 'LineWidth',2);
hold on
plot(malicious_nodes, safe4_get_resp, 'rs-', 'LineWidth',2);
plot(malicious_nodes, safe6_get_resp, 'bd-', 'LineWidth',2);
plot(malicious_nodes, overlay_get_resp, '+-', 'Color',[0.17,0.51,0.34],'MarkerSize',12, 'LineWidth',2);
ylabel('Latency (s)');
xlabel('Malicious node probability');
hleg = legend('Fast retrieval','Safe retrieval, 4 compares','Safe retrieval, 6 compares', 'Overlay retrieval');
set(hleg, 'Location', 'SouthWest');

figure
set(gca, 'FontSize', 24)
plot(malicious_nodes, fast_get, 'ko-', 'LineWidth',2);
hold on
plot(malicious_nodes, safe4_get, 'rs-', 'LineWidth',2);
plot(malicious_nodes, safe6_get, 'bd-', 'LineWidth',2);
plot(malicious_nodes, overlay_get, '+-', 'Color',[0.17,0.51,0.34],'MarkerSize',12, 'LineWidth',2);
plot(malicious_nodes, fast_get_data, 'd-', 'Color',[0.48,0.06,0.89], 'LineWidth',2);
ylabel('Bandwidth (Bps)');
xlabel('Malicious node probability');
hleg = legend('Fast retrieval','Safe retrieval, 4 compares','Safe retrieval, 6 compares', 'Overlay retrieval', 'Higher layer');
set(hleg, 'Location', 'SouthWest');