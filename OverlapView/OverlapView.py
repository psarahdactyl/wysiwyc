#adapted from https://www.cairographics.org/cookbook/svgtopycairo/
'''
    Render the Overlap view for laser cutting
    '''
import os.path
import sys
import re
import xml.etree.ElementTree as et
import cairocffi as cairo
from pyparsing import Literal, Combine, Optional, Word, Group, OneOrMore, nums, ParserElement
from svgpathtools import svg2paths2, wsvg
#from xml.dom.minidom import parse

ParserElement.enablePackrat()

#get file

file = sys.argv[1]
LINEWIDTH = float(sys.argv[2])
XML_FILE = os.path.abspath(__file__)
XML_FILE = os.path.dirname(XML_FILE)
XML_FILE = os.path.join(XML_FILE, file)

def cleanup(XML_FILE):
    '''
        Ready the svg for parsing. DEPRECATED, slow version. Use in conjunction with get_commands(XML_FILE) if the clean_parse encounters errors.
        '''
    name_space = "http://www.w3.org/2000/svg" #The XML namespace.
    et.register_namespace(' ', name_space)
    tree = et.parse(XML_FILE)
    for group in tree.iter('{%s}svg' % name_space):
        for path_check in group.iter('{%s}path' % name_space):
            d_string = path_check.get("d")
            if not 'M' in d_string.upper():
                d_string = 'M 0.0,0.0 L 0.0,0.0'
                path_check.set('d', d_string)
            
            if not ('L' in d_string.upper() or 'C' in d_string.upper()):
                d_string = d_string.replace(', ', ',')
                d_string = d_string.replace(' ,', ',')
                extra = d_string.replace('M', 'L')
                d_string = d_string + ' ' + extra
                path_check.set('d', d_string)
    
        for eachone in group.iter('{%s}polygon' % name_space):
            polyline_d = eachone.get("points")
            points = polyline_d.replace(', ', ',')
            points = points.replace(' ,', ',')
            points = points.split()
            
            if points[0] == points[-1]:
                points = points[:-1]
            points = ' '.join(points)
            eachone.set('points', points)
        
        for f_f in group.iter('{%s}rect' % name_space):
            accum = [f_f.get('x'), f_f.get('y'), f_f.get('width'), f_f.get('height')]
            f_f.tag = '{%s}path' % name_space
            dstring = 'M' + accum[0] + ',' + accum[1]
            dstring += ' L' + accum[0] + ',' + str(float(accum[1]) + float(accum[3]))
            dstring += ' L' + str(float(accum[0]) +float(accum[2])) + ',' + str(float(accum[1]) + float(accum[3]))
            dstring += ' L' + str(float(accum[0]) +float(accum[2])) + ',' + accum[1]
            dstring += ' L' + accum[0] + ',' + accum[1]
            f_f.set('d', dstring)

for el in tree.getiterator():
    match = re.match("^(?:\{.*?\})?(.*)$", el.tag)
    if match:
        el.tag = match.group(1)

    XML_FILE_2 = XML_FILE[:-4] + ('-clean.svg')

tree.write(XML_FILE_2, encoding="utf-8")
pathlist, attlist, svgattrib = svg2paths2(XML_FILE_2)
wsvg(pathlist, attributes=attlist, svg_attributes=svgattrib, filename=XML_FILE_2)
return XML_FILE_2

def clean_parse(XML_FILE):
    name_space = "http://www.w3.org/2000/svg" #The XML namespace.
    et.register_namespace(' ', name_space)
    tree = et.parse(XML_FILE)
    
    path_list = []
    paths = []
    dot = Literal(".")
    comma = Literal(",").suppress()
    floater = Combine(Optional("-") + Word(nums) + Optional(dot) + Optional(Word(nums)))
    couple = floater + Optional(comma) + floater
    M_command = "M" + Group(couple)
    C_command = "C" + Group(couple + Optional(comma) + couple + Optional(comma) + couple)
    L_command = "L" + Group(couple)
    H_command = "H" + Group(floater)
    V_Command = "V" + Group(floater)
    S_Command = "S" + Group(couple + couple)
    E_Command = "E" + Group(floater)
    F_Command = "F" + Group(floater)
    Z_command = "Z"
    svgcommand = M_command | C_command | L_command | E_Command | F_Command | H_command | V_Command | S_Command | Z_command
    phrase = OneOrMore(Group(svgcommand))
    count = 0
    
    for group in tree.iter('{%s}svg' % name_space):
        #line
        for eachone in group.iter('{%s}line' % name_space):
            accum = [eachone.get('x1'), eachone.get('y1'), eachone.get('x2'), eachone.get('y2')]
            accum[:] = [str(float(i)*3.3) for i in accum]
            command = ""
            command += "ctx.move_to(%s,%s);" % (accum[0], accum[1])
            command += "ctx.line_to(%s,%s);" % (accum[2], accum[3])
            path_list.append(command)
        
        #rectangle
        for eachone in group.iter('{%s}rect' % name_space):
            accum = [eachone.get('x'), eachone.get('y'), eachone.get('width'), eachone.get('height')]
            accum[:] = [str(float(i)*3.3) for i in accum]
            command = "ctx.rectangle(%s,%s,%s,%s);" % (accum[0], accum[1], accum[2], accum[3])
            path_list.append(command)
        
        for eachone in group.iter('{%s}path' % name_space):
            phrase_toparse = eachone.get("d")
            if ('M' in phrase_toparse.upper() or 'V' in phrase_toparse.upper() or 'H' in phrase_toparse.upper() or 'C' in phrase_toparse.upper() or 'L' in phrase_toparse.upper()):
                phrase_toparse = phrase_toparse.replace('h','e')
                phrase_toparse = phrase_toparse.replace('v','f')
                tokens = phrase.parseString(phrase_toparse.upper())
                
                cairo_commands = ''
                for i in tokens:
                    command = i[0]
                    if len(i) > 1:
                        clist = i[1]
                        clist[:] = [str(float(i)*3.3) for i in clist]
                        print(clist)
                    if command == "M":
                        cairo_commands += "ctx.move_to(%s,%s);" % (clist[0], clist[1])
                    if command == "C":
                        cairo_commands += "ctx.curve_to(%s,%s,%s,%s,%s,%s);" % (clist[0], clist[1], clist[2], clist[3], clist[4], clist[5])
                    if command == "L":
                        cairo_commands += "ctx.line_to(%s,%s);" % (clist[0], clist[1])
                    if command == "E":
                        cairo_commands += "ctx.line_to(ctx.get_current_point()[0] + %s, ctx.get_current_point()[1]);" % (clist[0])
                    if command == "F":
                        cairo_commands += "ctx.line_to(ctx.get_current_point()[0], ctx.get_current_point()[1] + %s);" % (clist[0])
                    if command == "H":
                        cairo_commands += "ctx.line_to(%s, ctx.get_current_point()[1]);" % (clist[0])
                    if command == "V":
                        cairo_commands += "ctx.line_to(ctx.get_current_point()[0], %s);" % (clist[0])
                    if command == "S":
                        cairo_commands += "ctx.curve_to(ctx.get_current_point()[0], ctx.get_current_point()[1], %s, %s, %s, %s);" % (clist[0], clist[1], clist[2], clist[3])
                    if command == "Z":
                        cairo_commands += "ctx.close_path();"
                #print('He;;o',cairo_commands)
                path_list.append(cairo_commands)

#polygon
for eachone in group.iter('{%s}polygon' % name_space):
    polyline_d = eachone.get('points')
    points = polyline_d.replace(', ', ',')
    points = points.replace(' ,', ',')
    points = points.split()
    new_points = []
    for i in points:
        a,b = i.split(',')
        a = str(float(a)*3.3)
        b = str(float(b)*3.3)
        i = a + ',' + b
            new_points.append(i)
            command = ""
            if(len(new_points) < 4):
                command = "ctx.move_to(0.0,0.0);ctx.line_to(0.0,0.0);"
        else:
            command += "ctx.move_to(%s);" % (new_points[0])
            for i in range(1,len(new_points)):
                command += "ctx.line_to(%s);" % (new_points[i])
                command += "ctx.line_to(%s);" % (new_points[0])
            path_list.append(command)
        
        #polyline
        for eachone in group.iter('{%s}polyline' % name_space):
            polyline_d = eachone.get('points')
            points = polyline_d.replace(', ', ',')
            points = points.replace(' ,', ',')
            points = points.split()
            new_points = []
            for i in points:
                a,b = i.split(',')
                a = str(float(a)*3.3)
                b = str(float(b)*3.3)
                i = a + ',' + b
                new_points.append(i)
            command = ""
            if(len(new_points) < 4):
                command = "ctx.move_to(0.0,0.0);ctx.line_to(0.0,0.0);"
            else:
                command += "ctx.move_to(%s);" % (new_points[0])
                for i in range(1,len(new_points)):
                    command += "ctx.line_to(%s);" % (new_points[i])
                command += "ctx.line_to(%s);" % (new_points[0])
            path_list.append(command)

#print(path_list)
return path_list

def get_commands(XML_FILE_2):
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
    tree = et.parse(XML_FILE_2)
    name_space = "http://www.w3.org/2000/svg" #The XML namespace.
    root = tree.getroot()
    rval2 = [root.get("viewBox"), root.get('width'), root.get('height')]
    for group in tree.iter('{%s}svg' % name_space):
        for eachone in group.iter('{%s}path' % name_space):
            phrase_toparse = eachone.get("d")
            tokens = phrase.parseString(phrase_toparse.upper())
            paths.append(tokens) # paths is a global var.

cairo_commands = ""
    command_list = []
    for tokens in paths:
        for command, couples in tokens: #looks weird, but it works :)
            clist = couples.asList()
            clist[:] = [str(float(i)*3.3) for i in clist]
            if command == "M":
                cairo_commands += "ctx.move_to(%s,%s);" % (clist[0], clist[1])
            if command == "C":
                cairo_commands += "ctx.curve_to(%s,%s,%s,%s,%s,%s);" % (clist[0], clist[1], clist[2], clist[3], clist[4], clist[5])
            if command == "L":
                cairo_commands += "ctx.line_to(%s,%s);" % (clist[0], clist[1])
            if command == "Z":
                cairo_commands += "ctx.close_path();"
    
        command_list.append(cairo_commands) #Add them to the list
    cairo_commands = ""

return command_list, rval2

def render_this(command_list, linewidth):
    img = cairo.ImageSurface(cairo.FORMAT_ARGB32, 1600, 1600)
    ctx = cairo.Context(img)
    ctx.set_antialias(cairo.ANTIALIAS_BEST)
    ctx.set_line_width(LINEWIDTH)
    ctx.set_source_rgba(0.0, 0.00, 0.0, 1.0)
    ctx.paint()
    
    ctx.set_source_rgba(0.03137254902, 0, 0, 1.0)
    ctx.set_operator(cairo.OPERATOR_ADD)
    for c in command_list:
        exec(c)
        ctx.stroke()
    
    img.write_to_png("output.png")
    print("success")
    return True

#COMMAND_LIST = clean_parse(XML_FILE)
XML_FILE_2 = cleanup(XML_FILE)
COMMAND_LIST, ATTRIB = get_commands(XML_FILE_2)
render_this(COMMAND_LIST, LINEWIDTH)
