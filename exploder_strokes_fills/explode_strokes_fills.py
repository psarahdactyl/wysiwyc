'''
SVGExploder: Splits the SVG into strokes-only and fills-only
'''
import os, sys
import xml.etree.ElementTree as et
import re

file = sys.argv[1]
LINEWIDTH = float(sys.argv[2])
XML_FILE = os.path.abspath(__file__)
XML_FILE = os.path.dirname(XML_FILE)
XML_FILE = os.path.join(XML_FILE, file)
'''
NAME_SPACE = "http://www.w3.org/2000/svg" #The XML namespace.
et.register_namespace(' ', NAME_SPACE)
tree = et.parse(XML_FILE)
for group in tree.iter('{%s}svg' % NAME_SPACE):
    for child in group.iter('{%s}g' % NAME_SPACE):
        for elementwise in child:
            if elementwise.attrib != {}:
                style_attributes = elementwise.get('style')
                style_attributes = style_attributes.split(';')
                out = ''
                for i in style_attributes:
                    if i[0:4] == 'fill':
                        out += 'fill:none;'
                    else:
                        out += i + ';'
                out = out[:-1]
                elementwise.set('style', out)
                print(elementwise.attrib)

for el in tree.getiterator():
    match = re.match("^(?:\{.*?\})?(.*)$", el.tag)
    if match:
        el.tag = match.group(1)

XML_FILE_2 = XML_FILE[:-4] + ('-strokes.svg')
tree.write(XML_FILE_2, encoding="utf-8")
print('success')
'''

def garbage(XML_FILE):
    NAME_SPACE = "http://www.w3.org/2000/svg" #The XML namespace.
    et.register_namespace(' ', NAME_SPACE)
    tree = et.parse(XML_FILE)
    for group in tree.iter('{%s}svg' % NAME_SPACE):
        for child in group.iter('{%s}g' % NAME_SPACE):
            for elementwise in child:
                if elementwise.attrib != {}:
                    style_attributes = elementwise.get('style')
                    style_attributes = style_attributes.split(';')
                    out = ''
                    for i in style_attributes:
                        if i[0:12] == 'stroke-width':
                            out += 'stroke-width:0.00;'
                        else:
                            out += i + ';'
                    out = out[:-1]
                    elementwise.set('style', out)
                    print(elementwise.attrib)

    for el in tree.getiterator():
        match = re.match("^(?:\{.*?\})?(.*)$", el.tag)
        if match:
            el.tag = match.group(1)

    XML_FILE_2 = XML_FILE[:-4] + ('-fills.svg')
    tree.write(XML_FILE_2, encoding="utf-8")
    print('success')

garbage(XML_FILE)