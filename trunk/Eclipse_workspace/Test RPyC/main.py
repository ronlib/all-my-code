import thread
import time

import outer_server
import inner_server


def main():
	thread.start_new_thread(inner_server.run, ())
	time.sleep(2)
	outer_server.run()
	
if __name__ == '__main__':
	main()