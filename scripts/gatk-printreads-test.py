#!/usr/bin/env python
import logging
import os
import subprocess
import sys
import argparse
import shlex

logger = None

def init_logger(filename):
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


def run(cmd, logfile='output.log'):
    "run shell command"
    logger.info('Directory: ' + os.getcwd())
    logger.info('Running: ' + cmd)
    cmd = ' /usr/bin/time -avo {} {} '.format(logfile, cmd)
    subprocess.call(shlex.split(cmd), shell=False)


def newdir(name):
    "create unique directory and chdir to it"
    dir_name = name
    try:
        os.mkdir(dir_name)
    except:
        level_id = 1
        while True:
            try:
                dir_name = '{}.{}'.format(name, level_id)
                os.mkdir(dir_name)
                break
            except:
                level_id += 1
    os.chdir(dir_name)


def main(args):
    parser = argparse.ArgumentParser()
    parser.add_argument('--gatk', default=argparse.SUPPRESS, help="input path to gatk tool")
    parser.add_argument('--input', default=argparse.SUPPRESS, help="input path to BAM file")

    args = parser.parse_args()
    if 'help' in args:
        print('Usage: python gatk-printreads-test.py --gatk <path to gatk> --input <path to bam file>')
    if 'gatk' in args:
        gatk_path = args.gatk
    else:
        gatk_path = os.getcwd()+"/gatk"

    if 'input' in args:
        input_path = args.input
    else:
        input_path = os.getcwd()+"/input.bam"

    gatk = os.path.abspath(gatk_path)
    if not os.path.isfile(gatk):
        exit("Please specify a valid gatk file using the --gatk parameter.")
    print('Setting gatk path as {}'.format(gatk_path))

    input_file = os.path.abspath(input_path)
    if not os.path.isfile(input_file):
        exit("Please specify a valid BAM file using the --input parameter.")
    print('Setting input bam as {}'.format(input_path))

    levels = [1, 5, 9]

    newdir('test')
    init_logger('run.log')

    for level in levels:
        # create test dir
        newdir('level-{}'.format(level))
        output = 'output.bam'

        # run PrintReads
        env = '--java-options "-Dsamjdk.compression_level={}"'.format(level)
        cmd = '{} {} PrintReads --input {} --output output.bam'.format(gatk, env, input_file, output)
        run(cmd)

        # run CompareSAMs
        cmd = '{} {} CompareSAMs {} {}'.format(gatk, env, input_file, output)
        run(cmd)
        os.chdir('..')


if __name__ == "__main__":
    if len(sys.argv) < 6:
        main(sys.argv[1:])
    else:
        print('Usage: python gatk-printreads-test.py --gatk </path/to/gatk> --input <path/to/input.bam>')
