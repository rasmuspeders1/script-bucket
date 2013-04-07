#!/usr/bin/env python
import logging
from datetime import datetime
import subprocess
import os
import argparse



logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)
logger = logging.getLogger("timelapse")

prefix = ""

def take_pictures(folder, interval, max_photos, prefix) :
    logger.info('Taking pictures')
    gphoto_cmd_line = 'gphoto2 --set-config beep=0 --set-config flashmode=0 -I {0} {1} --capture-image-and-download'.format(int(interval), '-F {0}'.format(max_photos) if max_photos else '').split()
    gphoto_cmd_line += ['--filename', '{0}/{1}%05n.jpg'.format(folder, prefix + '_' if prefix else '')]
    logger.debug('CMD LINE: %s',' '.join(gphoto_cmd_line))
    subprocess.call(gphoto_cmd_line)

def create_video(folder):
    logger.info('Making video')
    fps = 24
    crop = 2000 - (3008 / (16.0 / 9.0))
    folder = os.path.abspath(folder)
    output_file_name = os.path.join(folder, os.path.basename(folder)+'.mkv')
    ffmpeg_cmd_line = ['avconv', '-f', 'image2', '-i', '{0}/photo_%05d.jpg'.format(folder), '-c:v', 'libx264', '-preset', 'veryslow', '-crf', '18', '-r', str(fps), '-vf', 'crop=in_w:(in_w / (16.0 / 9.0)):0:0,scale=1920:1080', output_file_name]
    logger.debug('CMD LINE: %s', ' '.join(ffmpeg_cmd_line))
    subprocess.call(ffmpeg_cmd_line)

if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description='Take a bunch of pictures to create a time lapse video')
    parser.add_argument('--folder', default = 'photos', help = 'The name of the folder where the photos will be placed. Default is "photos".')
    parser.add_argument('--interval', default = 30, help = 'The interval with which the photos will be taken. Default is 30 seconds.')
    parser.add_argument('--maxphotos', default = None, help = 'The maximum amount of photos. Default is unlimited.')
    parser.add_argument('--createvid', default = None, help = 'use this to create a time lapse video of the photos in the folder specified here')
    args = parser.parse_args()
    
    if args.createvid != None:
        create_video(args.createvid)
        exit(0)
    


    
    else:
        if args.folder == '':
            raise StandardError('You cannot define an empty folder name!')

        if os.path.exists(args.folder) and not os.path.isdir(args.folder):
            raise StandardError('Specified folder <%s> already exists but is a file!')
        
        if not os.path.exists(args.folder):
            os.makedirs(args.folder)
        
        take_pictures(args.folder, args.interval, args.maxphotos, 'photo')
    
    
