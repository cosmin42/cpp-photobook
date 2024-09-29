import sys

def sort_lines_in_file(filename, start_line, end_line):
    try:
        # Read the content of the file
        with open(filename, 'r') as file:
            lines = file.readlines()

        # Adjust for zero-based index and check range validity
        start_line -= 1  # Convert to zero-based index
        end_line -= 1    # Convert to zero-based index
        if start_line < 0 or end_line >= len(lines) or start_line > end_line:
            print("Invalid line range specified.")
            return

        # Sort lines in the specified range
        lines[start_line:end_line + 1] = sorted(lines[start_line:end_line + 1])

        # Write back to the file
        with open(filename, 'w') as file:
            file.writelines(lines)

        print(f"Lines {start_line + 1} to {end_line + 1} sorted successfully.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python sort_lines.py <filename> <start_line> <end_line>")
    else:
        filename = sys.argv[1]
        try:
            start_line = int(sys.argv[2])
            end_line = int(sys.argv[3])
            sort_lines_in_file(filename, start_line, end_line)
        except ValueError:
            print("Please provide valid line numbers for start_line and end_line.")