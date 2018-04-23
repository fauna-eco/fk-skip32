# fk-skip32

Forked from https://github.com/lukeweber/pyskip32 to be "vendored".  The version(s) of skip32 available on PyPi are not compatible with Python3 (the C bindings changed between Python2 and Python3).  This version updates the C file with some macros to work in both Python2 and Python3 environments.  The original README.txt continues below:

-----

pyskip32
========

Wraps the original c implementation of skip32 by Greg Rose, QUALCOMM Australia with a thin layer of python.

Example Usage
=============

```python
>>> import random
>>> import skip32
>>> key = ''.join(chr(random.randint(0, 255)) for _ in xrange(10))
>>> encrypted = skip32.encrypt(key, 12345)
>>> encrypted
3798503945L
>>> skip32.decrypt(key, encoded)
12345L
```
