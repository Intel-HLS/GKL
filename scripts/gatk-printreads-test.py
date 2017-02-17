#!/usr/bin/env python
import logging
import os
import subprocess
import sys

logger = None

def initLogger(filename):
    global logger
    if logger == None:
        logger = logging.getLogger()
    else:
        for handler in logger.handlers[:]:
            logger.removeHandler(handler)

    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter(fmt='%(asctime)s %(levelname)s  %(message)s')

    fh = logging.FileHandler(filename)
    fh.setFormatter(formatter)
    logger.addHandler(fh)

    sh = logging.StreamHandler(sys.stdout)
    sh.setFormatter(formatter)
    logger.addHandler(sh)

def run(cmd, env='', logfile='output.log'):
    "run shell command"
    cmd = '{} /usr/bin/time -avo {} {} &>> {}'.format(env, logfile, cmd, logfile)
    logger.info('Directory: ' + os.getcwd())
    logger.info('Running: ' + cmd)
    subprocess.call(cmd, shell=True)

def newdir(name):
    "create unique directory and chdir to it"
    dir = name
    try:
        os.mkdir(dir)
    except:
        id = 1
        while True:
            try:
                dir = '{}.{}'.format(name, id)
                os.mkdir(dir)
                break
            except:
                id += 1
    os.chdir(dir)

def main(args):
    input = os.path.abspath(args[0])
    gatk = os.path.abspath(args[1])
    levels = [1, 5, 9]

    newdir('test')
    initLogger('run.log')

    for level in levels:
        # create test dir
        newdir('level-{}'.format(level))
        output = 'output.bam'
        
        # run PrintReads
        cmd = '{} PrintReads --addOutputSAMProgramRecord=false --input {} --output output.bam'.format(gatk, input, output)
        env = 'JAVA_OPTS=-Dsamjdk.compression_level={}'.format(level)
        run(cmd, env)
        
        # run CompareSAMs
        cmd = '{} CompareSAMs {} {}'.format(gatk, input, output)
        run(cmd)
        os.chdir('..')

if __name__ == "__main__":
   main(sys.argv[1:])
