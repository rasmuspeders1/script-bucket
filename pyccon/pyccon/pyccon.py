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
def interactive_handler_std_getpass(out, stdin):
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
	def __init__(self, filepath_):
		self.filepath = filepath_
		self.loop_dev = None
		self.mapped_name = self.__CreateMappedName(self.filepath)
		self.mapped_device = os.path.join('/dev/mapper', self.mapped_name)
		self.mount_path = self.__CreateDefaultMountPath(self.filepath)
		self.btrfs_root_volume_name = 'root_volume'
		self.user_owner = getpass.getuser()
	
	def __CreateMappedName(self, filepath):
		return os.path.abspath(filepath).replace('/','_')
		
	def __CreateDefaultMountPath(self, filepath):
		return filepath + '_mounted'
		
class CryptContainerManager:
	def __init__(self):
		self.logger = logging.getLogger(__name__)
		logHandler = logging.StreamHandler()
		self.logger.addHandler(logHandler)
		self.logger.setLevel(logging.INFO)
		self.containers = {}
		
	def CreateFile(self, filepath):
		pass
	
	def CreateContainer(self, filepath):
		self.containers[filepath] = CryptContainer(filepath)
		
	def CreateLoopDev(self, filepath):
		loop_dev = sh.losetup('-f').strip()
		self.logger.info('Creating loopback device [{0}] for file [{1}]'.format(loop_dev, filepath))
		
		if not loop_dev:
			raise RuntimeError('No available loop devices')

		if not os.path.isfile(filepath):
			raise RuntimeError('Path to file [{0}] does not exists or is not a file!'.format(filepath))
		
		self.containers[filepath].loop_dev = loop_dev
		sh.losetup(loop_dev, filepath)
		self.logger.info('Created loopback device [{0}] for file [{1}]'.format(loop_dev, filepath))
		
	def DeleteLoopDev(self, filepath):
		self.logger.info('Deleting loopback device [{0}] for file [{1}]'.format(self.containers[filepath].loop_dev, filepath))
		if not os.path.isfile(filepath):
			raise RuntimeError('Path to file [{0}] does not exist or is not a file!'.format(filepath))
		
		if not filepath in self.containers:
			raise RuntimeError('No loop device currently active for file [{0}]'.format(filepath))
		sh.losetup('-d', self.containers[filepath].loop_dev)
		self.logger.info('Deleted loopback device [{0}] for file [{1}]'.format(self.containers[filepath].loop_dev, filepath))
		
	def SetupCrypt(self, filepath):
		self.logger.info('Setting up encrypted container on loopback device [{0}] for file [{1}]'.format(self.containers[filepath].loop_dev, filepath))
		sh.cryptsetup( '-v', 'luksFormat', self.containers[filepath].loop_dev,  _tty_in=True, _err_to_out=True, _out_bufsize=0, _out=interactive_handler_std_getpass).wait()
		self.logger.info('Done setting up encrypted container on loopback device [{0}] for file [{1}]'.format(self.containers[filepath].loop_dev, filepath))
		
	def OpenCrypt(self, filepath):
		self.logger.info('Opening encrypted container on loopback device [{0}] for file [{1}]'.format(self.containers[filepath].loop_dev, filepath))
		container = self.containers[filepath]
		self.logger.info('Mapping to device [{0}]'.format(container.mapped_name))
		sh.cryptsetup('open', self.containers[filepath].loop_dev, container.mapped_name, _tty_in=True, _err_to_out=True, _out_bufsize=0, _out=interactive_handler_std_getpass).wait()
		self.logger.info('Opened encrypted container on loopback device [{0}] for file [{1}] on mapped device [{2}]'.format(self.containers[filepath].loop_dev, filepath, self.containers[filepath].mapped_device))
	
	def CreateFileSystem(self, filepath):
		container = self.containers[filepath]
		self.logger.info('Creating filesystem on encrypted container [{0}]'.format(container.filepath))
		sh.mkfs('-t','btrfs', container.mapped_device)
		self.logger.info('Created filesystem on encrypted container [{0}]'.format(container.filepath))
	
	def MountCrypt(self, filepath):
		container = self.containers[filepath]
		self.logger.info('Mounting encrypted container [{0}] to [{1}]'.format(container.filepath, container.mount_path))
		if os.path.exists(container.mount_path) and os.path.isfile(container.mount_path):
			raise RuntimeError('The mount path [{0}] already exists and is a file'.format(container.mount_path))
		
		if not os.path.exists(container.mount_path):
			self.logger.info('Creating directory [{0}] for mounting encrypted container [{1}]'.format(container.mount_path, container.filepath))
			os.makedirs(container.mount_path)
			sh.chown(container.user_owner, container.mount_path)
		
		sh.mount('-t', 'btrfs', container.mapped_device, container.mount_path)

			
	
		
#cryptsetup -v luksFormat /dev/loop0
filepath = os.path.abspath('/home/rasmus/crypt')

cc = CryptContainerManager()
cc.CreateContainer(filepath)
cc.CreateLoopDev(filepath)
cc.SetupCrypt(filepath)
cc.OpenCrypt(filepath)
cc.CreateFileSystem(filepath)
cc.MountCrypt(filepath)
#cc.DeleteLoopDev(filepath)
#print('Opening encrypting container')
#sh.cryptsetup('open', cc.loop_devs[filepath], 'secdir', _tty_in=True, _err_to_out=True, _out_bufsize=0, _out=interactive_handler).wait()
#print('Mounting container')
#sh.mount('/dev/mapper/secdir', 'secdir')
#print('Changing ownership of mounted container')
#current_user = os.getenv('SUDO_USER')
##sh.chown(current_user, 'secdir')




