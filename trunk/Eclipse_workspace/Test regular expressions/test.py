import re

STR = open("test_file.txt").read()


def main():
    re_pattern = re.compile("log\d\nlog\d")
    result = re_pattern.match(STR)
    
    print result.string
    

if __name__ == '__main__':
    main()