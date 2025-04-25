import sys
import re

def read_file_from_command_line(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()

    list_of_contents = [""]
    for i in range(len(lines)):
        list_of_contents[-1] = list_of_contents[-1]  + "\n" + lines[i]
        if lines[i].startswith("-"):
            list_of_contents.append("")

    list_of_contents.pop()
    
    return list_of_contents

def name_content_pair(layout_content):
    content = layout_content.split("-")[0]
    name = layout_content.split("-")[1].replace("\n", "").replace("\t", "")

    content = content.replace("\t", "y")
    
    content = re.sub(r'\|.*\n', '', content)
    return (name, content)

def content_to_matrix(content):
    content = content.split("\n")
    matrix = []
    for i in range(len(content)):
        if content[i] != "":
            matrix.append(content[i].replace("xy", "x"))
    return matrix

def rectangulars(matrix):
    result = []
    pair_set = set()
    for i, row in enumerate(matrix):
        for j, char in enumerate(row):
            if char == 'x':
                right_count = 0
                down_count = 0
                # Count 'y' downward of 'x'
                for k in range(i + 1, len(matrix)):
                    if matrix[k][j] == 'x':
                        break
                    if matrix[k][j] == 'y':
                        down_count += 1
                        pair_set.add((k, j))
                down_count = down_count+1
                # Count 'y' to the right of 'x'
                for k in range(j + 1, len(row)):
                    found = False
                    iterated_something = False
                    for l in range(i, i+down_count):
                        iterated_something = True
                        if matrix[l][k] == 'x' or (l, k) in pair_set:
                            found = True
                            break
                    if not found and iterated_something:
                        right_count += 1
                    else:
                        break
                
                result.append((j, i, right_count+1, down_count))
    return (len(matrix[0]), len(matrix), result)

def get_polygon(cols, rows, rectangle):
    points = []
    (ox, oy, wx, wy) = rectangle
    points.append((ox, oy))
    points.append((ox + wx, oy))
    points.append((ox+wx, oy+wy))
    points.append((ox, oy+wy))
    return points

def get_polygonList(cols, rows, rectangles):
    polygons = []
    for i in range(len(rectangles)):
        polygons.append(get_polygon(cols, rows, rectangles[i]))
    return polygons

def get_polygonsString(cols, rows, polygons):
    polygons_string = []
    for i in range(len(polygons)):
        polygon_string = "  <mask id=\"svgmask" + str(i) + "\">\n"
        polygon_string = polygon_string + "    <polygon fill=\"#ffffff\" points=\""
        for j in range(len(polygons[i])):
            ox = polygons[i][j][0]
            oy = polygons[i][j][1]
            if j >= len(polygons[i])-1:
                polygon_string = polygon_string + "{{" + str(ox) + " * width / " + str(cols) + "}} {{" + str(oy) + " * height / " + str(rows) + "}}\"></polygon>\n"
            else:
                polygon_string = polygon_string + "{{" + str(ox) + " * width / " + str(cols) + "}} {{" + str(oy) + " * height / " + str(rows) + "}}" + ", "
        polygon_string = polygon_string + "  </mask>\n"
        polygons_string.append(polygon_string)

    return polygons_string

def get_image_str(name, cols, rows, rectangles):
    polygon_string = ""
    print(name)
    for i in range(len(rectangles)):
        if "(fill)" in name:
            polygon_string = polygon_string + "  <image xlink:href=\"{{image_" + str(i) + "}}\" mask=\"url(#svgmask" + str(i) + ")\" width=\"{{" + str(rectangles[i][2]) + " * width / " + str(cols) + "}}\" height=\"{{" + str(rectangles[i][3]) + " * height / " + str(rows) + "}}\" x=\"{{" + str(rectangles[i][0]) + " * width / " + str(cols) + "}}\" y=\"{{" + str(rectangles[i][1]) + " * height / " + str(rows) + "}}\"  preserveAspectRatio=\"xMidYMid slice\"></image>\n"
        else:
            polygon_string = polygon_string + "  <image xlink:href=\"{{image_" + str(i) + "}}\" mask=\"url(#svgmask" + str(i) + ")\" width=\"{{" + str(rectangles[i][2]) + " * width / " + str(cols) + "}}\" height=\"{{" + str(rectangles[i][3]) + " * height / " + str(rows) + "}}\" x=\"{{" + str(rectangles[i][0]) + " * width / " + str(cols) + "}}\" y=\"{{" + str(rectangles[i][1]) + " * height / " + str(rows) + "}}\"></image>\n"
    return polygon_string

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script.py <filename>")
        sys.exit(1)

    filename = sys.argv[1]
    list_of_layouts = read_file_from_command_line(filename)
    list_of_tagged_layouts = []
    list_of_matrices = []
    list_of_rectangulars = []

    for i in range(len(list_of_layouts)):
        list_of_tagged_layouts.append(name_content_pair(list_of_layouts[i]))

    for i in range(len(list_of_tagged_layouts)):
        matrix = content_to_matrix(list_of_tagged_layouts[i][1])
        #print(matrix)
        rectangulars_list = rectangulars(matrix)
        list_of_rectangulars.append(rectangulars_list)
        (cols, rows, rectangles) = rectangulars_list
        polygons = get_polygonList(cols, rows, rectangles)
        poly_str = get_polygonsString(cols, rows, polygons)
        template_file = ""
        template_file = template_file + "<svg width=\"{{width}}\" height=\"{{height}}\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n"
        for poly in poly_str:
            template_file = template_file + poly

        image_string = get_image_str(list_of_tagged_layouts[i][0], cols, rows, rectangles)

        template_file = template_file + image_string
        template_file = template_file + "</svg>"
        
        filename = str(len(rectangles)) + "-" + list_of_tagged_layouts[i][0].replace(" (fill)", "").replace(" ", "-") + ".template"
        filecontent = template_file

        with open(filename, "w") as file:
            file.write(filecontent)

        #print(filename)
        #print(str(rows))
        #print(str(cols))
        #print(matrix)
        #print(rectangulars_list)
        #print(polygons)
        #print("\n")




        
    


