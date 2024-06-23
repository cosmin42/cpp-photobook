import requests
import sys

def download_file(url, local_filename):
    # Send a GET request to the URL
    with requests.get(url, stream=True) as r:
        r.raise_for_status()  # Check if the request was successful
        # Open a local file in write-binary mode
        with open(local_filename, 'wb') as f:
            # Write the content to the local file in chunks
            for chunk in r.iter_content(chunk_size=8192):
                f.write(chunk)
    return local_filename

# Example usage
url = sys.argv[1]
local_filename = 'ninja.png'
download_file(url, local_filename)
print(f'File downloaded as {local_filename}')