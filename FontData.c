#ifndef FONTINIT
#define FONTINIT
int CharIndex[] = {
	   0,    1,   12,   23,   38,   46,   72,   88,  107,    0,    0,  136,  150,    0,  164,  184,
	 217,  221,  225,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,  234,    0,    0,    0,    0,  242,    0,    0,    0,  246,    0,    0,  250,    0,
	 255,  265,    0,    0,    0,  270,  280,    0,    0,    0,  292,    0,    0,  302,  306,    0,
	   0,    0,  309,  322,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  330,    0,
	   0,    0,    0,    0,  334,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	   0,  338,  349,  359,  367,  377,  388,  395,  409,  416,  420,  425,  431,  435,  446,  453,
	 464,  473,  485,    0,  491,  497,  504,  508,  519,  523,  529,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	};

unsigned char FontData[] = {
	0x80, 0x0c, 0x0d, 0x16, 0x2e, 0x35, 0x34, 0x4c, 0x11, 0x53, 0x29, 0xeb, 0x11, 0x53, 0x29, 0x6b,
	0x14, 0x2c, 0x35, 0x36, 0x0e, 0x0d, 0xd4, 0x27, 0x0d, 0x03, 0x01, 0x08, 0x10, 0x18, 0x61, 0x00,
	0x20, 0x30, 0x39, 0x3b, 0x35, 0xdf, 0x27, 0x4b, 0x0c, 0x60, 0x18, 0x74, 0x33, 0xdf, 0x1b, 0x12,
	0x11, 0x18, 0x20, 0x29, 0x2a, 0x63, 0x2a, 0x32, 0x3b, 0x3c, 0x35, 0x2d, 0x64, 0x1c, 0x15, 0x0d,
	0x04, 0x03, 0x0a, 0x52, 0x1c, 0x1f, 0x27, 0xe4, 0x1f, 0x1d, 0x16, 0x0e, 0x05, 0x04, 0x0b, 0x60,
	0x18, 0x3c, 0x3d, 0x36, 0x2e, 0x25, 0x27, 0xdf, 0x00, 0x08, 0x11, 0x12, 0x0b, 0x14, 0x15, 0x0e,
	0x06, 0x40, 0x28, 0x18, 0x1e, 0x6e, 0x23, 0x5b, 0x30, 0x36, 0xfe, 0x00, 0x02, 0x0a, 0x09, 0x41,
	0x00, 0x08, 0x49, 0x12, 0x10, 0x18, 0x5a, 0x11, 0x59, 0x28, 0x20, 0x22, 0x6a, 0x30, 0x72, 0x31,
	0x78, 0x31, 0x7a, 0x13, 0x05, 0x57, 0x05, 0xfd, 0x20, 0x38, 0x7b, 0x1f, 0x17, 0x05, 0x04, 0x12,
	0x1a, 0x2c, 0x2d, 0x5f, 0x23, 0xf8, 0x20, 0x10, 0x09, 0x0b, 0x14, 0x24, 0x2b, 0x29, 0x60, 0x1c,
	0x1f, 0x4d, 0x1f, 0xed, 0x38, 0x59, 0x39, 0x5a, 0x19, 0x1e, 0x15, 0x0d, 0x06, 0x0f, 0x17, 0x5e,
	0x3e, 0x35, 0x2d, 0x26, 0x2f, 0x37, 0x3e, 0xf8, 0x22, 0x1a, 0x13, 0x14, 0x1d, 0x25, 0x2c, 0x2b,
	0x62, 0x38, 0x6a, 0x3f, 0x6d, 0x07, 0x55, 0x12, 0x40, 0x01, 0x4a, 0x30, 0x69, 0x35, 0x7e, 0x16,
	0x4f, 0x0d, 0x46, 0x11, 0x48, 0x32, 0x79, 0x37, 0xee, 0x07, 0x3c, 0x01, 0xc7, 0x39, 0x3f, 0x04,
	0xf9, 0x18, 0x42, 0x18, 0x72, 0x18, 0x5f, 0x05, 0x1f, 0xf5, 0x10, 0x12, 0x18, 0x50, 0x20, 0x22,
	0x28, 0xe0, 0x18, 0x1a, 0x20, 0xd8, 0x19, 0x5f, 0x04, 0xf4, 0x1f, 0x1e, 0x26, 0x27, 0xdf, 0x17,
	0x0e, 0x09, 0x10, 0x28, 0x31, 0x36, 0x2f, 0x17, 0xe8, 0x0b, 0x20, 0x27, 0x0f, 0xf7, 0x30, 0x10,
	0x09, 0x0b, 0x2b, 0x34, 0x36, 0x2f, 0x17, 0xce, 0x31, 0x28, 0x10, 0x09, 0x0e, 0x17, 0x2f, 0x36,
	0x34, 0x2b, 0x13, 0xcc, 0x1f, 0x1e, 0x26, 0x27, 0x5f, 0x18, 0x19, 0x21, 0x20, 0xd8, 0x32, 0x4a,
	0x0d, 0xf5, 0x17, 0x2d, 0xd3, 0x08, 0x0f, 0x2f, 0x36, 0x34, 0x2b, 0x4b, 0x08, 0x20, 0x28, 0x31,
	0x32, 0xeb, 0x31, 0x28, 0x10, 0x09, 0x0e, 0x17, 0x2f, 0xf6, 0x0f, 0x08, 0x37, 0xf0, 0x1f, 0x60,
	0x00, 0xf8, 0x32, 0x36, 0x7f, 0x33, 0x2a, 0x12, 0x0b, 0x0e, 0x17, 0x2f, 0xf6, 0x09, 0x4f, 0x0e,
	0x17, 0x2f, 0x36, 0x34, 0x2b, 0x13, 0xcc, 0x33, 0x2a, 0x12, 0x0b, 0x0e, 0x17, 0x2f, 0xf6, 0x31,
	0x77, 0x36, 0x2f, 0x17, 0x0e, 0x0c, 0x13, 0x2b, 0xf4, 0x37, 0x17, 0x0e, 0x0b, 0x12, 0x2a, 0x33,
	0x34, 0x2d, 0x15, 0xcc, 0x32, 0x29, 0x19, 0x12, 0x57, 0x0c, 0xec, 0x32, 0x29, 0x19, 0x12, 0x13,
	0x1c, 0x2c, 0x33, 0x72, 0x33, 0x36, 0x2f, 0x1f, 0xd6, 0x08, 0x4f, 0x0b, 0x12, 0x2a, 0x33, 0xf7,
	0x27, 0x1e, 0x5a, 0xd9, 0x61, 0x22, 0x26, 0x1f, 0xd6, 0x08, 0x4f, 0x0c, 0x69, 0x0c, 0xf7, 0x18,
	0x1e, 0x27, 0xef, 0x0f, 0x4a, 0x0c, 0x13, 0x1b, 0x24, 0x67, 0x24, 0x2b, 0x34, 0xf7, 0x0b, 0x4f,
	0x0c, 0x13, 0x2b, 0x34, 0xf7, 0x12, 0x0b, 0x0e, 0x17, 0x2f, 0x36, 0x33, 0x2a, 0x52, 0x32, 0xf3,
	0x0f, 0x0a, 0x11, 0x29, 0x32, 0x33, 0x2c, 0x14, 0xcb, 0x37, 0x33, 0x2a, 0x1a, 0x13, 0x14, 0x1d,
	0x75, 0x32, 0x73, 0x36, 0xee, 0x0a, 0x4f, 0x0b, 0x12, 0x2a, 0xf3, 0x0a, 0x72, 0x1a, 0x1e, 0x27,
	0xef, 0x0a, 0x0e, 0x17, 0x2f, 0x76, 0x32, 0xf7, 0x0a, 0x1f, 0x27, 0xf2, 0x0a, 0x0d, 0x0e, 0x17,
	0x1f, 0x5b, 0x1f, 0x2f, 0x6a, 0x2e, 0xf7, 0x32, 0x4f, 0x0a, 0xf7, 0x12, 0x14, 0x65, 0x2a, 0x2c,
	0xd7, 0x0a, 0x32, 0x0f, 0x77, 0x14, 0xf4, };

#endif
