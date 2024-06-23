import zipfile
import sys

def extract_zip(zip_file, extract_to):
    with zipfile.ZipFile(zip_file, 'r') as zip_ref:
        zip_ref.extractall(extract_to)


# Example usage
extract_zip(sys.argv[1], sys.argv[2])
print(f'ZIP file extracted to {sys.argv[2]}')