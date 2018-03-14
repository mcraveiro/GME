import os
import os.path
import sys

from xml.etree import ElementTree


def indent(elem, level=0, parent=None):
    i = "\n" + level * "  "
    j = "\n" + (level - 1) * "  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for subelem in elem:
            indent(subelem, level + 1, elem)
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            if parent is not None and elem == parent[-1]:
                elem.tail = j
            else:
                elem.tail = i
    return elem


def id2guid(input, output, sort_elements=False, omit_default_attributes=False, sort_before_guids=None, sort_after_guids=None):
    from xml.etree import ElementTree
    xme = ElementTree.parse(input)
    id_guid_map = {}
    for element in xme.iter():
        if element.get('id') and element.get('guid'):
            id_guid_map[element.get('id')] = element.get('guid')
            del element.attrib['id']
        elif element.get('guid'):
            id_guid_map[element.get('guid')] = element.get('guid')

        if element.get('childrelidcntr'):
            del element.attrib['childrelidcntr']
    for element in xme.iter():
        if element.get('derivedfrom'):
            element.attrib['derivedfrom'] = id_guid_map[element.get('derivedfrom')]
    for element in xme.iter('reference'):
        if element.get('referred'):
            element.attrib['referred'] = id_guid_map[element.get('referred')]
    for element in xme.iter('set'):
        if element.get('members'):
            element.attrib['members'] = " ".join(sorted([id_guid_map[id] for id in element.get('members').split()]))
    for element in xme.iter('connpoint'):
        if element.get('target'):
            element.attrib['target'] = id_guid_map[element.get('target')]
        if element.get('refs'):
            element.attrib['refs'] = " ".join([id_guid_map[id] for id in element.get('refs').split()])

    if omit_default_attributes:
        parent_map = dict((c, p) for p in xme.getiterator() for c in p)
        for element in xme.findall('.//attribute[@status=\'meta\']'):
            parent_map[element].remove(element)

    if sort_elements:
        if sort_before_guids:
            before_guids = set((g.strip() for g in open(sort_before_guids).readlines()))
        if sort_after_guids:
            after_guids = set((g.strip() for g in open(sort_after_guids).readlines()))

        def sortchildrenby(parent, attr, reverse=False):
            parent[:] = sorted(parent, key=lambda child: child.get(attr), reverse=reverse)

        for element in list(xme.iter()):
            sortchildrenby(element, 'name')
            sortchildrenby(element, 'guid')
            # sortchildrenby(element, 'role')
            # sortchildrenby(element, 'relid')

            def sort_guids(guids, reverse):
                def cmp_e(a, b):
                    if a.get('guid') and b.get('guid'):
                        return cmp(a.get('guid') in guids, b.get('guid') in guids)
                    return 0
                element[:] = sorted(element, cmp=cmp_e, reverse=reverse)

            if sort_before_guids:
                sort_guids(before_guids, reverse=False)
            if sort_after_guids:
                sort_guids(after_guids, reverse=True)

            def get_name(e):
                name = e.find('./name')
                return name.text if name is not None else ''

            # element[:] = sorted(element, key=get_name)

    # remove all regnode data (useful for diffs):
    # for element in xme.iter('regnode'):
    #    element[:] = []
    output.write('<!DOCTYPE project SYSTEM "mga2.dtd">\n')
    indent(xme.getroot())
    xme.write(output)


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description='Convert GME id-style .xme (mga.dtd) to GUID-style .xme (mga2.dtd).')
    parser.add_argument('--sort', action='store_true')
    parser.add_argument('--omit-default-attributes', action='store_true')
    parser.add_argument('--sort-after-guids')
    parser.add_argument('--sort-before-guids')
    parser.add_argument('input-xme-file')
    # parser.add_argument('--output-file')

    args = parser.parse_args()
    input = vars(args)['input-xme-file']
    options = {
        "sort_elements": args.sort,
        "omit_default_attributes": 'omit_default_attributes' in vars(args),
        "sort_before_guids": args.sort_before_guids,
        "sort_after_guids": args.sort_after_guids
    }
    if input != '-':
        output_filename = os.path.splitext(input)[0] + "_guids.xme"
        with open(output_filename, 'wb') as output:
            id2guid(input, output, **options)
    else:
        id2guid(sys.stdin, sys.stdout, **options)
