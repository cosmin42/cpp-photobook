import os
import sys
from jinja2 import Template

def read_file(file_path):
    if os.path.exists(file_path):
        try:
            with open(file_path, 'r') as file:
                contents = file.read()
                return contents
        except Exception as e:
            print(f"An error occurred while reading the file: {e}")
    else:
        print(f"The file '{file_path}' does not exist.")
    return None

def render_file(templateContent, values):
    template = Template(templateContent)
    return template.render(values)

def main():
    if len(sys.argv) < 2:
        print("Usage: python renderTemplate.py <file_path>")
        sys.exit(1)

    filepath = sys.argv[1]
    
    content = read_file(filepath)

    if content == None:
        return
    
    result = render_file(content, {})

    print(result)

if __name__ == "__main__":
    main()
