import json
import sys

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

    ui_palette = palette.get("ui", {})
    fonts_palette = palette.get("fonts", {})

    print(f"UI Pallete colors:")
    for name, color in ui_palette.items():
        print(f"  {name}: {color}")

    print(f"Fonts Pallete colors:")
    for name, color in fonts_palette.items():
        print(f"  {name}: {color}")

    print()
    print()

    print("<!-- Generated for WinUI3 -->")
    for name, color in ui_palette.items():
        print(f"<Color x:Key=\"{name}\">{color}</Color>")
    for name, color in fonts_palette.items():
        print(f"<Color x:Key=\"{name}\">{color}</Color>")

    print()
    print()

    print("// Generated for SwiftUI")
    print("extension Color {")
    for name, color in ui_palette.items():
        print(f"    static let {name} = parseColor(\"hex: {color}\")")
    for name, color in fonts_palette.items():
        print(f"    static let {name} = parseColor(\"hex: {color}\")")
    print("}")


# Check command-line arguments
if len(sys.argv) != 3:
    print("Usage: python gen-palette.py <file_name> <palette_name>")
    print("Example: python gen-palette.py palette.json original-palette")
else:
    file_name = sys.argv[1]
    palette_name = sys.argv[2]
    load_palette(file_name, palette_name)
