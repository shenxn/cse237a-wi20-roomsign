import numpy as np
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont
from mako.template import Template
from templating_const import *

template = Template(filename='templates/font.cpp.tpl')

def templatingFont(fontSize, width):
    appendSize = 8 - width % 8
    if appendSize == 8:
        appendSize = 0
    append = np.zeros((fontSize, appendSize), dtype=np.bool)
    dataStr = ''
    for i in range(32, 127):
        img = Image.new('1', (width, fontSize))
        draw = ImageDraw.Draw(img)
        fnt = ImageFont.truetype(fontPath, fontSize)
        draw.text((0, 0), str(chr(i)), font=fnt, fill=uncolored)
        imgData = np.hstack((np.array(img), append))
        for i in range(fontSize):
            for j in range(0, width, 8):
                v = 0
                for k in range(8):
                    v = v * 2 + imgData[i, j + k]
                dataStr += hex(v) + ','
    content = template.render(
        fontData=dataStr,
        fontSize=fontSize,
        width=width
    )
    with open('%s/font%d.cpp' % (outputPathPrefix, fontSize), 'w') as f:
        print(content, file=f)

templatingFont(8, 4)
templatingFont(12, 6)
templatingFont(16, 8)
templatingFont(20, 10)
templatingFont(24, 12)
