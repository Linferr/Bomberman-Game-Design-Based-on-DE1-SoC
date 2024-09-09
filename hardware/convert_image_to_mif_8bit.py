from PIL import Image


def rgb_image_to_mif(image_file, mif_file, width, height):
    image = Image.open(image_file).convert("RGB")
    image = image.resize((width, height))

    with open(mif_file, 'w') as mif:
        mif.write(f"DEPTH = {width * height};\n")
        mif.write("WIDTH = 8;\n")
        mif.write("ADDRESS_RADIX = UNS;\n")
        mif.write("DATA_RADIX = BIN;\n")
        mif.write("CONTENT\n")
        mif.write("BEGIN\n")

        address = 0
        for y in range(height):
            for x in range(width):
                r, g, b = image.getpixel((x, y))

                r = round(r/36)  # e.g. 255/32=7 = 111
                g = round(g/36)
                b = round(b/85)

                mif.write(f"{address}: {format(r, '03b')}{format(g, '03b')}{format(b, '02b')};\n")
                address += 1
        mif.write("END;\n")


# Usage example:
input_image = "char_y.png"
output_mif = "char_y.mif"
width = 29  # Desired width
height = 29  # Desired height
rgb_image_to_mif(input_image, output_mif, width, height)
