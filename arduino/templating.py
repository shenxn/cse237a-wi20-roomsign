import numpy as np
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont
from mako.template import Template

width = 296
height = 128
padding = 16
colored = 0
uncolored = 1

fontPath = 'UbuntuMono-R.ttf'

roomName = "ROOM B210"
roomNameHeight = 24
roomNameFontSize = 20

statusInUse = "IN USE"
statusAvailable = "AVAILABLE"
statusWidth = 96
statusFontSize = 22

detailPadding = 16
detailLineHeight = 24
detailFontSize = 16

def genImageData(available):
    img = Image.new('1', (width, height), color=uncolored)
    draw = ImageDraw.Draw(img)

    # room name
    draw.rectangle([0, 0, width, roomNameHeight], fill=colored)
    fnt = ImageFont.truetype(fontPath, roomNameFontSize)
    y = (roomNameHeight - roomNameFontSize) / 2
    draw.text((padding, y), roomName, font=fnt, fill=uncolored)

    if available:
        fnt = ImageFont.truetype(fontPath, statusFontSize)
        y = (height + roomNameHeight - statusFontSize) / 2
        draw.text((padding, y), statusAvailable, font=fnt, fill=colored)
    else:
        # status
        draw.rectangle([0, roomNameHeight, statusWidth, height], fill=colored)
        fnt = ImageFont.truetype(fontPath, statusFontSize)
        y = (height + roomNameHeight - statusFontSize) / 2
        draw.text((padding, y), statusInUse, font=fnt, fill=uncolored)

        # detail
        fnt = ImageFont.truetype(fontPath, detailFontSize)
        x = statusWidth + detailPadding
        y = (detailLineHeight - detailFontSize) / 2 + roomNameHeight + detailPadding
        draw.text((x, y), 'BY   Xiaonan S', font=fnt)
        y += detailLineHeight
        draw.text((x, y), 'Time 11:25am-12:44pm', font=fnt)

    imgData = np.array(img)
    imgDataStr = ''
    for i in range(width):
        for j in range(0, height, 8):
            v = 0
            for k in range(8):
                v = v * 2 + imgData[height - j - k - 1][i]
            imgDataStr += hex(v) + ','
    return imgDataStr

# imgdata.cpp
template = Template(filename='templates/imagedata.cpp.tpl')
content = template.render(
    imgDataInUse=genImageData(False),
    imgDataAvailable=genImageData(True)
)
with open('epaper/imagedata.cpp', 'w') as f:
    print(content, file=f)
