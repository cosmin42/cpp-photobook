import json
import sys

def parse_color(palette):
    ui_palette = palette.get("ui", {})
    fonts_palette = palette.get("fonts", {})
    return ui_palette, fonts_palette

def print_for_macos_ios(ui_palette, fonts_palette):
    print("// Generated for SwiftUI")
    for name, color in ui_palette.items():
        print(f"    static let {name} = parseColor(hex: \"{color}\")")
    for name, color in fonts_palette.items():
        print(f"    static let {name} = parseColor(hex: \"{color}\")")

def print_combined_macos_ios(data):
    data_macos = data.get("macos", {})
    data_ios = data.get("ios", {})
    ui_palette_macos, fonts_palette_macos = parse_color(data_macos)
    ui_palette_ios, fonts_palette_ios = parse_color(data_ios)
    print("#if os(macOS)")
    print_for_macos_ios(ui_palette_macos, fonts_palette_macos)
    print("#else")
    print_for_macos_ios(ui_palette_ios, fonts_palette_ios)
    print("#endif")


def print_for_windows(data):
    data_windows = data.get("windows", {})
    ui_palette, fonts_palette = parse_color(data_windows)
    print("<!-- Generated for WinUI3 -->")
    for name, color in ui_palette.items():
        print(f"<Color x:Key=\"{name}\">{color}</Color>")
    for name, color in fonts_palette.items():
        print(f"<Color x:Key=\"{name}\">{color}</Color>")

def load_palette(file_name, palette_name):
    # Load JSON data from the specified file
    try:
        with open(file_name, 'r') as file:
            data = json.load(file)
    except FileNotFoundError:
        print(f"Error: File '{file_name}' not found.")
        return
    except json.JSONDecodeError:
        print("Error: Failed to parse JSON.")
        return

    # Check if the specified palette exists in the JSON data
    palette = data.get(palette_name, {})
    if palette is None:
        print(f"Error: Palette '{palette_name}' not found.")
        return
    # Print the colors in the chosen palette
    print_combined_macos_ios(palette)
    print_for_windows(palette)


# Check command-line arguments
if len(sys.argv) != 3:
    print("Usage: python gen-palette.py <file_name> <palette_name>")
    print("Example: python gen-palette.py palette.json original-palette")
else:
    file_name = sys.argv[1]
    palette_name = sys.argv[2]
    load_palette(file_name, palette_name)
