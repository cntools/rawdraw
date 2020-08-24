#ifndef FONTINIT
#define FONTINIT
int CharIndex[] = {
	   0,    1,   13,   25,   40,   49,   76,   93,  113,    0,    0,  143,  158,    0,  173,  194,
	 212,  217,  222,  232,  239,  252,  269,  314,  325,  331,  337,  343,  349,  353,  362,  427,
	   0,  488,  492,  501,  510,  525,  538,  550,  555,  560,  565,  572,  577,  583,  586,  592,
	 595,  606,  612,  621,  633,  639,  651,  664,  670,  682,  694,  705,  717,  721,  726,  730,
	 739,  755,  761,  773,  782,  791,  798,  804,  816,  823,  830,  838,  846,  850,  856,  861,
	 871,  879,  891,  900,  913,  918,  925,  929,  938,  943,  949,  956,  961,  964,  969,  973,
	 976,  981,  993, 1004, 1011, 1022, 1032, 1040, 1053, 1061, 1066, 1072, 1080, 1085, 1098, 1106,
	1116, 1126, 1139, 1146, 1153, 1160, 1168, 1173, 1182, 1187, 1195, 1202, 1212, 1215, 1225, 1232,
	1239, 1250, 1260, 1274, 1289, 1303, 1319, 1336, 1345, 1358, 1370, 1384, 1390, 1397, 1405, 1416,
	1429, 1438, 1456, 1468, 1482, 1494, 1508, 1521, 1534, 1542, 1554, 1563, 1574, 1586, 1596, 1615,
	1625, 1641, 1649, 1663, 1676, 1688, 1699, 1708, 1716, 1725, 1729, 1733, 1744, 1755, 1759, 1766,
	1773, 1790, 1823, 1864, 1867, 1873, 1880, 1888, 1894, 1903, 1912, 1917, 1924, 1931, 1937, 1943,
	1947, 1951, 1956, 1961, 1966, 1969, 1974, 1981, 1988, 1995, 2002, 2011, 2020, 2029, 2034, 2047,
	2052, 2059, 2066, 2073, 2079, 2087, 2093, 2099, 2108, 2117, 2121, 2125, 2189, 2235, 2280, 2329,
	2378, 2390, 2404, 2408, 2419, 2425, 2435, 2445, 2450, 2461, 2473, 2484, 2497, 2511, 2523, 2533,
	2540, 2547, 2554, 2560, 2566, 2572, 2575, 2588, 2601,    0, 2607, 2613, 2618, 2625, 2631, 2676,
	};

unsigned char FontData[] = {
	0x80, 0xe9, 0x0c, 0x0d, 0x16, 0x2e, 0x35, 0x34, 0x4c, 0x11, 0x53, 0x29, 0xeb, 0xe9, 0x11, 0x53,
	0x29, 0x6b, 0x14, 0x2c, 0x35, 0x36, 0x0e, 0x0d, 0xd4, 0xe9, 0x27, 0x0d, 0x03, 0x01, 0x08, 0x10,
	0x62, 0x1a, 0x28, 0x30, 0x39, 0x3b, 0x35, 0xdf, 0xe9, 0x27, 0x4b, 0x0c, 0x60, 0x18, 0x74, 0x33,
	0xdf, 0xe9, 0x1b, 0x12, 0x11, 0x18, 0x20, 0x29, 0x2a, 0x63, 0x2a, 0x32, 0x3b, 0x3c, 0x35, 0x2d,
	0x64, 0x1c, 0x15, 0x0d, 0x04, 0x03, 0x0a, 0x52, 0x1c, 0x1f, 0x27, 0xe4, 0xe9, 0x1f, 0x1d, 0x16,
	0x0e, 0x05, 0x04, 0x0b, 0x60, 0x18, 0x3c, 0x3d, 0x36, 0x2e, 0x25, 0x27, 0xdf, 0xe9, 0x00, 0x08,
	0x11, 0x12, 0x0b, 0x14, 0x15, 0x0e, 0x06, 0x40, 0x28, 0x18, 0x1e, 0x6e, 0x23, 0x5b, 0x30, 0x36,
	0xfe, 0xe9, 0x00, 0x02, 0x0a, 0x09, 0x41, 0x00, 0x08, 0x49, 0x12, 0x10, 0x18, 0x5a, 0x11, 0x59,
	0x28, 0x20, 0x22, 0x6a, 0x30, 0x72, 0x31, 0x78, 0x31, 0x7a, 0x13, 0x05, 0x57, 0x05, 0xfd, 0xe9,
	0x20, 0x38, 0x7b, 0x1f, 0x17, 0x05, 0x04, 0x12, 0x1a, 0x2c, 0x2d, 0x5f, 0x23, 0xf8, 0xe9, 0x20,
	0x10, 0x09, 0x0b, 0x14, 0x24, 0x2b, 0x29, 0x60, 0x1c, 0x1f, 0x4d, 0x1f, 0xed, 0xe9, 0x38, 0x59,
	0x39, 0x5a, 0x19, 0x1e, 0x15, 0x0d, 0x06, 0x0f, 0x17, 0x5e, 0x3e, 0x35, 0x2d, 0x26, 0x2f, 0x37,
	0x3e, 0xf8, 0xe9, 0x22, 0x1a, 0x13, 0x14, 0x1d, 0x25, 0x2c, 0x2b, 0x62, 0x38, 0x6a, 0x3f, 0x6d,
	0x07, 0x55, 0x12, 0xc0, 0xe9, 0x07, 0x3c, 0x01, 0xc7, 0xe9, 0x39, 0x3f, 0x04, 0xf9, 0xc9, 0x18,
	0x42, 0x18, 0x72, 0x18, 0x5f, 0x05, 0x1f, 0xf5, 0xe9, 0x10, 0x55, 0x57, 0x28, 0x6d, 0xef, 0xe9,
	0x30, 0x18, 0x11, 0x12, 0x1b, 0x23, 0x60, 0x28, 0x2e, 0x27, 0x1f, 0xd6, 0xe9, 0x29, 0x20, 0x18,
	0x11, 0x12, 0x23, 0x2c, 0x6d, 0x16, 0x1f, 0x27, 0x2e, 0x2d, 0x1c, 0x13, 0xd2, 0xe9, 0x04, 0x07,
	0x3f, 0x3c, 0x04, 0x0f, 0x0c, 0x17, 0x14, 0x1f, 0x1c, 0x27, 0x24, 0x2f, 0x2c, 0x37, 0x34, 0x7f,
	0x3c, 0x77, 0x34, 0x6f, 0x2c, 0x67, 0x24, 0x5f, 0x1c, 0x57, 0x14, 0x4f, 0x0c, 0x47, 0x04, 0x3d,
	0x05, 0x3e, 0x06, 0x7f, 0x3c, 0x45, 0x06, 0x7d, 0x3e, 0xc7, 0xc9, 0x0f, 0x6f, 0x0d, 0x1e, 0x6d,
	0x1e, 0x18, 0x49, 0x18, 0xe9, 0xc9, 0x1f, 0x18, 0x49, 0x18, 0xe9, 0xc9, 0x18, 0x1f, 0x4e, 0x2e,
	0xdf, 0xe9, 0x04, 0x3c, 0x72, 0x3c, 0xf6, 0xe9, 0x3c, 0x04, 0x4a, 0x04, 0xce, 0xe9, 0x03, 0x07,
	0xff, 0xe9, 0x0a, 0x04, 0x4e, 0x04, 0x3c, 0x72, 0x3c, 0xf6, 0xc9, 0x1b, 0x07, 0x37, 0x1b, 0x4f,
	0x17, 0x1b, 0x5f, 0x1b, 0x67, 0x1b, 0x6f, 0x2e, 0x4e, 0x25, 0x55, 0x1c, 0x4e, 0x1c, 0x6e, 0x07,
	0x4e, 0x16, 0x47, 0x26, 0x77, 0x2e, 0x77, 0x1e, 0x47, 0x1e, 0x77, 0x0e, 0x0f, 0x16, 0x17, 0x1e,
	0x1f, 0x26, 0x27, 0x2e, 0x6f, 0x0e, 0x57, 0x16, 0x5f, 0x1e, 0x67, 0x26, 0x6f, 0x15, 0x16, 0x1d,
	0x1e, 0x25, 0x66, 0x15, 0x5e, 0x1d, 0x66, 0x1c, 0x56, 0x1c, 0xe6, 0xc9, 0x1c, 0x00, 0x30, 0x5c,
	0x08, 0x5c, 0x10, 0x5c, 0x18, 0x5c, 0x20, 0x5c, 0x28, 0x5c, 0x29, 0x28, 0x21, 0x20, 0x19, 0x18,
	0x11, 0x10, 0x09, 0x08, 0x11, 0x19, 0x69, 0x09, 0x51, 0x10, 0x59, 0x18, 0x61, 0x20, 0x69, 0x09,
	0x12, 0x11, 0x1a, 0x19, 0x22, 0x61, 0x29, 0x62, 0x21, 0x5a, 0x19, 0x12, 0x62, 0x12, 0x1b, 0x62,
	0x1b, 0x48, 0x1b, 0x68, 0x1b, 0x40, 0x1b, 0xf0, 0xc9, 0x1e, 0x58, 0xdf, 0xa9, 0x08, 0x0a, 0x10,
	0x48, 0x18, 0x1a, 0x20, 0xd8, 0xe9, 0x21, 0x4f, 0x31, 0x5f, 0x0b, 0x7b, 0x05, 0xf5, 0xe9, 0x15,
	0x1e, 0x26, 0x2d, 0x2c, 0x13, 0x12, 0x19, 0x21, 0x6a, 0x18, 0x5f, 0x20, 0xe7, 0xe9, 0x28, 0x57,
	0x10, 0x09, 0x12, 0x19, 0x50, 0x2d, 0x26, 0x2f, 0x36, 0xed, 0xe9, 0x2d, 0x1f, 0x17, 0x0e, 0x0d,
	0x22, 0x19, 0x12, 0x1b, 0x2e, 0xf7, 0x69, 0x08, 0x0a, 0x10, 0xc8, 0x49, 0x10, 0x02, 0x05, 0xd7,
	0x49, 0x00, 0x12, 0x15, 0xc7, 0x69, 0x10, 0x54, 0x0b, 0x59, 0x09, 0xdb, 0xc9, 0x19, 0x5f, 0x04,
	0xf4, 0x29, 0x07, 0x0e, 0x05, 0x0d, 0xce, 0xa9, 0x04, 0xf4, 0x29, 0x06, 0x07, 0x0f, 0x0e, 0xc6,
	0x69, 0x18, 0xc7, 0xa9, 0x20, 0x08, 0x01, 0x06, 0x0f, 0x27, 0x2e, 0x29, 0x20, 0xcf, 0xc9, 0x03,
	0x18, 0x5f, 0x07, 0xf7, 0xa9, 0x01, 0x08, 0x20, 0x29, 0x2b, 0x07, 0x2f, 0xee, 0xa9, 0x01, 0x08,
	0x20, 0x29, 0x2b, 0x53, 0x2b, 0x2e, 0x27, 0x0f, 0xc6, 0xa9, 0x08, 0x03, 0x6b, 0x20, 0xe7, 0xa9,
	0x28, 0x08, 0x01, 0x04, 0x0b, 0x23, 0x2c, 0x2e, 0x27, 0x0f, 0xc6, 0xa9, 0x29, 0x20, 0x08, 0x01,
	0x06, 0x0f, 0x27, 0x2e, 0x2c, 0x23, 0x0b, 0xc4, 0xa9, 0x00, 0x28, 0x4f, 0x13, 0xe3, 0xa9, 0x08,
	0x01, 0x06, 0x0f, 0x27, 0x2e, 0x29, 0x20, 0x48, 0x03, 0xeb, 0xa9, 0x1f, 0x2c, 0x2b, 0x29, 0x20,
	0x08, 0x01, 0x03, 0x0c, 0x24, 0xeb, 0x29, 0x00, 0x01, 0x09, 0x08, 0x40, 0x06, 0x07, 0x0f, 0x0e,
	0xc6, 0x29, 0x03, 0x04, 0x0c, 0x0b, 0x43, 0x05, 0x0e, 0x0d, 0x45, 0x0e, 0xc7, 0x69, 0x19, 0x04,
	0xdf, 0xa9, 0x02, 0x6a, 0x05, 0xed, 0x69, 0x01, 0x1c, 0xc7, 0x89, 0x01, 0x08, 0x18, 0x21, 0x22,
	0x14, 0x56, 0xd7, 0xa9, 0x2f, 0x26, 0x24, 0x1b, 0x0b, 0x04, 0x06, 0x0f, 0x17, 0x1e, 0x1d, 0x14,
	0x0d, 0x16, 0xdd, 0xc9, 0x07, 0x19, 0x77, 0x0c, 0xec, 0xa9, 0x01, 0x07, 0x27, 0x2e, 0x2c, 0x23,
	0x2a, 0x21, 0x41, 0x03, 0xe3, 0xa9, 0x2a, 0x21, 0x09, 0x02, 0x06, 0x0f, 0x27, 0xee, 0xa9, 0x01,
	0x07, 0x17, 0x26, 0x2c, 0x22, 0x19, 0xc1, 0xa9, 0x29, 0x01, 0x07, 0x6f, 0x04, 0xdc, 0xa9, 0x07,
	0x01, 0x69, 0x04, 0xdc, 0xa9, 0x14, 0x24, 0x2d, 0x2e, 0x27, 0x0f, 0x06, 0x02, 0x09, 0x21, 0xea,
	0x89, 0x01, 0x47, 0x04, 0x64, 0x21, 0xe7, 0x89, 0x01, 0x61, 0x11, 0x57, 0x07, 0xe7, 0xa9, 0x09,
	0x69, 0x19, 0x1e, 0x17, 0x0f, 0xc6, 0xa9, 0x01, 0x47, 0x05, 0x0d, 0x61, 0x0d, 0xef, 0x89, 0x01,
	0x07, 0xe7, 0xc9, 0x07, 0x01, 0x1b, 0x31, 0xf7, 0x89, 0x07, 0x01, 0x27, 0xe1, 0xa9, 0x09, 0x02,
	0x06, 0x0f, 0x27, 0x2e, 0x2a, 0x21, 0xc9, 0x89, 0x07, 0x01, 0x19, 0x22, 0x23, 0x1c, 0xc4, 0xa9,
	0x2e, 0x27, 0x0f, 0x06, 0x02, 0x09, 0x21, 0x2a, 0x6e, 0x26, 0xef, 0x89, 0x07, 0x01, 0x19, 0x22,
	0x23, 0x1c, 0x04, 0xe7, 0xa9, 0x2a, 0x21, 0x09, 0x02, 0x03, 0x0c, 0x24, 0x2d, 0x2e, 0x27, 0x0f,
	0xc6, 0xc9, 0x1f, 0x58, 0x00, 0xf0, 0xa9, 0x01, 0x06, 0x0f, 0x27, 0x2e, 0xe9, 0x89, 0x01, 0x17,
	0xe1, 0xa9, 0x01, 0x06, 0x17, 0x1f, 0x59, 0x1f, 0x2f, 0xe9, 0xa9, 0x29, 0x47, 0x01, 0xef, 0x89,
	0x01, 0x14, 0x61, 0x14, 0xd7, 0xa9, 0x01, 0x29, 0x07, 0x6f, 0x0c, 0xe4, 0x69, 0x18, 0x00, 0x07,
	0xdf, 0x69, 0x00, 0xdf, 0x69, 0x00, 0x18, 0x1f, 0xc7, 0x89, 0x02, 0x10, 0xe2, 0xe9, 0x07, 0xff,
	0x49, 0x12, 0x00, 0x08, 0xd2, 0x89, 0x1c, 0x1e, 0x17, 0x0f, 0x06, 0x05, 0x0c, 0x14, 0x5d, 0x1e,
	0xe7, 0x69, 0x02, 0x47, 0x06, 0x0f, 0x17, 0x1e, 0x1d, 0x14, 0x0c, 0xc5, 0x49, 0x14, 0x0c, 0x05,
	0x06, 0x0f, 0xd7, 0x69, 0x1f, 0x5a, 0x1e, 0x17, 0x0f, 0x06, 0x05, 0x0c, 0x14, 0xdd, 0x69, 0x1e,
	0x17, 0x0f, 0x06, 0x05, 0x0c, 0x14, 0x1d, 0xc5, 0x89, 0x04, 0x5c, 0x23, 0x1a, 0x12, 0x0b, 0xcf,
	0x6c, 0x06, 0x0f, 0x17, 0x1e, 0x1a, 0x11, 0x09, 0x02, 0x03, 0x0c, 0x14, 0xdb, 0x69, 0x02, 0x07,
	0x05, 0x0c, 0x14, 0x1d, 0xdf, 0x09, 0x04, 0x06, 0x4f, 0xc3, 0x4c, 0x50, 0x12, 0x16, 0x0f, 0xc7,
	0x69, 0x02, 0x47, 0x05, 0x0d, 0x53, 0x0d, 0xdf, 0x49, 0x02, 0x06, 0x0f, 0xd7, 0xa9, 0x04, 0x47,
	0x05, 0x0c, 0x14, 0x1d, 0x5f, 0x1d, 0x1c, 0x24, 0x2d, 0xef, 0x69, 0x04, 0x47, 0x05, 0x0c, 0x14,
	0x1d, 0xdf, 0x69, 0x0c, 0x05, 0x06, 0x0f, 0x17, 0x1e, 0x1d, 0x14, 0xcc, 0x6c, 0x07, 0x02, 0x09,
	0x11, 0x1a, 0x1b, 0x14, 0x0c, 0xc3, 0x6c, 0x1b, 0x14, 0x0c, 0x03, 0x02, 0x09, 0x11, 0x1a, 0x1b,
	0x5f, 0x15, 0xdd, 0x69, 0x04, 0x47, 0x05, 0x0c, 0x14, 0xdd, 0x69, 0x1c, 0x0c, 0x05, 0x1e, 0x17,
	0xc7, 0x69, 0x03, 0x5b, 0x0a, 0x0e, 0x17, 0xdf, 0x69, 0x04, 0x06, 0x0f, 0x17, 0x5e, 0x1c, 0xdf,
	0x69, 0x04, 0x0f, 0x17, 0xdc, 0x69, 0x04, 0x06, 0x0f, 0x17, 0x54, 0x17, 0x1f, 0xdc, 0x69, 0x04,
	0x5f, 0x1c, 0xc7, 0x6c, 0x01, 0x02, 0x54, 0x19, 0x1a, 0x0f, 0xc7, 0x69, 0x04, 0x1c, 0x07, 0x5f,
	0x0d, 0xdd, 0x89, 0x20, 0x10, 0x09, 0x0b, 0x04, 0x0c, 0x0e, 0x17, 0xe7, 0x49, 0x08, 0xcf, 0x89,
	0x00, 0x10, 0x19, 0x1b, 0x24, 0x1c, 0x1e, 0x17, 0xc7, 0xa9, 0x03, 0x0a, 0x12, 0x1b, 0x23, 0xea,
	0xc9, 0x04, 0x07, 0x37, 0x34, 0x19, 0xc4, 0x0a, 0x35, 0x2e, 0x16, 0x0d, 0x09, 0x10, 0x28, 0x71,
	0x26, 0xdf, 0x09, 0x2f, 0x6c, 0x2e, 0x27, 0x1f, 0x16, 0x54, 0x5a, 0xea, 0x09, 0x2e, 0x27, 0x1f,
	0x16, 0x15, 0x1c, 0x24, 0x2d, 0x55, 0x2a, 0x23, 0x32, 0xea, 0x09, 0x37, 0x2e, 0x6c, 0x2d, 0x24,
	0x1c, 0x15, 0x16, 0x1f, 0x27, 0x6e, 0x2a, 0x21, 0xda, 0x09, 0x37, 0x2e, 0x6c, 0x2d, 0x24, 0x1c,
	0x15, 0x16, 0x1f, 0x27, 0x6e, 0x6a, 0xda, 0x09, 0x37, 0x2e, 0x6c, 0x2e, 0x27, 0x1f, 0x16, 0x15,
	0x1c, 0x24, 0x6d, 0x23, 0x12, 0x1a, 0xe3, 0x09, 0x37, 0x2e, 0x6c, 0x2e, 0x27, 0x1f, 0x16, 0x15,
	0x1c, 0x24, 0x6d, 0x2a, 0x23, 0x1a, 0x21, 0xea, 0x0a, 0x26, 0x1e, 0x15, 0x14, 0x1b, 0x63, 0x26,
	0xdf, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24, 0x2d, 0x55, 0x1a, 0x21, 0xea, 0x09, 0x2e,
	0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24, 0x2d, 0x55, 0x5a, 0xea, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15,
	0x1c, 0x24, 0x2d, 0x55, 0x1a, 0x09, 0x11, 0xda, 0x09, 0x27, 0x1e, 0x5b, 0x61, 0xd1, 0x09, 0x27,
	0x1e, 0x5b, 0x11, 0x18, 0xe1, 0x09, 0x27, 0x1e, 0x5b, 0x1a, 0x09, 0x11, 0xda, 0x09, 0x0f, 0x0c,
	0x19, 0x21, 0x34, 0x77, 0x0c, 0x74, 0x50, 0xe8, 0x09, 0x0f, 0x0c, 0x19, 0x21, 0x34, 0x77, 0x0c,
	0x74, 0x11, 0x18, 0x20, 0xe9, 0x09, 0x37, 0x0f, 0x09, 0x71, 0x0c, 0x6c, 0x28, 0xd9, 0x09, 0x36,
	0x2f, 0x27, 0x1e, 0x5c, 0x1e, 0x17, 0x0f, 0x06, 0x05, 0x0c, 0x14, 0x1d, 0x24, 0x2c, 0x35, 0xdd,
	0x09, 0x0f, 0x21, 0x67, 0x37, 0x67, 0x24, 0x6c, 0x21, 0x71, 0x15, 0xe5, 0x09, 0x2e, 0x27, 0x1f,
	0x16, 0x15, 0x1c, 0x24, 0x2d, 0x6e, 0x13, 0x1a, 0x22, 0xeb, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15,
	0x1c, 0x24, 0x2d, 0x6e, 0x6a, 0xda, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24, 0x2d, 0x6e,
	0x23, 0x12, 0x1a, 0xe3, 0x09, 0x2c, 0x2e, 0x77, 0x2e, 0x27, 0x1f, 0x16, 0x54, 0x13, 0x1a, 0x22,
	0xeb, 0x09, 0x14, 0x16, 0x1f, 0x27, 0x6e, 0x2c, 0x2e, 0x77, 0x1b, 0x0a, 0x12, 0xdb, 0x09, 0x2a,
	0x1f, 0x57, 0x24, 0x52, 0x68, 0xd8, 0x09, 0x12, 0x0b, 0x0e, 0x17, 0x2f, 0x36, 0x33, 0x2a, 0x52,
	0x50, 0xe8, 0x09, 0x32, 0x36, 0x2f, 0x17, 0x0e, 0x4a, 0x50, 0xe8, 0x09, 0x35, 0x2e, 0x16, 0x0d,
	0x0a, 0x11, 0x29, 0x72, 0x28, 0xd7, 0x09, 0x2f, 0x17, 0x1d, 0x13, 0x12, 0x19, 0x21, 0x2a, 0x6b,
	0x24, 0xd4, 0x09, 0x1f, 0x1c, 0x49, 0x1c, 0x69, 0x15, 0x65, 0x16, 0xe6, 0x09, 0x0f, 0x09, 0x11,
	0x1a, 0x1b, 0x14, 0x4c, 0x1c, 0x1e, 0x67, 0x15, 0x65, 0x35, 0x2d, 0x2e, 0x36, 0x37, 0xef, 0x09,
	0x29, 0x21, 0x1a, 0x1b, 0x1e, 0x17, 0x4f, 0x14, 0xe4, 0x09, 0x37, 0x2e, 0x6c, 0x2d, 0x24, 0x1c,
	0x15, 0x16, 0x1f, 0x27, 0x6e, 0x31, 0x22, 0x29, 0xf1, 0x09, 0x27, 0x1e, 0x5b, 0x1a, 0x29, 0x21,
	0xda, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24, 0x2d, 0x6e, 0x31, 0x22, 0x29, 0xf1, 0x09,
	0x37, 0x2e, 0x6c, 0x2e, 0x27, 0x1f, 0x16, 0x54, 0x22, 0x31, 0x29, 0xe2, 0x09, 0x14, 0x57, 0x15,
	0x1c, 0x24, 0x2d, 0x6f, 0x13, 0x1a, 0x23, 0xea, 0x09, 0x0f, 0x0a, 0x37, 0x72, 0x09, 0x10, 0x18,
	0x21, 0x29, 0xf0, 0x09, 0x18, 0x20, 0x22, 0x1a, 0x19, 0x61, 0x13, 0xeb, 0x09, 0x19, 0x1a, 0x22,
	0x21, 0x59, 0x13, 0xeb, 0x09, 0x36, 0x2f, 0x1f, 0x16, 0x15, 0x23, 0x61, 0xe0, 0x09, 0x0d, 0x0b,
	0xf3, 0x09, 0x35, 0x33, 0xcb, 0x09, 0x0a, 0x11, 0x53, 0x0f, 0x70, 0x25, 0x2c, 0x35, 0x26, 0xf6,
	0x09, 0x2c, 0x25, 0x75, 0x34, 0x76, 0x30, 0x4f, 0x0a, 0x11, 0xd3, 0x09, 0x60, 0x22, 0xe7, 0x09,
	0x19, 0x0c, 0x5f, 0x29, 0x1c, 0xef, 0x11, 0x21, 0x34, 0x67, 0x11, 0x24, 0xd7, 0x09, 0x78, 0x58,
	0x69, 0x49, 0x4b, 0x4d, 0x4f, 0x5e, 0x5c, 0x5a, 0x7a, 0x7c, 0x7e, 0x6d, 0x6b, 0xef, 0x09, 0x78,
	0x68, 0x58, 0x48, 0x51, 0x61, 0x41, 0x71, 0x4a, 0x5a, 0x6a, 0x7a, 0x73, 0x63, 0x53, 0x43, 0x4c,
	0x5c, 0x6c, 0x7c, 0x75, 0x65, 0x55, 0x45, 0x4e, 0x47, 0x57, 0x5e, 0x67, 0x6e, 0x77, 0xfe, 0x09,
	0x00, 0x78, 0x02, 0x7a, 0x04, 0x7c, 0x06, 0x7e, 0x00, 0x42, 0x0a, 0x4c, 0x14, 0x56, 0x10, 0x52,
	0x1a, 0x5c, 0x24, 0x66, 0x2e, 0x6f, 0x20, 0x62, 0x2a, 0x6c, 0x34, 0x76, 0x3e, 0x7f, 0x04, 0x46,
	0x0e, 0x4f, 0x1e, 0x5f, 0x3c, 0x7a, 0x32, 0xf0, 0x09, 0x1f, 0xd8, 0x09, 0x04, 0x1c, 0x5f, 0x18,
	0xdc, 0x09, 0x04, 0x1c, 0x5f, 0x03, 0x1b, 0xd8, 0x09, 0x04, 0x1c, 0x5f, 0x27, 0x60, 0x18, 0xdc,
	0x09, 0x04, 0x1c, 0x5f, 0x27, 0xe4, 0x09, 0x04, 0x1c, 0x5f, 0x27, 0x60, 0x03, 0x1b, 0xd8, 0x09,
	0x04, 0x1c, 0x5f, 0x27, 0x60, 0x03, 0x1b, 0xd8, 0x09, 0x27, 0x60, 0x18, 0xdf, 0x09, 0x04, 0x1c,
	0x5f, 0x27, 0x23, 0xc3, 0x09, 0x04, 0x24, 0x60, 0x18, 0x1b, 0xc3, 0x09, 0x03, 0x1b, 0x58, 0x20,
	0xe3, 0x09, 0x1c, 0x44, 0x03, 0x1b, 0xd8, 0x09, 0x04, 0x1c, 0xdf, 0x09, 0x3c, 0x1c, 0xd8, 0x09,
	0x3c, 0x44, 0x1c, 0xd8, 0x09, 0x3c, 0x44, 0x1c, 0xdf, 0x09, 0x3c, 0x5c, 0x18, 0xdf, 0x09, 0x3c,
	0xc4, 0x09, 0x3c, 0x44, 0x18, 0xdf, 0x09, 0x3c, 0x24, 0x67, 0x3b, 0x23, 0xe0, 0x09, 0x3c, 0x64,
	0x20, 0x67, 0x1f, 0xd8, 0x09, 0x3c, 0x1c, 0x58, 0x20, 0x23, 0xfb, 0x09, 0x3c, 0x24, 0x67, 0x1f,
	0x1b, 0xfb, 0x09, 0x3c, 0x44, 0x03, 0x1b, 0x58, 0x20, 0x23, 0xfb, 0x09, 0x3b, 0x43, 0x04, 0x1c,
	0x5f, 0x27, 0x24, 0xfc, 0x09, 0x1f, 0x58, 0x3b, 0x23, 0x60, 0x27, 0x24, 0xfc, 0x09, 0x3c, 0x44,
	0x03, 0xfb, 0x09, 0x27, 0x24, 0x7c, 0x3b, 0x23, 0x60, 0x18, 0x1b, 0x43, 0x04, 0x1c, 0xdf, 0x09,
	0x3c, 0x44, 0x03, 0xfb, 0x09, 0x3c, 0x44, 0x24, 0x60, 0x1c, 0xd8, 0x09, 0x1f, 0x5c, 0x04, 0x7c,
	0x3b, 0xc3, 0x09, 0x1f, 0x5c, 0x27, 0x64, 0x04, 0xfc, 0x09, 0x3b, 0x23, 0x60, 0x18, 0xdb, 0x09,
	0x3b, 0x23, 0x60, 0x18, 0x5b, 0x24, 0xfc, 0x09, 0x3b, 0x63, 0x3c, 0x24, 0xe7, 0x09, 0x3c, 0x24,
	0x67, 0x1f, 0xdc, 0x09, 0x27, 0x60, 0x1f, 0x58, 0x04, 0x5c, 0x24, 0xfc, 0x09, 0x1f, 0x5c, 0x1b,
	0x58, 0x03, 0x7b, 0x04, 0xfc, 0x09, 0x04, 0x1c, 0xd8, 0x09, 0x1f, 0x1c, 0xfc, 0x09, 0x00, 0x07,
	0x0f, 0x08, 0x10, 0x17, 0x1f, 0x18, 0x20, 0x27, 0x2f, 0x28, 0x30, 0x37, 0x3f, 0x78, 0x01, 0x79,
	0x3a, 0x42, 0x03, 0x7b, 0x3c, 0x44, 0x05, 0x7d, 0x3e, 0x46, 0x00, 0x78, 0x3f, 0x07, 0x08, 0x17,
	0x18, 0x27, 0x28, 0x37, 0x78, 0x00, 0x0f, 0x10, 0x1f, 0x20, 0x2f, 0x30, 0x7f, 0x07, 0x3e, 0x05,
	0x3c, 0x03, 0x3a, 0x01, 0x78, 0x00, 0x39, 0x02, 0x3b, 0x04, 0x3d, 0x06, 0xff, 0x09, 0x04, 0x07,
	0x0c, 0x0f, 0x14, 0x17, 0x1c, 0x1f, 0x24, 0x27, 0x2c, 0x2f, 0x34, 0x37, 0x3c, 0x3f, 0x74, 0x0c,
	0x57, 0x1c, 0x67, 0x2c, 0x77, 0x24, 0x6f, 0x14, 0x5f, 0x04, 0x4f, 0x07, 0x7f, 0x3c, 0x44, 0x05,
	0x3d, 0x06, 0x3e, 0x05, 0x7c, 0x3f, 0x46, 0x07, 0x7e, 0x04, 0xfd, 0x09, 0x18, 0x1f, 0x07, 0x00,
	0x18, 0x17, 0x10, 0x0f, 0x08, 0x47, 0x00, 0x4f, 0x08, 0x57, 0x10, 0x5f, 0x06, 0x1e, 0x05, 0x1c,
	0x04, 0x1b, 0x03, 0x1a, 0x02, 0x19, 0x01, 0x58, 0x1f, 0x46, 0x05, 0x5d, 0x07, 0x5e, 0x1d, 0x44,
	0x1b, 0x42, 0x1a, 0x41, 0x19, 0x40, 0x1d, 0xc6, 0x09, 0x18, 0x1f, 0x3f, 0x38, 0x18, 0x39, 0x19,
	0x3a, 0x1a, 0x3b, 0x1b, 0x3c, 0x1c, 0x3d, 0x1d, 0x3e, 0x1e, 0x7f, 0x1f, 0x7e, 0x1e, 0x7d, 0x3c,
	0x5d, 0x1c, 0x7b, 0x3a, 0x5b, 0x1a, 0x79, 0x38, 0x59, 0x1f, 0x20, 0x27, 0x28, 0x2f, 0x30, 0x37,
	0x78, 0x3f, 0x70, 0x37, 0x68, 0x2f, 0x60, 0x27, 0xd8, 0x09, 0x3c, 0x04, 0x00, 0x38, 0x3c, 0x30,
	0x34, 0x28, 0x2c, 0x20, 0x24, 0x18, 0x1c, 0x10, 0x14, 0x08, 0x0c, 0x40, 0x04, 0x48, 0x0c, 0x50,
	0x14, 0x58, 0x1c, 0x60, 0x24, 0x68, 0x2c, 0x70, 0x34, 0x78, 0x04, 0x3b, 0x03, 0x3a, 0x02, 0x39,
	0x01, 0x78, 0x00, 0x79, 0x01, 0x7a, 0x02, 0x7b, 0x03, 0xfc, 0x09, 0x34, 0x2e, 0x27, 0x1f, 0x16,
	0x15, 0x1c, 0x24, 0x2d, 0x2e, 0xf7, 0x09, 0x17, 0x12, 0x19, 0x21, 0x2a, 0x2b, 0x24, 0x5c, 0x24,
	0x35, 0x36, 0x2f, 0xdf, 0x09, 0x17, 0x11, 0xe9, 0x05, 0x0f, 0x17, 0x5c, 0x2c, 0x2f, 0x77, 0x0d,
	0x14, 0x2c, 0xf3, 0x09, 0x31, 0x09, 0x24, 0x0f, 0xf7, 0x09, 0x34, 0x1c, 0x15, 0x16, 0x1f, 0x27,
	0x2e, 0x2d, 0xe4, 0x09, 0x17, 0x53, 0x15, 0x1e, 0x26, 0x2d, 0x6b, 0x2d, 0xf6, 0x09, 0x1f, 0x5b,
	0x13, 0xe3, 0x09, 0x11, 0x0a, 0x0c, 0x15, 0x2d, 0x34, 0x32, 0x29, 0x19, 0xdf, 0x09, 0x29, 0x11,
	0x0a, 0x0e, 0x17, 0x2f, 0x36, 0x32, 0x69, 0x14, 0xec, 0x09, 0x0f, 0x17, 0x0e, 0x0a, 0x11, 0x29,
	0x32, 0x36, 0x2f, 0xf7, 0x09, 0x20, 0x19, 0x22, 0x2a, 0x33, 0x36, 0x2f, 0x17, 0x0e, 0x0b, 0x12,
	0xe2, 0x09, 0x0c, 0x05, 0x06, 0x0f, 0x17, 0x2c, 0x34, 0x3d, 0x3e, 0x37, 0x2f, 0x14, 0xcc, 0x09,
	0x1b, 0x14, 0x15, 0x1e, 0x26, 0x2d, 0x2c, 0x23, 0x5b, 0x22, 0xdf, 0x06, 0x2a, 0x12, 0x0b, 0x14,
	0x6c, 0x14, 0x0d, 0x16, 0xee, 0x09, 0x17, 0x14, 0x1b, 0x23, 0x2c, 0xef, 0x09, 0x0b, 0x73, 0x0c,
	0x74, 0x0d, 0xf5, 0x09, 0x0e, 0x6e, 0x1d, 0x59, 0x0b, 0xeb, 0x09, 0x0e, 0x6e, 0x0d, 0x2b, 0xc9,
	0x05, 0x29, 0x0b, 0x6d, 0x0e, 0xee, 0x09, 0x29, 0x20, 0x18, 0x11, 0xd7, 0x09, 0x10, 0xd7, 0x09,
	0x1a, 0x1b, 0x23, 0x22, 0x5a, 0x0c, 0x74, 0x1d, 0x1e, 0x26, 0x25, 0xdd, 0x09, 0x0e, 0x15, 0x1d,
	0x26, 0x2e, 0x75, 0x0c, 0x13, 0x1b, 0x24, 0x2c, 0xf3, 0x09, 0x18, 0x19, 0x21, 0x20, 0xd8, 0x80,
	0x09, 0x0c, 0x14, 0x1f, 0xe8, 0x09, 0x0c, 0x14, 0x1f, 0xe8, 0x09, 0x10, 0x52, 0x11, 0x18, 0x21,
	0xe2, 0x09, 0x11, 0x18, 0x21, 0x12, 0xe2, 0x09, 0x0a, 0x0f, 0x37, 0x32, 0x0a, 0x17, 0x12, 0x1f,
	0x1a, 0x27, 0x22, 0x2f, 0x2a, 0x77, 0x32, 0x6f, 0x2a, 0x67, 0x22, 0x5f, 0x1a, 0x57, 0x12, 0x4f,
	0x0a, 0x33, 0x0b, 0x34, 0x0c, 0x35, 0x0d, 0x36, 0x0e, 0x77, 0x32, 0x4b, 0x33, 0x4c, 0x34, 0x4d,
	0x35, 0x4e, 0x36, 0xcf, 0x80, 0x00, 0xff, };

#endif
