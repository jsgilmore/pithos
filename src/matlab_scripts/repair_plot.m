clear all
close all

lifetime = [100, 200, 400, 600, 800, 1000, 1400, 1800]

reliability_none = [0.2586, 0.5436, 0.8674, 0.9606, 0.9855, 0.9976];
reliability_leaving = [0.8462, 0.8801, 0.9860, 0.9929, 0.9958, 0.9982];
reliability_periodic = [0.5016, 0.9562, 0.9832, 0.9927, 0.9958, 0.9984];

responsiveness_none = [2.1421, 1.430, 0.6053, 0.3340, 0.2381, 0.1676];
responsiveness_leaving = [0.9162, 0.768, 0.2632, 0.2046, 0.1781, 0.1555];
responsiveness_periodic = [2.0738, 0.470, 0.2767, 0.2092, 0.1823, 0.1533];

bandwidth_group_in_none = [54, 183, 200, 199, 197];
bandwidth_group_out_none = [41, 125, 143, 149, 161];
bandwidth_group_in_leaving = [549, 393, 328, 380, 222];
bandwidth_group_out_leaving = [482, 317, 266, 230, 189];
bandwidth_group_in_periodic = [187, 365, 312, 273, 226];
bandwidth_group_out_periodic = [147, 289, 251, 223, 193];

bandwidth_overlay_in_none = [295, 1073, 1124, 1147, 1177];
bandwidth_overlay_out_none = [304, 1087, 1139, 1162, 1191];
bandwidth_overlay_in_leaving = [307, 1071, 1123, 1144, 1176];
bandwidth_overlay_out_leaving = [322, 1085, 1138, 1158, 1190];
bandwidth_overlay_in_periodic = [378, 1074, 1124, 1144, 1178];
bandwidth_overlay_out_periodic = [397, 1088, 1139, 1160, 1192];

bandwidth_overall_in_none = bandwidth_group_in_none + bandwidth_overlay_in_none;
bandwidth_overall_out_none = bandwidth_group_out_none + bandwidth_overlay_out_none;
bandwidth_overall_in_leaving = bandwidth_group_in_leaving + bandwidth_overlay_in_leaving;
bandwidth_overall_out_leaving = bandwidth_group_out_leaving + bandwidth_overlay_out_leaving;
bandwidth_overall_in_periodic = bandwidth_group_in_periodic + bandwidth_overlay_in_periodic;
bandwidth_overall_out_periodic = bandwidth_group_out_periodic + bandwidth_overlay_out_periodic;

plot(lifetime, reliability_none, 'ko-');
hold on
plot(lifetime, reliability_leaving, 'ko-');
plot(lifetime, reliability_periodic, 'ko-');

figure
plot(lifetime, responsiveness_none, 'ko-');
hold on
plot(lifetime, responsiveness_leaving, 'ko-');
plot(lifetime, responsiveness_periodic, 'ko-');