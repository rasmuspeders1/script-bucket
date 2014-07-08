#! /usr/bin/env python

import sh
from sh import sudo
import os
import getpass
#sudo losetup /dev/loop0 crypt
#sudo cryptsetup open /dev/loop0 secdir
#sudo mount /dev/mapper/secdir secdir
#sudo chown rasmus secdir
import logging

output = ''
def interactive_handler(out, stdin):
	global output
	output += out
	print(out, end='', flush=True)
	if output.endswith(': '):
		interactive_input = getpass.getpass('')
		stdin.put(interactive_input +'\n')
		return False
	else:
		return False
		
	return True

class CryptContainer:
	def __init__(self):
		self.logger = logging.getLogger(__name__)
		logHandler = logging.StreamHandler()
		self.logger.addHandler(logHandler)
		self.logger.setLevel(logging.INFO)
		self.loop_devs = {}
		
	def CreateFile(self):
		pass
	
	def CreateLoopDev(self, filepath):
		loop_dev = sh.losetup('-f').strip()
		self.logger.info('Creating loopback device [{0}] for file [{1}]'.format(loop_dev, filepath))
		
		if not loop_dev:
			raise RuntimeError('No available loop devices')

		if not os.path.isfile(filepath):
			raise RuntimeError('Path to file [{0}] does not exists or is not a file!'.format(filepath))
		
		self.loop_devs[filepath] = loop_dev
		sh.losetup(loop_dev, filepath)
		self.logger.info('Created loopback device [{0}] for file [{1}]'.format(loop_dev, filepath))
		
	def DeleteLoopDev(self, filepath):
		self.logger.info('Deleting loopback device [{0}] for file [{1}]'.format(self.loop_devs[filepath], filepath))
		if not os.path.isfile(filepath):
			raise RuntimeError('Path to file [{0}] does not exist or is not a file!'.format(filepath))
		
		if not filepath in self.loop_devs:
			raise RuntimeError('No loop device currently active for file [{0}]'.format(filepath))
		sh.losetup('-d', self.loop_devs[filepath])
		self.logger.info('Deleted loopback device [{0}] for file [{1}]'.format(self.loop_devs[filepath], filepath))
		


filepath = os.path.abspath('/home/rasmus/tmp/crypt')
cc = CryptContainer()
cc.CreateLoopDev(filepath)
cc.DeleteLoopDev(filepath)
#print('Opening encrypting container')
#sh.cryptsetup('open', cc.loop_devs[filepath], 'secdir', _tty_in=True, _err_to_out=True, _out_bufsize=0, _out=interactive_handler).wait()
#print('Mounting container')
#sh.mount('/dev/mapper/secdir', 'secdir')
#print('Changing ownership of mounted container')
#current_user = os.getenv('SUDO_USER')
#sh.chown(current_user, 'secdir')




