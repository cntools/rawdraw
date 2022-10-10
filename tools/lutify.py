#!/usr/bin/python3

creating_lut = False
ignoring_lines = False
lut_params = []
lut_c_array = ""

LUT_MIN_LINE_LENGTH = 80

line_number = 1
last_ignore_start_line = -1

with open("CNFGKeyRaw.h", "r") as toLUT:
	with open("CNFGKeyLUT.h", "w") as LUTfile:
		for l in toLUT:
			params = l.strip().split(' ')
			if l[0] == "@":
				print(params)
				if params[0] == '@LUT_IGNORE_START':
					ignoring_lines = True
					last_ignore_start_line = line_number
					continue
				elif params[0] == '@LUT_IGNORE_END':
					ignoring_lines = False
					continue
				if not ignoring_lines:
					if params[0] == '@LUT_BEGIN':
						creating_lut = True
						lut_c_array = "const " + params[1] + " " + params[2]
						continue
					elif params[0] == '@LUT_END':
						creating_lut = False
						for i in lut_params:
							if i[0].startswith("0x"):
								i[0] = int(i[0], 16)
							else:
								i[0] = int(i[0])
						lut_params.sort()
						lut_c_array += "[" + str(lut_params[-1][0]+1) + "] = {"
						cur_pos = 0
						for i in lut_params:
							while cur_pos != i[0]:
								lut_c_array += "0, "
								cur_pos += 1
								if (len(lut_c_array) - lut_c_array.rfind("\n")) > LUT_MIN_LINE_LENGTH:
									lut_c_array += "\n"
							if i == lut_params[-1]:
								lut_c_array += i[1] + "};"
							else:
								lut_c_array += f"{i[1]}, "
							cur_pos += 1
							if (len(lut_c_array) - lut_c_array.rfind("\n")) > LUT_MIN_LINE_LENGTH:
								lut_c_array += "\n"
						lut_params = []
						LUTfile.write(lut_c_array)
						lut_c_array = ""
						continue
			if creating_lut:
				lut_params.append(params)
			elif not ignoring_lines:
				LUTfile.write(l)
			line_number += 1
