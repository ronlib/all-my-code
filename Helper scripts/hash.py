import hashlib
import sys

if __name__ == '__main__':
	file_path = sys.argv[1]
	m = hashlib.md5()
	m.update(open(file_path, 'rb').read())
	print (m.hexdigest())