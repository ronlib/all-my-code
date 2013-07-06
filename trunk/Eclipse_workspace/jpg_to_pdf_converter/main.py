import os
import sys
import subprocess

CONVERSION_UTILITY_PATH = r"C:\Program Files\ImageMagick-6.8.6-Q16\convert.exe"

def convert_path(source_path):
#     source_path = sys.argv[1]
#     dest_path = sys.argv[2]
    dest_path = os.path.join(os.path.dirname(source_path),
                             os.path.basename(source_path) + "_pdf")
    
    if not os.path.exists(dest_path):
        os.mkdir(dest_path)
    source_files_to_convert = os.listdir(source_path)
    for file_name in source_files_to_convert:
        if file_name.lower() == "thumbs.db":
            continue
        
        subprocess.call('%s %s %s' %(CONVERSION_UTILITY_PATH,
                                     os.path.join(source_path, file_name),
                                     os.path.join(dest_path, file_name[:file_name.rfind("jpg")] + "pdf")
                                     ))
        
        
def main():
    for path in os.listdir(sys.argv[1]):
        convert_path(os.path.join(sys.argv[1], path))
    
    
if __name__=='__main__':
    main()
    
