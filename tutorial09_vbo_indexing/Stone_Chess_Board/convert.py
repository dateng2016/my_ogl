# from PIL import Image

# # Open the JPG file
# img = Image.open("12951_Stone_Chess_Board_diff.jpg")

# # Save as BMP
# img.save("12951_Stone_Chess_Board_diff.bmp")


from PIL import Image
import pydds


def convert_jpg_to_dds(jpg_file, dds_file):
    # Open the JPG image
    with Image.open(jpg_file) as img:
        # Convert the image to RGBA (if not already in that mode)
        img = img.convert("RGBA")

        # Save as DDS using the pydds library
        pydds.save(dds_file, img.tobytes(), img.size[0], img.size[1])


# Example usage
convert_jpg_to_dds(
    "12951_Stone_Chess_Board_diff.jpg", "12951_Stone_Chess_Board_diff.dds"
)
