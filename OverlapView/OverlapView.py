#adapted from https://www.cairographics.org/cookbook/svgtopycairo/
import cairocffi as cairo
from pyparsing import *
import os, sys
import xml.etree.ElementTree as et
import svgpathtools as sp
from xml.dom.minidom import parse
import re

#get file

file = sys.argv[1]
linewidth = float(sys.argv[2])


xml_file = os.path.abspath(__file__)
xml_file = os.path.dirname(xml_file)
xml_file = os.path.join(xml_file, file)

def cleanup(xml_file):
    ns = "http://www.w3.org/2000/svg" #The XML namespace.
    et.register_namespace(' ', ns)
    tree = et.parse(xml_file)
    for group in tree.iter('{%s}svg' % ns):
        for y in group.iter('{%s}path' % ns):
            x = y.get("d")
            if not 'M' in x.upper():
                x = 'M 0.0,0.0 L 0.0,0.0'
                y.set('d',x)
            
            if not ('L' in x.upper() or 'C' in x.upper()):
                x = x.replace(', ', ',')
                x = x.replace(' ,', ',')
                z = x.replace('M', 'L')
                x = x + ' ' + z
                y.set('d', x)
    
        
        for e in group.iter('{%s}polygon' % ns):
            polyline_d = e.get("points")
            points = polyline_d.replace(', ', ',')
            points = points.replace(' ,', ',')
            points = points.split()
            
            if points[0] == points[-1]:
                points = points[:-1]
            points = ' '.join(points)
            e.set('points', points)
        
        for f in group.iter('{%s}rect' % ns):
            accum = [f.get('x'), f.get('y'), f.get('width'), f.get('height')]
            f.tag = '{%s}path' % ns
            dstring = 'M' + accum[0] + ',' + accum[1]
            dstring += ' L' + accum[0] + ',' + str(float(accum[1]) + float(accum[3]))
            dstring += ' L' + str(float(accum[0]) +float(accum[2])) + ',' + str(float(accum[1]) + float(accum[3]))
            dstring += ' L' + str(float(accum[0]) +float(accum[2])) + ',' + accum[1]
            dstring += ' L' + accum[0] + ',' + accum[1]
            f.set('d', dstring)


for el in tree.getiterator():
    match = re.match("^(?:\{.*?\})?(.*)$", el.tag)
    if match:
        el.tag = match.group(1)
    
    xml_file_2 = xml_file[:-4] + ('-clean.svg')
    tree.write(xml_file_2, encoding = "utf-8")
    pathlist, attlist, svgattrib = sp.svg2paths2(xml_file_2)
    sp.wsvg(pathlist, attributes = attlist, svg_attributes = svgattrib, filename = xml_file_2)
    return xml_file_2

def get_commands(xml_file_2):
    #parsing commands
    dot = Literal(".")
    comma = Literal(",").suppress()
    floater = Combine(Optional("-") + Word(nums) + dot + Word(nums))
    couple = floater + comma + floater
    M_command = "M" + Group(couple)
    C_command = "C" + Group(couple + couple + couple)
    L_command = "L" + Group(couple)
    Z_command = "Z"
    svgcommand = M_command | C_command | L_command | Z_command
    phrase = OneOrMore(Group(svgcommand))
    
    #parse using pyparsing
    paths = []
    tree = et.parse(xml_file_2)
    ns = "http://www.w3.org/2000/svg" #The XML namespace.
    root = tree.getroot()
    rval2 = [root.get("viewBox"), root.get('width'), root.get('height')]
    for group in tree.iter('{%s}svg' % ns):
        for e in group.iter('{%s}path' % ns):
            p = e.get("d")
            tokens = phrase.parseString(p.upper())
            paths.append(tokens) # paths is a global var.
    
    cairo_commands = ""
    command_list = []
    for tokens in paths:
        for command,couples in tokens: #looks weird, but it works :)
            c = couples.asList()
            c[:] = [str(float(i)*3.3) for i in c]
            if command == "M":
                cairo_commands += "ctx.move_to(%s,%s);" % (c[0],c[1])
            if command == "C":
                cairo_commands += "ctx.curve_to(%s,%s,%s,%s,%s,%s);" % (c[0],c[1],c[2],c[3],c[4],c[5])
            if command == "L":
                cairo_commands += "ctx.line_to(%s,%s);" % (c[0],c[1])
            if command == "Z":
                cairo_commands += "ctx.close_path();"

command_list.append(cairo_commands) #Add them to the list
cairo_commands = ""
    
    return command_list, rval2

def renderThis(command_list, attrib, linewidth):
    '''
        accum = ''
        for i in range(len(attrib[1])):
        if attrib[1][i] in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']:
        accum += attrib[1][i]
        width = int(accum)
        
        accum = ''
        for i in range(len(attrib[2])):
        if attrib[2][i] in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']:
        accum += attrib[2][i]
        height = int(accum)
        
        #img = cairo.ImageSurface(cairo.FORMAT_ARGB32, 2524, 2560)
        '''
    img = cairo.ImageSurface(cairo.FORMAT_ARGB32, 1600, 1600)
    #img = cairo.ImageSurface.create_from_png('/Users/arjunchhabra/Desktop/BirchBirchBirch.png')
    ctx = cairo.Context(img)
    ctx.set_antialias(cairo.ANTIALIAS_BEST)
    ctx.set_line_width(linewidth)
    ctx.set_source_rgba(0.0, 0.00, 0.0, 1.0)
    ctx.paint()
    
    ctx.set_source_rgba(0.03137254902 ,0,0, 1.0)
    ctx.set_operator(cairo.OPERATOR_ADD)
    for c in command_list:
        exec(c)
        ctx.stroke()
    
    img.write_to_png("output.png")
    print("success")
    return True


xml_file_2 = cleanup(xml_file)
print("done cleanup")
command_list, attrib = get_commands(xml_file_2)
print("done get_commands")
renderThis(command_list, attrib, linewidth)
