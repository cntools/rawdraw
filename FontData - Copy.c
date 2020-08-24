#ifndef FONTINIT
#define FONTINIT
int CharIndex[] = {
	   0,    1,   13,   25,   40,   49,   76,   93,  113,    0,    0,  143,  158,    0,  173,  194,
	 228,  233,  238,  248,  255,  268,  285,  330,  341,  347,  353,  359,  365,  369,  378,  443,
	   0,  504,  508,  517,  526,  541,  554,  566,  571,  576,  581,  592,  597,  603,  606,  612,
	 615,  616,  622,  633,  645,  651,  662,  675,  681,  693,  704,  715,  726,  730,  735,  739,
	 748,  764,  773,  786,  795,  804,  811,  817,  829,  836,  843,  851,  858,  862,  868,  873,
	 883,  892,  904,  914,  926,  931,  938,  945,  954,  959,  965,  972,  977,  980,  985,  989,
	 992,  997, 1009, 1020, 1027, 1038, 1048, 1056, 1069, 1077, 1082, 1088, 1096, 1101, 1114, 1123,
	1133, 1143, 1155, 1162, 1169, 1176, 1184, 1189, 1198, 1203, 1209, 1216, 1226, 1229, 1239, 1246,
	1253, 1264, 1274, 1288, 1303, 1317, 1333, 1350, 1359, 1372, 1384, 1398, 1404, 1411, 1419, 1430,
	1443, 1452, 1470, 1482, 1496, 1508, 1522, 1535, 1548, 1556, 1568, 1577, 1588, 1600, 1610, 1629,
	1639, 1655, 1663, 1677, 1690, 1702, 1713, 1722, 1730, 1739, 1743, 1747, 1758, 1769, 1773, 1780,
	1787, 1804, 1837, 1878, 1881, 1887, 1894, 1902, 1908, 1917, 1926, 1931, 1938, 1945, 1951, 1957,
	1961, 1965, 1970, 1975, 1980, 1983, 1988, 1995, 2002, 2009, 2016, 2025, 2034, 2043, 2048, 2061,
	2066, 2073, 2080, 2087, 2093, 2101, 2107, 2113, 2122, 2131, 2135, 2139, 2203, 2249, 2294, 2343,
	2392, 2404, 2418, 2422, 2433, 2439, 2449, 2459, 2464, 2475, 2487, 2498, 2511, 2525, 2537, 2547,
	2554, 2561, 2568, 2574, 2580, 2586, 2589, 2602, 2615,    0,    0, 2621, 2626, 2633, 2639,    0,
	};

unsigned char FontData[] = {
	0x80, 0x09, 0x0c, 0x0d, 0x16, 0x2e, 0x35, 0x34, 0x4c, 0x11, 0x53, 0x29, 0xeb, 0x09, 0x11, 0x53,
	0x29, 0x6b, 0x14, 0x2c, 0x35, 0x36, 0x0e, 0x0d, 0xd4, 0x09, 0x27, 0x0d, 0x03, 0x01, 0x08, 0x10,
	0x62, 0x1a, 0x28, 0x30, 0x39, 0x3b, 0x35, 0xdf, 0x09, 0x27, 0x4b, 0x0c, 0x60, 0x18, 0x74, 0x33,
	0xdf, 0x09, 0x1b, 0x12, 0x11, 0x18, 0x20, 0x29, 0x2a, 0x63, 0x2a, 0x32, 0x3b, 0x3c, 0x35, 0x2d,
	0x64, 0x1c, 0x15, 0x0d, 0x04, 0x03, 0x0a, 0x52, 0x1c, 0x1f, 0x27, 0xe4, 0x09, 0x1f, 0x1d, 0x16,
	0x0e, 0x05, 0x04, 0x0b, 0x60, 0x18, 0x3c, 0x3d, 0x36, 0x2e, 0x25, 0x27, 0xdf, 0x09, 0x00, 0x08,
	0x11, 0x12, 0x0b, 0x14, 0x15, 0x0e, 0x06, 0x40, 0x28, 0x18, 0x1e, 0x6e, 0x23, 0x5b, 0x30, 0x36,
	0xfe, 0x09, 0x00, 0x02, 0x0a, 0x09, 0x41, 0x00, 0x08, 0x49, 0x12, 0x10, 0x18, 0x5a, 0x11, 0x59,
	0x28, 0x20, 0x22, 0x6a, 0x30, 0x72, 0x31, 0x78, 0x31, 0x7a, 0x13, 0x05, 0x57, 0x05, 0xfd, 0x09,
	0x20, 0x38, 0x7b, 0x1f, 0x17, 0x05, 0x04, 0x12, 0x1a, 0x2c, 0x2d, 0x5f, 0x23, 0xf8, 0x09, 0x20,
	0x10, 0x09, 0x0b, 0x14, 0x24, 0x2b, 0x29, 0x60, 0x1c, 0x1f, 0x4d, 0x1f, 0xed, 0x09, 0x38, 0x59,
	0x39, 0x5a, 0x19, 0x1e, 0x15, 0x0d, 0x06, 0x0f, 0x17, 0x5e, 0x3e, 0x35, 0x2d, 0x26, 0x2f, 0x37,
	0x3e, 0xf8, 0x09, 0x22, 0x1a, 0x13, 0x14, 0x1d, 0x25, 0x2c, 0x2b, 0x62, 0x38, 0x6a, 0x3f, 0x6d,
	0x07, 0x55, 0x12, 0x40, 0x01, 0x4a, 0x30, 0x69, 0x35, 0x7e, 0x16, 0x4f, 0x0d, 0x46, 0x11, 0x48,
	0x32, 0x79, 0x37, 0xee, 0x09, 0x07, 0x3c, 0x01, 0xc7, 0x09, 0x39, 0x3f, 0x04, 0xf9, 0x09, 0x18,
	0x42, 0x18, 0x72, 0x18, 0x5f, 0x05, 0x1f, 0xf5, 0x09, 0x10, 0x55, 0x57, 0x28, 0x6d, 0xef, 0x09,
	0x30, 0x18, 0x11, 0x12, 0x1b, 0x23, 0x60, 0x28, 0x2e, 0x27, 0x1f, 0xd6, 0x09, 0x29, 0x20, 0x18,
	0x11, 0x12, 0x23, 0x2c, 0x6d, 0x16, 0x1f, 0x27, 0x2e, 0x2d, 0x1c, 0x13, 0xd2, 0x09, 0x04, 0x07,
	0x3f, 0x3c, 0x04, 0x0f, 0x0c, 0x17, 0x14, 0x1f, 0x1c, 0x27, 0x24, 0x2f, 0x2c, 0x37, 0x34, 0x7f,
	0x3c, 0x77, 0x34, 0x6f, 0x2c, 0x67, 0x24, 0x5f, 0x1c, 0x57, 0x14, 0x4f, 0x0c, 0x47, 0x04, 0x3d,
	0x05, 0x3e, 0x06, 0x7f, 0x3c, 0x45, 0x06, 0x7d, 0x3e, 0xc7, 0x09, 0x0f, 0x6f, 0x0d, 0x1e, 0x6d,
	0x1e, 0x18, 0x49, 0x18, 0xe9, 0x09, 0x1f, 0x18, 0x49, 0x18, 0xe9, 0x09, 0x18, 0x1f, 0x4e, 0x2e,
	0xdf, 0x09, 0x04, 0x3c, 0x72, 0x3c, 0xf6, 0x09, 0x3c, 0x04, 0x4a, 0x04, 0xce, 0x09, 0x03, 0x07,
	0xff, 0x09, 0x0a, 0x04, 0x4e, 0x04, 0x3c, 0x72, 0x3c, 0xf6, 0x09, 0x1b, 0x07, 0x37, 0x1b, 0x4f,
	0x17, 0x1b, 0x5f, 0x1b, 0x67, 0x1b, 0x6f, 0x2e, 0x4e, 0x25, 0x55, 0x1c, 0x4e, 0x1c, 0x6e, 0x07,
	0x4e, 0x16, 0x47, 0x26, 0x77, 0x2e, 0x77, 0x1e, 0x47, 0x1e, 0x77, 0x0e, 0x0f, 0x16, 0x17, 0x1e,
	0x1f, 0x26, 0x27, 0x2e, 0x6f, 0x0e, 0x57, 0x16, 0x5f, 0x1e, 0x67, 0x26, 0x6f, 0x15, 0x16, 0x1d,
	0x1e, 0x25, 0x66, 0x15, 0x5e, 0x1d, 0x66, 0x1c, 0x56, 0x1c, 0xe6, 0x09, 0x1c, 0x00, 0x30, 0x5c,
	0x08, 0x5c, 0x10, 0x5c, 0x18, 0x5c, 0x20, 0x5c, 0x28, 0x5c, 0x29, 0x28, 0x21, 0x20, 0x19, 0x18,
	0x11, 0x10, 0x09, 0x08, 0x11, 0x19, 0x69, 0x09, 0x51, 0x10, 0x59, 0x18, 0x61, 0x20, 0x69, 0x09,
	0x12, 0x11, 0x1a, 0x19, 0x22, 0x61, 0x29, 0x62, 0x21, 0x5a, 0x19, 0x12, 0x62, 0x12, 0x1b, 0x62,
	0x1b, 0x48, 0x1b, 0x68, 0x1b, 0x40, 0x1b, 0xf0, 0x09, 0x1e, 0x58, 0xdf, 0x09, 0x10, 0x12, 0x18,
	0x50, 0x20, 0x22, 0x28, 0xe0, 0x09, 0x21, 0x4f, 0x31, 0x5f, 0x0b, 0x7b, 0x05, 0xf5, 0x09, 0x15,
	0x1e, 0x26, 0x2d, 0x2c, 0x13, 0x12, 0x19, 0x21, 0x6a, 0x18, 0x5f, 0x20, 0xe7, 0x09, 0x28, 0x57,
	0x10, 0x09, 0x12, 0x19, 0x50, 0x2d, 0x26, 0x2f, 0x36, 0xed, 0x09, 0x2d, 0x1f, 0x17, 0x0e, 0x0d,
	0x22, 0x19, 0x12, 0x1b, 0x2e, 0xf7, 0x09, 0x18, 0x1a, 0x20, 0xd8, 0x09, 0x20, 0x12, 0x15, 0xe7,
	0x09, 0x18, 0x2a, 0x2d, 0xdf, 0x09, 0x20, 0x63, 0x28, 0x5b, 0x18, 0x6b, 0x31, 0x52, 0x11, 0xf2,
	0x09, 0x19, 0x5f, 0x04, 0xf4, 0x09, 0x1f, 0x26, 0x1d, 0x25, 0xe6, 0x09, 0x04, 0xf4, 0x09, 0x1f,
	0x1e, 0x26, 0x27, 0xdf, 0x09, 0x28, 0xd7, 0xe9, 0x09, 0x0b, 0x20, 0x27, 0x0f, 0xf7, 0x09, 0x09,
	0x10, 0x20, 0x29, 0x2a, 0x23, 0x0e, 0x0f, 0x2f, 0xee, 0x09, 0x09, 0x10, 0x28, 0x31, 0x33, 0x53,
	0x33, 0x36, 0x2f, 0x17, 0xce, 0x09, 0x10, 0x0b, 0x73, 0x28, 0xef, 0x09, 0x30, 0x10, 0x09, 0x0b,
	0x2b, 0x34, 0x36, 0x2f, 0x17, 0xce, 0x09, 0x31, 0x28, 0x10, 0x09, 0x0e, 0x17, 0x2f, 0x36, 0x34,
	0x2b, 0x13, 0xcc, 0x09, 0x08, 0x30, 0x5f, 0x2b, 0xdb, 0x09, 0x09, 0x0e, 0x17, 0x2f, 0x36, 0x31,
	0x28, 0x10, 0x49, 0x0b, 0xf3, 0x09, 0x27, 0x34, 0x31, 0x28, 0x10, 0x09, 0x0b, 0x14, 0x2c, 0xf3,
	0x09, 0x1f, 0x1e, 0x26, 0x27, 0x5f, 0x18, 0x19, 0x21, 0x20, 0xd8, 0x09, 0x1f, 0x26, 0x25, 0x1d,
	0x66, 0x1b, 0x1c, 0x24, 0x23, 0xdb, 0x09, 0x29, 0x14, 0xef, 0x09, 0x32, 0x4a, 0x0d, 0xf5, 0x09,
	0x11, 0x2c, 0xd7, 0x09, 0x5f, 0x1e, 0x1c, 0x2a, 0x29, 0x20, 0x10, 0xc9, 0x09, 0x24, 0x1d, 0x14,
	0x1b, 0x24, 0x25, 0x1e, 0x16, 0x0d, 0x0b, 0x12, 0x22, 0x2b, 0x2d, 0xf6, 0x0a, 0x0e, 0x0b, 0x18,
	0x20, 0x33, 0x76, 0x0b, 0xf3, 0x0a, 0x0e, 0x08, 0x28, 0x31, 0x32, 0x2b, 0x4b, 0x0e, 0x2e, 0x35,
	0x34, 0xeb, 0x0a, 0x35, 0x2e, 0x16, 0x0d, 0x09, 0x10, 0x28, 0xf1, 0x0a, 0x08, 0x0e, 0x1e, 0x2d,
	0x33, 0x29, 0x18, 0xc8, 0x0a, 0x30, 0x08, 0x0e, 0x76, 0x2b, 0xcb, 0x0a, 0x30, 0x08, 0x4e, 0x0b,
	0xeb, 0x09, 0x31, 0x28, 0x10, 0x09, 0x0d, 0x16, 0x2e, 0x35, 0x34, 0x2b, 0xdb, 0x0a, 0x08, 0x4e,
	0x0b, 0x73, 0x30, 0xf6, 0x0a, 0x28, 0x48, 0x0e, 0x6e, 0x18, 0xde, 0x0a, 0x08, 0x30, 0x20, 0x25,
	0x1e, 0x16, 0xcd, 0x0a, 0x08, 0x4e, 0x0c, 0x69, 0x0c, 0xf6, 0x09, 0x10, 0x16, 0xf6, 0x0a, 0x0e,
	0x08, 0x22, 0x30, 0xf6, 0x0a, 0x0e, 0x08, 0x36, 0xf0, 0x0a, 0x10, 0x09, 0x0d, 0x16, 0x2e, 0x35,
	0x31, 0x28, 0xd0, 0x0a, 0x08, 0x4e, 0x08, 0x28, 0x31, 0x33, 0x2c, 0xcc, 0x0a, 0x28, 0x10, 0x09,
	0x0d, 0x16, 0x2e, 0x35, 0x31, 0x68, 0x2d, 0xf6, 0x0a, 0x08, 0x4e, 0x08, 0x28, 0x31, 0x33, 0x2c,
	0x0c, 0xee, 0x0a, 0x31, 0x28, 0x10, 0x09, 0x0a, 0x13, 0x2b, 0x34, 0x35, 0x2e, 0xce, 0x0a, 0x1f,
	0x58, 0x00, 0xf0, 0x0a, 0x08, 0x0d, 0x16, 0x2e, 0x35, 0xf0, 0x0a, 0x08, 0x15, 0x1e, 0x26, 0x2d,
	0xf0, 0x0a, 0x08, 0x0d, 0x1e, 0x26, 0x60, 0x26, 0x36, 0xf0, 0x0a, 0x30, 0x4e, 0x08, 0xf6, 0x0a,
	0x08, 0x64, 0x30, 0x1e, 0xd6, 0x0a, 0x08, 0x30, 0x0e, 0x76, 0x32, 0xd2, 0x09, 0x28, 0x10, 0x17,
	0xef, 0x09, 0x10, 0xef, 0x09, 0x10, 0x28, 0x2f, 0xd7, 0x09, 0x12, 0x20, 0xf2, 0x09, 0x07, 0xff,
	0x09, 0x10, 0x22, 0x18, 0xd0, 0x09, 0x2d, 0x24, 0x1c, 0x15, 0x16, 0x1f, 0x27, 0x6e, 0x2c, 0x2e,
	0xf7, 0x09, 0x12, 0x57, 0x15, 0x1c, 0x24, 0x2d, 0x2e, 0x27, 0x1f, 0xd6, 0x09, 0x27, 0x1f, 0x16,
	0x15, 0x1c, 0xe4, 0x09, 0x2a, 0x6f, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24, 0xed, 0x09, 0x2e,
	0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24, 0x2d, 0xd5, 0x09, 0x17, 0x13, 0x1a, 0x22, 0x6b, 0x0c, 0xe4,
	0x0c, 0x16, 0x1f, 0x27, 0x2e, 0x2a, 0x21, 0x19, 0x12, 0x13, 0x1c, 0x24, 0xeb, 0x09, 0x12, 0x57,
	0x15, 0x1c, 0x24, 0x2d, 0xef, 0x09, 0x1c, 0x1e, 0x67, 0xda, 0x0c, 0x17, 0x1f, 0x26, 0x62, 0xe0,
	0x09, 0x12, 0x57, 0x15, 0x1d, 0x6b, 0x1d, 0xef, 0x09, 0x1a, 0x1e, 0x27, 0xef, 0x09, 0x0f, 0x4c,
	0x0d, 0x14, 0x1c, 0x25, 0x67, 0x25, 0x24, 0x2c, 0x35, 0xf7, 0x09, 0x14, 0x57, 0x15, 0x1c, 0x24,
	0x2d, 0x2e, 0xef, 0x09, 0x2d, 0x24, 0x1c, 0x15, 0x16, 0x1f, 0x27, 0x2e, 0xed, 0x0c, 0x17, 0x12,
	0x19, 0x21, 0x2a, 0x2b, 0x24, 0x1c, 0xd3, 0x0c, 0x2f, 0x2a, 0x21, 0x19, 0x12, 0x13, 0x1c, 0x24,
	0x6b, 0x25, 0xf5, 0x09, 0x17, 0x54, 0x15, 0x1c, 0x24, 0xed, 0x09, 0x17, 0x27, 0x2e, 0x15, 0x1c,
	0xec, 0x09, 0x13, 0x6b, 0x1a, 0x1e, 0x27, 0xef, 0x09, 0x14, 0x16, 0x1f, 0x27, 0x6e, 0x2f, 0xec,
	0x09, 0x14, 0x1f, 0x27, 0xec, 0x09, 0x14, 0x16, 0x1f, 0x27, 0x64, 0x27, 0x2f, 0xec, 0x09, 0x17,
	0x6c, 0x14, 0xef, 0x0c, 0x12, 0x64, 0x2a, 0x1f, 0xd7, 0x09, 0x14, 0x2c, 0x17, 0x6f, 0x1d, 0xed,
	0x09, 0x28, 0x18, 0x11, 0x13, 0x0c, 0x14, 0x16, 0x1f, 0xef, 0x09, 0x18, 0xdf, 0x09, 0x10, 0x20,
	0x29, 0x2b, 0x34, 0x2c, 0x2e, 0x27, 0xd7, 0x09, 0x0b, 0x12, 0x1a, 0x23, 0x2b, 0xf2, 0x09, 0x04,
	0x07, 0x37, 0x34, 0x19, 0xc4, 0x0a, 0x35, 0x2e, 0x16, 0x0d, 0x09, 0x10, 0x28, 0x71, 0x26, 0xdf,
	0x09, 0x2f, 0x6c, 0x2e, 0x27, 0x1f, 0x16, 0x54, 0x5a, 0xea, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15,
	0x1c, 0x24, 0x2d, 0x55, 0x2a, 0x23, 0x32, 0xea, 0x09, 0x37, 0x2e, 0x6c, 0x2d, 0x24, 0x1c, 0x15,
	0x16, 0x1f, 0x27, 0x6e, 0x2a, 0x21, 0xda, 0x09, 0x37, 0x2e, 0x6c, 0x2d, 0x24, 0x1c, 0x15, 0x16,
	0x1f, 0x27, 0x6e, 0x6a, 0xda, 0x09, 0x37, 0x2e, 0x6c, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24,
	0x6d, 0x23, 0x12, 0x1a, 0xe3, 0x09, 0x37, 0x2e, 0x6c, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24,
	0x6d, 0x2a, 0x23, 0x1a, 0x21, 0xea, 0x0a, 0x26, 0x1e, 0x15, 0x14, 0x1b, 0x63, 0x26, 0xdf, 0x09,
	0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24, 0x2d, 0x55, 0x1a, 0x21, 0xea, 0x09, 0x2e, 0x27, 0x1f,
	0x16, 0x15, 0x1c, 0x24, 0x2d, 0x55, 0x5a, 0xea, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24,
	0x2d, 0x55, 0x1a, 0x09, 0x11, 0xda, 0x09, 0x27, 0x1e, 0x5b, 0x61, 0xd1, 0x09, 0x27, 0x1e, 0x5b,
	0x11, 0x18, 0xe1, 0x09, 0x27, 0x1e, 0x5b, 0x1a, 0x09, 0x11, 0xda, 0x09, 0x0f, 0x0c, 0x19, 0x21,
	0x34, 0x77, 0x0c, 0x74, 0x50, 0xe8, 0x09, 0x0f, 0x0c, 0x19, 0x21, 0x34, 0x77, 0x0c, 0x74, 0x11,
	0x18, 0x20, 0xe9, 0x09, 0x37, 0x0f, 0x09, 0x71, 0x0c, 0x6c, 0x28, 0xd9, 0x09, 0x36, 0x2f, 0x27,
	0x1e, 0x5c, 0x1e, 0x17, 0x0f, 0x06, 0x05, 0x0c, 0x14, 0x1d, 0x24, 0x2c, 0x35, 0xdd, 0x09, 0x0f,
	0x21, 0x67, 0x37, 0x67, 0x24, 0x6c, 0x21, 0x71, 0x15, 0xe5, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15,
	0x1c, 0x24, 0x2d, 0x6e, 0x13, 0x1a, 0x22, 0xeb, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24,
	0x2d, 0x6e, 0x6a, 0xda, 0x09, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24, 0x2d, 0x6e, 0x23, 0x12,
	0x1a, 0xe3, 0x09, 0x2c, 0x2e, 0x77, 0x2e, 0x27, 0x1f, 0x16, 0x54, 0x13, 0x1a, 0x22, 0xeb, 0x09,
	0x14, 0x16, 0x1f, 0x27, 0x6e, 0x2c, 0x2e, 0x77, 0x1b, 0x0a, 0x12, 0xdb, 0x09, 0x2a, 0x1f, 0x57,
	0x24, 0x52, 0x68, 0xd8, 0x09, 0x12, 0x0b, 0x0e, 0x17, 0x2f, 0x36, 0x33, 0x2a, 0x52, 0x50, 0xe8,
	0x09, 0x32, 0x36, 0x2f, 0x17, 0x0e, 0x4a, 0x50, 0xe8, 0x09, 0x35, 0x2e, 0x16, 0x0d, 0x0a, 0x11,
	0x29, 0x72, 0x28, 0xd7, 0x09, 0x2f, 0x17, 0x1d, 0x13, 0x12, 0x19, 0x21, 0x2a, 0x6b, 0x24, 0xd4,
	0x09, 0x1f, 0x1c, 0x49, 0x1c, 0x69, 0x15, 0x65, 0x16, 0xe6, 0x09, 0x0f, 0x09, 0x11, 0x1a, 0x1b,
	0x14, 0x4c, 0x1c, 0x1e, 0x67, 0x15, 0x65, 0x35, 0x2d, 0x2e, 0x36, 0x37, 0xef, 0x09, 0x29, 0x21,
	0x1a, 0x1b, 0x1e, 0x17, 0x4f, 0x14, 0xe4, 0x09, 0x37, 0x2e, 0x6c, 0x2d, 0x24, 0x1c, 0x15, 0x16,
	0x1f, 0x27, 0x6e, 0x31, 0x22, 0x29, 0xf1, 0x09, 0x27, 0x1e, 0x5b, 0x1a, 0x29, 0x21, 0xda, 0x09,
	0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c, 0x24, 0x2d, 0x6e, 0x31, 0x22, 0x29, 0xf1, 0x09, 0x37, 0x2e,
	0x6c, 0x2e, 0x27, 0x1f, 0x16, 0x54, 0x22, 0x31, 0x29, 0xe2, 0x09, 0x14, 0x57, 0x15, 0x1c, 0x24,
	0x2d, 0x6f, 0x13, 0x1a, 0x23, 0xea, 0x09, 0x0f, 0x0a, 0x37, 0x72, 0x09, 0x10, 0x18, 0x21, 0x29,
	0xf0, 0x09, 0x18, 0x20, 0x22, 0x1a, 0x19, 0x61, 0x13, 0xeb, 0x09, 0x19, 0x1a, 0x22, 0x21, 0x59,
	0x13, 0xeb, 0x09, 0x36, 0x2f, 0x1f, 0x16, 0x15, 0x23, 0x61, 0xe0, 0x09, 0x0d, 0x0b, 0xf3, 0x09,
	0x35, 0x33, 0xcb, 0x09, 0x0a, 0x11, 0x53, 0x0f, 0x70, 0x25, 0x2c, 0x35, 0x26, 0xf6, 0x09, 0x2c,
	0x25, 0x75, 0x34, 0x76, 0x30, 0x4f, 0x0a, 0x11, 0xd3, 0x09, 0x60, 0x22, 0xe7, 0x09, 0x19, 0x0c,
	0x5f, 0x29, 0x1c, 0xef, 0x11, 0x21, 0x34, 0x67, 0x11, 0x24, 0xd7, 0x09, 0x78, 0x58, 0x69, 0x49,
	0x4b, 0x4d, 0x4f, 0x5e, 0x5c, 0x5a, 0x7a, 0x7c, 0x7e, 0x6d, 0x6b, 0xef, 0x09, 0x78, 0x68, 0x58,
	0x48, 0x51, 0x61, 0x41, 0x71, 0x4a, 0x5a, 0x6a, 0x7a, 0x73, 0x63, 0x53, 0x43, 0x4c, 0x5c, 0x6c,
	0x7c, 0x75, 0x65, 0x55, 0x45, 0x4e, 0x47, 0x57, 0x5e, 0x67, 0x6e, 0x77, 0xfe, 0x09, 0x00, 0x78,
	0x02, 0x7a, 0x04, 0x7c, 0x06, 0x7e, 0x00, 0x42, 0x0a, 0x4c, 0x14, 0x56, 0x10, 0x52, 0x1a, 0x5c,
	0x24, 0x66, 0x2e, 0x6f, 0x20, 0x62, 0x2a, 0x6c, 0x34, 0x76, 0x3e, 0x7f, 0x04, 0x46, 0x0e, 0x4f,
	0x1e, 0x5f, 0x3c, 0x7a, 0x32, 0xf0, 0x09, 0x1f, 0xd8, 0x09, 0x04, 0x1c, 0x5f, 0x18, 0xdc, 0x09,
	0x04, 0x1c, 0x5f, 0x03, 0x1b, 0xd8, 0x09, 0x04, 0x1c, 0x5f, 0x27, 0x60, 0x18, 0xdc, 0x09, 0x04,
	0x1c, 0x5f, 0x27, 0xe4, 0x09, 0x04, 0x1c, 0x5f, 0x27, 0x60, 0x03, 0x1b, 0xd8, 0x09, 0x04, 0x1c,
	0x5f, 0x27, 0x60, 0x03, 0x1b, 0xd8, 0x09, 0x27, 0x60, 0x18, 0xdf, 0x09, 0x04, 0x1c, 0x5f, 0x27,
	0x23, 0xc3, 0x09, 0x04, 0x24, 0x60, 0x18, 0x1b, 0xc3, 0x09, 0x03, 0x1b, 0x58, 0x20, 0xe3, 0x09,
	0x1c, 0x44, 0x03, 0x1b, 0xd8, 0x09, 0x04, 0x1c, 0xdf, 0x09, 0x3c, 0x1c, 0xd8, 0x09, 0x3c, 0x44,
	0x1c, 0xd8, 0x09, 0x3c, 0x44, 0x1c, 0xdf, 0x09, 0x3c, 0x5c, 0x18, 0xdf, 0x09, 0x3c, 0xc4, 0x09,
	0x3c, 0x44, 0x18, 0xdf, 0x09, 0x3c, 0x24, 0x67, 0x3b, 0x23, 0xe0, 0x09, 0x3c, 0x64, 0x20, 0x67,
	0x1f, 0xd8, 0x09, 0x3c, 0x1c, 0x58, 0x20, 0x23, 0xfb, 0x09, 0x3c, 0x24, 0x67, 0x1f, 0x1b, 0xfb,
	0x09, 0x3c, 0x44, 0x03, 0x1b, 0x58, 0x20, 0x23, 0xfb, 0x09, 0x3b, 0x43, 0x04, 0x1c, 0x5f, 0x27,
	0x24, 0xfc, 0x09, 0x1f, 0x58, 0x3b, 0x23, 0x60, 0x27, 0x24, 0xfc, 0x09, 0x3c, 0x44, 0x03, 0xfb,
	0x09, 0x27, 0x24, 0x7c, 0x3b, 0x23, 0x60, 0x18, 0x1b, 0x43, 0x04, 0x1c, 0xdf, 0x09, 0x3c, 0x44,
	0x03, 0xfb, 0x09, 0x3c, 0x44, 0x24, 0x60, 0x1c, 0xd8, 0x09, 0x1f, 0x5c, 0x04, 0x7c, 0x3b, 0xc3,
	0x09, 0x1f, 0x5c, 0x27, 0x64, 0x04, 0xfc, 0x09, 0x3b, 0x23, 0x60, 0x18, 0xdb, 0x09, 0x3b, 0x23,
	0x60, 0x18, 0x5b, 0x24, 0xfc, 0x09, 0x3b, 0x63, 0x3c, 0x24, 0xe7, 0x09, 0x3c, 0x24, 0x67, 0x1f,
	0xdc, 0x09, 0x27, 0x60, 0x1f, 0x58, 0x04, 0x5c, 0x24, 0xfc, 0x09, 0x1f, 0x5c, 0x1b, 0x58, 0x03,
	0x7b, 0x04, 0xfc, 0x09, 0x04, 0x1c, 0xd8, 0x09, 0x1f, 0x1c, 0xfc, 0x09, 0x00, 0x07, 0x0f, 0x08,
	0x10, 0x17, 0x1f, 0x18, 0x20, 0x27, 0x2f, 0x28, 0x30, 0x37, 0x3f, 0x78, 0x01, 0x79, 0x3a, 0x42,
	0x03, 0x7b, 0x3c, 0x44, 0x05, 0x7d, 0x3e, 0x46, 0x00, 0x78, 0x3f, 0x07, 0x08, 0x17, 0x18, 0x27,
	0x28, 0x37, 0x78, 0x00, 0x0f, 0x10, 0x1f, 0x20, 0x2f, 0x30, 0x7f, 0x07, 0x3e, 0x05, 0x3c, 0x03,
	0x3a, 0x01, 0x78, 0x00, 0x39, 0x02, 0x3b, 0x04, 0x3d, 0x06, 0xff, 0x09, 0x04, 0x07, 0x0c, 0x0f,
	0x14, 0x17, 0x1c, 0x1f, 0x24, 0x27, 0x2c, 0x2f, 0x34, 0x37, 0x3c, 0x3f, 0x74, 0x0c, 0x57, 0x1c,
	0x67, 0x2c, 0x77, 0x24, 0x6f, 0x14, 0x5f, 0x04, 0x4f, 0x07, 0x7f, 0x3c, 0x44, 0x05, 0x3d, 0x06,
	0x3e, 0x05, 0x7c, 0x3f, 0x46, 0x07, 0x7e, 0x04, 0xfd, 0x09, 0x18, 0x1f, 0x07, 0x00, 0x18, 0x17,
	0x10, 0x0f, 0x08, 0x47, 0x00, 0x4f, 0x08, 0x57, 0x10, 0x5f, 0x06, 0x1e, 0x05, 0x1c, 0x04, 0x1b,
	0x03, 0x1a, 0x02, 0x19, 0x01, 0x58, 0x1f, 0x46, 0x05, 0x5d, 0x07, 0x5e, 0x1d, 0x44, 0x1b, 0x42,
	0x1a, 0x41, 0x19, 0x40, 0x1d, 0xc6, 0x09, 0x18, 0x1f, 0x3f, 0x38, 0x18, 0x39, 0x19, 0x3a, 0x1a,
	0x3b, 0x1b, 0x3c, 0x1c, 0x3d, 0x1d, 0x3e, 0x1e, 0x7f, 0x1f, 0x7e, 0x1e, 0x7d, 0x3c, 0x5d, 0x1c,
	0x7b, 0x3a, 0x5b, 0x1a, 0x79, 0x38, 0x59, 0x1f, 0x20, 0x27, 0x28, 0x2f, 0x30, 0x37, 0x78, 0x3f,
	0x70, 0x37, 0x68, 0x2f, 0x60, 0x27, 0xd8, 0x09, 0x3c, 0x04, 0x00, 0x38, 0x3c, 0x30, 0x34, 0x28,
	0x2c, 0x20, 0x24, 0x18, 0x1c, 0x10, 0x14, 0x08, 0x0c, 0x40, 0x04, 0x48, 0x0c, 0x50, 0x14, 0x58,
	0x1c, 0x60, 0x24, 0x68, 0x2c, 0x70, 0x34, 0x78, 0x04, 0x3b, 0x03, 0x3a, 0x02, 0x39, 0x01, 0x78,
	0x00, 0x79, 0x01, 0x7a, 0x02, 0x7b, 0x03, 0xfc, 0x09, 0x34, 0x2e, 0x27, 0x1f, 0x16, 0x15, 0x1c,
	0x24, 0x2d, 0x2e, 0xf7, 0x09, 0x17, 0x12, 0x19, 0x21, 0x2a, 0x2b, 0x24, 0x5c, 0x24, 0x35, 0x36,
	0x2f, 0xdf, 0x09, 0x17, 0x11, 0xe9, 0x05, 0x0f, 0x17, 0x5c, 0x2c, 0x2f, 0x77, 0x0d, 0x14, 0x2c,
	0xf3, 0x09, 0x31, 0x09, 0x24, 0x0f, 0xf7, 0x09, 0x34, 0x1c, 0x15, 0x16, 0x1f, 0x27, 0x2e, 0x2d,
	0xe4, 0x09, 0x17, 0x53, 0x15, 0x1e, 0x26, 0x2d, 0x6b, 0x2d, 0xf6, 0x09, 0x1f, 0x5b, 0x13, 0xe3,
	0x09, 0x11, 0x0a, 0x0c, 0x15, 0x2d, 0x34, 0x32, 0x29, 0x19, 0xdf, 0x09, 0x29, 0x11, 0x0a, 0x0e,
	0x17, 0x2f, 0x36, 0x32, 0x69, 0x14, 0xec, 0x09, 0x0f, 0x17, 0x0e, 0x0a, 0x11, 0x29, 0x32, 0x36,
	0x2f, 0xf7, 0x09, 0x20, 0x19, 0x22, 0x2a, 0x33, 0x36, 0x2f, 0x17, 0x0e, 0x0b, 0x12, 0xe2, 0x09,
	0x0c, 0x05, 0x06, 0x0f, 0x17, 0x2c, 0x34, 0x3d, 0x3e, 0x37, 0x2f, 0x14, 0xcc, 0x09, 0x1b, 0x14,
	0x15, 0x1e, 0x26, 0x2d, 0x2c, 0x23, 0x5b, 0x22, 0xdf, 0x06, 0x2a, 0x12, 0x0b, 0x14, 0x6c, 0x14,
	0x0d, 0x16, 0xee, 0x09, 0x17, 0x14, 0x1b, 0x23, 0x2c, 0xef, 0x09, 0x0b, 0x73, 0x0c, 0x74, 0x0d,
	0xf5, 0x09, 0x0e, 0x6e, 0x1d, 0x59, 0x0b, 0xeb, 0x09, 0x0e, 0x6e, 0x0d, 0x2b, 0xc9, 0x05, 0x29,
	0x0b, 0x6d, 0x0e, 0xee, 0x09, 0x29, 0x20, 0x18, 0x11, 0xd7, 0x09, 0x10, 0xd7, 0x09, 0x1a, 0x1b,
	0x23, 0x22, 0x5a, 0x0c, 0x74, 0x1d, 0x1e, 0x26, 0x25, 0xdd, 0x09, 0x0e, 0x15, 0x1d, 0x26, 0x2e,
	0x75, 0x0c, 0x13, 0x1b, 0x24, 0x2c, 0xf3, 0x09, 0x18, 0x19, 0x21, 0x20, 0xd8, 0x09, 0x0c, 0x14,
	0x1f, 0xe8, 0x09, 0x10, 0x52, 0x11, 0x18, 0x21, 0xe2, 0x09, 0x11, 0x18, 0x21, 0x12, 0xe2, 0x09,
	0x0a, 0x0f, 0x37, 0x32, 0x0a, 0x17, 0x12, 0x1f, 0x1a, 0x27, 0x22, 0x2f, 0x2a, 0x77, 0x32, 0x6f,
	0x2a, 0x67, 0x22, 0x5f, 0x1a, 0x57, 0x12, 0x4f, 0x0a, 0x33, 0x0b, 0x34, 0x0c, 0x35, 0x0d, 0x36,
	0x0e, 0x77, 0x32, 0x4b, 0x33, 0x4c, 0x34, 0x4d, 0x35, 0x4e, 0x36, 0xcf, };

#endif
