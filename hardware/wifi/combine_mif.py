combined_f = open("combined.mif", 'w')

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
mif_files = ["black_29x29.mif", "char_yellow.mif", "test.mif"]
### ^^^ ###

for mif in mif_files:
    DEPTH = extract_mif(mif, DEPTH)

combined_f.write("END;\n")

print(f"Final DEPTH: {DEPTH}\n Edit this into the .mif file.")