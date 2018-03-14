import os
import os.path
import sys
import uuid

def fix_dups(filename, output_filename):
    from xml.etree import ElementTree
    xme = ElementTree.parse(filename)
    guid_map = {}
    for element in xme.iter():
        guid = element.get('guid')
        if guid:
            if guid_map.get(guid):
                guid = element.attrib['guid'] = '{' + str(uuid.uuid4()) + '}'
            guid_map[guid] = 1
    
    with open(output_filename, 'wb') as output:
        output.write('<!DOCTYPE project SYSTEM "mga.dtd">\n')
        xme.write(output)

if __name__ == '__main__':
    input = sys.argv[1]
    output = os.path.splitext(input)[0] + "_fixed.xme"
    fix_dups(input, output)
