NUM_SAMPLES = 1024
TWO_POW_RANGE = (-10, 10)
SIN_RANGE = (0, 1) # 0 to 1 is one phase meaning 0 to 2pi radians

def generate_two_pow_lut():
    lut = []
    for i in range(NUM_SAMPLES):
        x = i / (NUM_SAMPLES - 1)  # Normalize to [0, 1]
        exp_value = TWO_POW_RANGE[0] + x * (TWO_POW_RANGE[1] - TWO_POW_RANGE[0])
        lut.append(2 ** exp_value)
    return lut

def generate_sin_lut():
    import math
    lut = []
    for i in range(NUM_SAMPLES):
        x = i / (NUM_SAMPLES - 1)  # Normalize to [0, 1]
        angle = SIN_RANGE[0] + x * (SIN_RANGE[1] - SIN_RANGE[0])  # Map to [0, 1]
        lut.append(math.sin(angle * 2 * math.pi))  # Convert to radians
    return lut

def generate_sample_function(): # Generate C functions that will actually do the lookup
    two_pow_lut = generate_two_pow_lut()
    sin_lut = generate_sin_lut()

    def generate_lookup_function(lut, function_name, range):
        function_code = f"float {function_name}(float x) {{\n"
        function_code += f"    // x should be in the range [{range[0]}, {range[1]}]\n" 
        function_code += f"    int index = (int)((x - {range[0]}) / ({range[1]} - {range[0]}) * ({NUM_SAMPLES} - 1));\n"
        function_code += f"    if (index < 0) index = 0;\n"
        function_code += f"    if (index >= {NUM_SAMPLES}) index = {NUM_SAMPLES} - 1;\n"
        function_code += f"    static const float lut[{NUM_SAMPLES}] = {{\n"
        for value in lut:
            function_code += f"        {value}f,\n"
        function_code += f"    }};\n"
        function_code += f"    return lut[index];\n"
        function_code += f"}}\n"
        return function_code
    two_pow_function = generate_lookup_function(two_pow_lut, "lookup_two_pow", TWO_POW_RANGE)
    sin_function = generate_lookup_function(sin_lut, "lookup_sin", SIN_RANGE)
    return two_pow_function + "\n" + sin_function

if __name__ == "__main__":
    c_code = generate_sample_function()
    print(c_code)