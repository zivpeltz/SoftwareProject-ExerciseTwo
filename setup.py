from setuptools import Extension, setup

module = Extension("kmeanssp", sources=['kmeanslogic.c','kmeansmodule.c'])
setup(name='kmeanssp',
     version='1.0',
     description='Python wrapper for C implementation of kmeans',
     ext_modules=[module])
