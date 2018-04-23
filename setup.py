#!/usr/bin/env python

from setuptools import setup, Extension

module1 = Extension('skip32', sources = ['skip32/skip32.c'])

setup (
    name = 'fk-skip32',
    version = '1.0',
    description = 'implementation of skip32 based on http://search.cpan.org/~esh/Crypt-Skip32-0.08/lib/Crypt/Skip32.pm',
    url='https://github.com/fictivekin/fk-skip32',
    ext_modules = [module1],
    author = 'Keith Bussell',
    maintainer='Fictive Kin, LLC',
    maintainer_email='frank@fictivekin.com',
)
