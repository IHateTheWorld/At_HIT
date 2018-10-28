from wand.image import Image
from PIL import Image as PI
import pyocr
import pyocr.builders
import io
import sys

def ocr_convert(pdf_name):
    tools = pyocr.get_available_tools()
    if len(tools) == 0:
        print("NO OCR tool found")
        sys.exit(1)
    # The tools are returned in the recommended order of usage
    tool = tools[0]
    print("Will use tool '%s'" % (tool.get_name()))

    langs = tool.get_available_languages()
    print("Available languages: %s" % ", ".join(langs))
    lang = langs[0]
    print("Will use lang '%s'" % (lang))

    req_image = []
    final_text = []
    # pdf ==> image
    print('pdf ==> image')
    image_pdf = Image(filename=pdf_name, resolution=300)
    image_jpeg = image_pdf.convert('jpeg')
    for img in image_jpeg.sequence:
        img_page = Image(image=img)
        req_image.append(img_page.make_blob('jpeg'))
    # image ==> text (by orc)
    print("image ==> text (by orc)")
    builder = pyocr.builders.TextBuilder()
    for img in req_image:
        txt = tool.image_to_string(
                PI.open(io.BytesIO(img)),
                lang=lang,
                builder = builder
        )
        final_text.append(txt)
        print txt.encode('utf-8')
    # output the result
    #for txt in final_text:
    #    print txt

    return 

if __name__ == "__main__":
    pdf_name = "./text_ocr.pdf"
    ocr_convert(pdf_name)
