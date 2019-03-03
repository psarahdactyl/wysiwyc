'''
    SVGExploder: Splits the SVG into strokes-only and fills-only
    '''
import os, sys
import xml.etree.ElementTree as et
import re
import copy

file = sys.argv[1]
OUPUT_XML_FILE = os.path.dirname(os.path.abspath(__file__))
OUTPUT_XML_FILE, script = os.path.split(OUPUT_XML_FILE)
path, filename = os.path.split(file)
OUTPUT_XML_FILE = os.path.join(OUPUT_XML_FILE, filename)

INPUT_XML_FILE = os.getcwd()
INPUT_XML_FILE = os.path.join(INPUT_XML_FILE, file)

def duplicate(XML_FILE):
    NAME_SPACE = "http://www.w3.org/2000/svg" #The XML namespace.
    et.register_namespace('', NAME_SPACE)
    tree = et.parse(INPUT_XML_FILE)
    root = tree.getroot()
    for el in tree.getiterator():
        no_fill = copy.deepcopy(el)
        no_stroke = copy.deepcopy(el)
        if el.tag[-3:] != 'svg':
            if 'style' in el.attrib:
                parent = root.findall(".//{0}/..".format(el.tag))
                style_nofill = no_fill.get('style')
                style_nostroke = no_stroke.get('style')
                if no_stroke.get('done') == None:
                    style_nostroke = style_nostroke.split(';')
                    out = ''
                    found = False
                    for i in style_nostroke:
                        if i[0:12] == 'stroke-width':
                            out += 'stroke-width:0.00;'
                            found = True
                        else:
                            out += i + ';'
                    out = out[:-1]
                    if found == False:
                        out += 'stroke-width:0.00;'
                    no_stroke.set('style', out)
                    no_stroke.set('done', 'True')
                
                if no_fill.get('done') == None:
                    style_nofill = style_nofill.split(';')
                    out = ''
                    found = False
                    for i in style_nofill:
                        if i[0:4] == 'fill':
                            out += 'fill:none;'
                            found = True
                        else:
                            out += i + ';'
                    out = out[:-1]
                    if found == False:
                        out += 'fill:none;'
                    no_fill.set('style', out)
                    no_fill.set('done', 'True')
                el.attrib = no_stroke.attrib
                et.SubElement(parent[0], el.tag, attrib = no_fill.attrib)
    
    ''' #Removed to fix browser issue since it takes the ns out
    for el in tree.getiterator():
        match = re.match("^(?:\{.*?\})?(.*)$", el.tag)
        if match:
            el.tag = match.group(1)
    '''

    XML_FILE_2 = XML_FILE[:-4] + ('-exploded.svg')
    tree.write(XML_FILE_2, encoding="utf-8", xml_declaration = True, method = 'xml')
    print('success')

#Only the strokes, if ever needed
def stroke(XML_FILE):
    NAME_SPACE = "http://www.w3.org/2000/svg" #The XML namespace.
    et.register_namespace(' ', NAME_SPACE)
    tree = et.parse(INPUT_XML_FILE)
    for group in tree.iter('{%s}svg' % NAME_SPACE):
        for child in group.iter('{%s}g' % NAME_SPACE):
            print(child.tag)
            for elementwise in child:
                if 'style' in elementwise.attrib:
                    style_attributes = elementwise.get('style')
                    style_attributes = style_attributes.split(';')
                    out = ''
                    found = False
                    for i in style_attributes:
                        if i[0:4] == 'fill':
                            out += 'fill:none;'
                            found = True
                        else:
                            out += i + ';'
                    out = out[:-1]
                    if found == False:
                        out += 'fill:none;'
                    elementwise.set('style', out)
                
                else:
                    out = 'fill:none;'
                    elementwise.set('style', out)

    for el in tree.getiterator():
        match = re.match("^(?:\{.*?\})?(.*)$", el.tag)
        if match:
            el.tag = match.group(1)
        
    XML_FILE_2 = OUTPUT_XML_FILE[:-4] + ('-strokes.svg')
    tree.write(XML_FILE_2, encoding="utf-8")
    print('success')

#Only the fills, if ever needed
def fill(XML_FILE):
    NAME_SPACE = "http://www.w3.org/2000/svg" #The XML namespace.
    et.register_namespace(' ', NAME_SPACE)
    tree = et.parse(INPUT_XML_FILE)
    for group in tree.iter('{%s}svg' % NAME_SPACE):
        for child in group.iter('{%s}g' % NAME_SPACE):
            for elementwise in child:
                if 'style' in elementwise.attrib:
                    style_attributes = elementwise.get('style')
                    style_attributes = style_attributes.split(';')
                    out = ''
                    found = False
                    for i in style_attributes:
                        if i[0:12] == 'stroke-width':
                            out += 'stroke-width:0.00;'
                            found = True
                        else:
                            out += i + ';'
                    out = out[:-1]
                    if found == False:
                        out += 'stroke-width:0.00;'
                    elementwise.set('style', out)
                else:
                    out = 'stroke-width:0.00;'
                    elementwise.set('style', out)

    for el in tree.getiterator():
        match = re.match("^(?:\{.*?\})?(.*)$", el.tag)
        if match:
            el.tag = match.group(1)
        
    XML_FILE_2 = OUTPUT_XML_FILE[:-4] + ('-fills.svg')
    tree.write(XML_FILE_2, encoding="utf-8")
    print('success')

duplicate(INPUT_XML_FILE)

