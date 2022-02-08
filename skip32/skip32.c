 /*
    SKIP32 -- 32 bit block cipher based on SKIPJACK.
    Written by Greg Rose, QUALCOMM Australia, 1999/04/27.

    In common: F-table, G-permutation, key schedule.
    Different: 24 round feistel structure.
    Based on:  Unoptimized test implementation of SKIPJACK algorithm
               Panu Rissanen <bande@lut.fi>

    SKIPJACK and KEA Algorithm Specifications
    Version 2.0
    29 May 1998

    Not copyright, no rights reserved.
    
    Keith Bussell: Added python wrapping code 7/11/2008
 */

#define PY_SSIZE_T_CLEAN /* Make "z#" use Py_ssize_t rather than int. */
#include <Python.h>

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

typedef unsigned char   BYTE; /* 8 bits */
typedef unsigned short  WORD; /* 16 bits */

const BYTE ftable[256] = {
0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3,0x21,0x15,0x78,0x99,0xb1,0xaf,0xf9,
0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,0x4e,0x38,0x44,0x28,
0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,0x7a,0xc3,0xe9,0xfa,0x3d,0x53,
0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2,
0x39,0xb6,0x7b,0x0f,0xc1,0x93,0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8,
0x55,0xb9,0xda,0x85,0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90,
0x35,0xd5,0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76,
0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,0x4a,0x1d,
0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,0xde,0xc7,0x67,0x18,
0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,0xdc,0xc9,0x5d,0x5c,0x31,0xa4,
0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40,
0x34,0x4b,0x1c,0x73,0xd1,0xc4,0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5,
0xad,0x04,0x23,0x9c,0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2,
0x0c,0xef,0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8,
0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,0xbb,0xac,
0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,0x05,0x59,0x2a,0x46
};

WORD
g(const BYTE *key, int k, WORD w)
{
    BYTE g1, g2, g3, g4, g5, g6;

    g1 = (w>>8)&0xff;
    g2 = w&0xff;

    g3 = ftable[g2 ^ key[(4*k)%10]] ^ g1;
    g4 = ftable[g3 ^ key[(4*k+1)%10]] ^ g2;
    g5 = ftable[g4 ^ key[(4*k+2)%10]] ^ g3;
    g6 = ftable[g5 ^ key[(4*k+3)%10]] ^ g4;

    return ((g5<<8) + g6);
}

void
skip32_impl(const BYTE key[10], BYTE buf[4], int encrypt)
{
    int         k; /* round number */
    int         i; /* round counter */
    int         kstep;
    WORD        wl, wr;

    /* sort out direction */
    if (encrypt)
        kstep = 1, k = 0;
    else
        kstep = -1, k = 23;

    /* pack into words */
    wl = (buf[0] << 8) + buf[1];
    wr = (buf[2] << 8) + buf[3];

    /* 24 feistel rounds, doubled up */
    for (i = 0; i < 24/2; ++i) {
        wr ^= g(key, k, wl) ^ k;
        k += kstep;
        wl ^= g(key, k, wr) ^ k;
        k += kstep;
    }

    /* implicitly swap halves while unpacking */
    buf[0] = wr >> 8;   buf[1] = wr & 0xFF;
    buf[2] = wl >> 8;   buf[3] = wl & 0xFF;
}

static PyObject* test(PyObject* self, PyObject* args)
{
    BYTE        in[4] = { 0x33,0x22,0x11,0x00 };
    BYTE        key[10] = { 0x00,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11 };
    
    skip32_impl(key, in, 1);
    printf("%02x%02x%02x%02x\n", in[0], in[1], in[2], in[3]);
    if (in[0] != 0x81 || in[1] != 0x9d || in[2] != 0x5f || in[3] != 0x1f) {
        printf("819d5f1f is the answer! Didn't encrypt correctly!\n");
        return NULL;
    }

    skip32_impl(key, in, 0);
    if (in[0] != 0x33 || in[1] != 0x22 || in[2] != 0x11 || in[3] != 0x00) {
        printf("%02x%02x%02x%02x\n", in[0], in[1], in[2], in[3]);
        printf("33221100 is the answer! Didn't decrypt correctly!\n");
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* skip32_encrypt(PyObject* self, PyObject* args)
{
    const BYTE* key;
    int key_len;
    BYTE buf[4];
    unsigned int input;
    unsigned int encrypted;

    if (!PyArg_ParseTuple(args, "z#I", &key, &key_len, &input))
        return NULL;

    if (key_len != 10)
        return NULL;
    
    memcpy(&buf, &input, 4);
    
    skip32_impl(key, (BYTE*)&buf, 1);
    
    memcpy(&encrypted, &buf, 4);
    
    return PyLong_FromUnsignedLong(encrypted);
}

static PyObject* skip32_decrypt(PyObject* self, PyObject* args)
{
    const BYTE* key;
    int key_len;
    BYTE buf[4];
    unsigned int input;
    unsigned int decrypted;

    if (!PyArg_ParseTuple(args, "z#I", &key, &key_len, &input))
        return NULL;

    if (key_len != 10)
        return NULL;
    
    memcpy(&buf, &input, 4);
    
    skip32_impl(key, (BYTE*)&buf, 0);
    
    memcpy(&decrypted, &buf, 4);
    
    return PyLong_FromUnsignedLong(decrypted);
}

static PyMethodDef Skip32Methods[] =
{
    {"test", test, METH_VARARGS, "test the encryption/decryption."},
    {"encrypt", skip32_encrypt, METH_VARARGS, "encrypt a 4-byte number using a 10-byte key"},
    {"decrypt", skip32_decrypt, METH_VARARGS, "decrypt a 4-byte number using a 10-byte key"},
    {NULL, NULL, 0, NULL}
};

#ifdef IS_PY3K
  #define MOD_ERROR_VAL NULL
  #define MOD_SUCCESS_VAL(val) val
  #define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
  #define MOD_DEF(ob, name, doc, methods) \
          static struct PyModuleDef moduledef = { \
            PyModuleDef_HEAD_INIT, name, doc, -1, methods, }; \
          ob = PyModule_Create(&moduledef);
#else
  #define MOD_ERROR_VAL
  #define MOD_SUCCESS_VAL(val)
  #define MOD_INIT(name) void init##name(void)
  #define MOD_DEF(ob, name, doc, methods) \
          ob = Py_InitModule3(name, methods, doc);
#endif

MOD_INIT(skip32)
{
    assert(sizeof(int) == 4);
    PyObject *m;

    MOD_DEF(m, "skip32", "Skip32 Module.",
            Skip32Methods)

    if (m == NULL)
        return MOD_ERROR_VAL;

    return MOD_SUCCESS_VAL(m);
}