combined_f = open("combined_all.mif", 'w')

DEPTH = 0

#write headers
combined_f.writelines(["DEPTH = REPLACE THIS;\n", "WIDTH = 8;\n", "ADDRESS_RADIX = UNS;\n", "DATA_RADIX = BIN;\n", "CONTENT\n", "BEGIN\n"])

def extract_mif(fname, DEPTH):
    curr_mif = open(fname, 'r')

    #discard headers
    for i in range(0, 6):
        curr_mif.readline()

    for line in curr_mif.readlines():
        if "END;" in line:
            return DEPTH
        parts = line.split(':')
        combined_f.write(f"{DEPTH}: {parts[1].strip()}\n")
        DEPTH += 1
    
    return DEPTH

### .mif files here ###
DEPTH = extract_mif("black_29x29.mif", DEPTH) #0
DEPTH = extract_mif("char_y.mif", DEPTH) #1
DEPTH = extract_mif("char_pink.mif", DEPTH) #2
DEPTH = extract_mif("char_g.mif", DEPTH) #3
DEPTH = extract_mif("char_b.mif", DEPTH) #4

DEPTH = extract_mif("bomb29.mif", DEPTH) #5
DEPTH = extract_mif("bomb29_v.mif", DEPTH) #6
DEPTH = extract_mif("bomb29_h.mif", DEPTH) #7
DEPTH = extract_mif("explosion_29x29.mif", DEPTH) #8

DEPTH = extract_mif("heart20x20.mif", DEPTH) #9
DEPTH = extract_mif("black_20x20.mif", DEPTH) #10
### ^^^ ###

combined_f.write("END;\n")

print(f"Final DEPTH: {DEPTH}\n Edit this into the .mif file.")