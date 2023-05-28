#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <inttypes.h>

#define TFPF_MERSENNE_TWISTER_INCLUDE_MT19937_H_SKIP_MACRO_DEFINITIONS
#include "mt19937.h"


// `-9223372036854775808LL` raises a warning: it is processed as a unary minus
// applied to a positive integer, and that positive integer,
// 9223372036854775808, is outside the range of `int long long` on my system.
// Actually, even -9223372036854775808 may be outside the range of
// `int long long` as per the C standard; however, the standard guarantees that
// it is inside the range of `int64_t`. Hence, some gymnastics are necessary to
// assign values to these variables. I have done the same for the 32-bit case
// because 2147483648 and -2147483648 might be outside the range of `int long`
// on some systems.
static int32_t const i32_min = (int32_t)-2147483647L - 1;
static int32_t const i32_max = (int32_t)2147483647L;
static int64_t const i64_min = (int64_t)-9223372036854775807LL - 1;
static int64_t const i64_max = (int64_t)9223372036854775807LL;
static uint32_t const u32_max = 4294967295UL;
static uint64_t const u64_max = 18446744073709551615ULL;


static PyObject *
seed32(PyObject *self, PyObject *args)
{
    int long unsigned seed;
    if(!PyArg_ParseTuple(args, "k", &seed))
    {
        return NULL;
    }
    seed = PyLong_AsUnsignedLong(PyTuple_GET_ITEM(args, 0));
    if(PyErr_Occurred() != NULL || seed > u32_max)
    {
        return PyErr_Format(PyExc_ValueError, "argument 1 must be an integer in the range [0, %"PRIu32"]", u32_max);
    }
    mt19937_seed32((uint32_t)seed, NULL);
    Py_RETURN_NONE;
}


static PyObject *
seed64(PyObject *self, PyObject *args)
{
    int long long unsigned seed;
    if(!PyArg_ParseTuple(args, "K", &seed))
    {
        return NULL;
    }
    seed = PyLong_AsUnsignedLongLong(PyTuple_GET_ITEM(args, 0));
    if(PyErr_Occurred() != NULL || seed > u64_max)
    {
        return PyErr_Format(PyExc_ValueError, "argument 1 must be an integer in the range [0, %"PRIu64"]", u64_max);
    }
    mt19937_seed64((uint64_t)seed, NULL);
    Py_RETURN_NONE;
}


static PyObject *
init32(PyObject *self, PyObject *args)
{
    mt19937_init32(NULL);
    Py_RETURN_NONE;
}


static PyObject *
init64(PyObject *self, PyObject *args)
{
    mt19937_init64(NULL);
    Py_RETURN_NONE;
}


static PyObject *
rand32(PyObject *self, PyObject *args)
{
    return PyLong_FromUnsignedLong((int long unsigned)mt19937_rand32(NULL));
}


static PyObject *
rand64(PyObject *self, PyObject *args)
{
    return PyLong_FromUnsignedLongLong((int long long unsigned)mt19937_rand64(NULL));
}


static PyObject *
uint32(PyObject *self, PyObject *args)
{
    int long unsigned modulus;
    if(!PyArg_ParseTuple(args, "k", &modulus))
    {
        return NULL;
    }
    modulus = PyLong_AsUnsignedLong(PyTuple_GET_ITEM(args, 0));
    if(PyErr_Occurred() != NULL || modulus == 0 || modulus > u32_max)
    {
        return PyErr_Format(PyExc_ValueError, "argument 1 must be an integer in the range [1, %"PRIu32"]", u32_max);
    }
    return PyLong_FromUnsignedLong((int long unsigned)mt19937_uint32((uint32_t)modulus, NULL));
}


static PyObject *
uint64(PyObject *self, PyObject *args)
{
    int long long unsigned modulus;
    if(!PyArg_ParseTuple(args, "K", &modulus))
    {
        return NULL;
    }
    modulus = PyLong_AsUnsignedLongLong(PyTuple_GET_ITEM(args, 0));
    if(PyErr_Occurred() != NULL || modulus == 0 || modulus > u64_max)
    {
        return PyErr_Format(PyExc_ValueError, "argument 1 must be an integer in the range [1, %"PRIu64"]", u64_max);
    }
    return PyLong_FromUnsignedLongLong((int long long unsigned)mt19937_uint64((uint64_t)modulus, NULL));
}


static PyObject *
span32(PyObject *self, PyObject *args)
{
    int long left, right;
    PyObject *err = NULL;
    if(!PyArg_ParseTuple(args, "ll", &left, &right))
    {
        err = PyErr_Occurred();
        if(!PyErr_GivenExceptionMatches(err, PyExc_OverflowError))
        {
            return NULL;
        }
    }
    if(err != NULL || left < i32_min || left > i32_max || right < i32_min || right > i32_max || left >= right)
    {
        return PyErr_Format(
            PyExc_ValueError,
            "argument 1 must be less than argument 2; both must be integers in the range [%"PRId32", %"PRId32"] "
            "and fit in the C `long` type",
            i32_min, i32_max
        );
    }
    return PyLong_FromLong((int long)mt19937_span32((int32_t)left, (int32_t)right, NULL));
}


static PyObject *
span64(PyObject *self, PyObject *args)
{
    int long long left, right;
    PyObject *err = NULL;
    if(!PyArg_ParseTuple(args, "LL", &left, &right))
    {
        err = PyErr_Occurred();
        if(!PyErr_GivenExceptionMatches(err, PyExc_OverflowError))
        {
            return NULL;
        }
    }
    if(err != NULL || left < i64_min || left > i64_max || right < i64_min || right > i64_max || left >= right)
    {
        return PyErr_Format(
            PyExc_ValueError,
            "argument 1 must be less than argument 2; both must be integers in the range [%"PRId64", %"PRId64"] "
            "and fit in the C `long long` type",
            i64_min, i64_max
        );
    }
    return PyLong_FromLongLong((int long long)mt19937_span64((int64_t)left, (int64_t)right, NULL));
}


static PyObject *
real32(PyObject *self, PyObject *args)
{
    return PyFloat_FromDouble(mt19937_real32(NULL));
}


static PyObject *
real64(PyObject *self, PyObject *args)
{
    return PyFloat_FromDouble((double)mt19937_real64(NULL));
}


static PyObject *
drop32(PyObject *self, PyObject *args)
{
    int long long count;
    if(!PyArg_ParseTuple(args, "L", &count))
    {
        return NULL;
    }
    mt19937_drop32(count, NULL);
    Py_RETURN_NONE;
}


static PyObject *
drop64(PyObject *self, PyObject *args)
{
    int long long count;
    if(!PyArg_ParseTuple(args, "L", &count))
    {
        return NULL;
    }
    mt19937_drop64(count, NULL);
    Py_RETURN_NONE;
}


// Module information.
PyDoc_STRVAR(
    seed32_doc,
    "seed32(seed)\n"
    "Seed 32-bit MT19937.\n\n"
    ":param seed: 32-bit number."
);
PyDoc_STRVAR(
    seed64_doc,
    "seed64(seed)\n"
    "Seed 64-bit MT19937.\n\n"
    ":param seed: 64-bit number."
);
PyDoc_STRVAR(
    init32_doc,
    "init32()\n"
    "Seed 32-bit MT19937 with an unspecified value generated at run-time."
);
PyDoc_STRVAR(
    init64_doc,
    "init64()\n"
    "Seed 64-bit MT19937 with an unspecified value generated at run-time."
);
PyDoc_STRVAR(
    rand32_doc,
    "rand32() -> int\n"
    "Generate a pseudorandom number.\n\n"
    ":return: Uniform pseudorandom 32-bit number."
);
PyDoc_STRVAR(
    rand64_doc,
    "rand64() -> int\n"
    "Generate a pseudorandom number.\n\n"
    ":return: Uniform pseudorandom 64-bit number."
);
PyDoc_STRVAR(
    uint32_doc,
    "uint32(modulus) -> int\n"
    "Generate a pseudorandom residue.\n\n"
    ":param modulus: 32-bit number. Must not be 0.\n\n"
    ":return: Uniform pseudorandom 32-bit number from 0 (inclusive) to `modulus` (exclusive)."
);
PyDoc_STRVAR(
    uint64_doc,
    "uint64(modulus) -> int\n"
    "Generate a pseudorandom residue.\n\n"
    ":param modulus: 64-bit number. Must not be 0.\n\n"
    ":return: Uniform pseudorandom 64-bit number from 0 (inclusive) to `modulus` (exclusive)."
);
PyDoc_STRVAR(
    span32_doc,
    "span32(left, right) -> int\n"
    "Generate a pseudorandom residue offset.\n\n"
    ":param left: 32-bit number.\n"
    ":param right: 32-bit number. Must be greater than `left`.\n\n"
    ":return: Uniform pseudorandom 32-bit number from `left` (inclusive) to `right` (exclusive)."
);
PyDoc_STRVAR(
    span64_doc,
    "span64(left, right) -> int\n"
    "Generate a pseudorandom residue offset.\n\n"
    ":param left: 64-bit number.\n"
    ":param right: 64-bit number. Must be greater than `left`.\n\n"
    ":return: Uniform pseudorandom 64-bit number from `left` (inclusive) to `right` (exclusive)."
);
PyDoc_STRVAR(
    real32_doc,
    "real32() -> float\n"
    "Generate a pseudorandom fraction.\n\n"
    ":return: Uniform pseudorandom number from 0 (inclusive) to 1 (inclusive)."
);
PyDoc_STRVAR(
    real64_doc,
    "real64() -> float\n"
    "Generate a pseudorandom fraction.\n\n"
    ":return: Uniform pseudorandom number from 0 (inclusive) to 1 (inclusive)."
);
PyDoc_STRVAR(
    drop32_doc,
    "drop32(count)\n"
    "Mutate 32-bit MT19937 by advancing its internal state. Equivalent to running `rand32()` `count` times and "
    "discarding the results.\n\n"
    ":param count: Number of steps to advance the state by. If not positive, this function has no effect."
);
PyDoc_STRVAR(
    drop64_doc,
    "drop64(count)\n"
    "Mutate 64-bit MT19937 by advancing its internal state. Equivalent to running `rand64()` `count` times and "
    "discarding the results.\n\n"
    ":param count: Number of steps to advance the state by. If not positive, this function has no effect."
);
static PyMethodDef pymt19937_methods[] =
{
    {"seed32", seed32, METH_VARARGS, seed32_doc},
    {"seed64", seed64, METH_VARARGS, seed64_doc},
    {"init32", init32, METH_NOARGS, init32_doc},
    {"init64", init64, METH_NOARGS, init64_doc},
    {"rand32", rand32, METH_NOARGS, rand32_doc},
    {"rand64", rand64, METH_NOARGS, rand64_doc},
    {"uint32", uint32, METH_VARARGS, uint32_doc},
    {"uint64", uint64, METH_VARARGS, uint64_doc},
    {"span32", span32, METH_VARARGS, span32_doc},
    {"span64", span64, METH_VARARGS, span64_doc},
    {"real32", real32, METH_NOARGS, real32_doc},
    {"real64", real64, METH_NOARGS, real64_doc},
    {"drop32", drop32, METH_VARARGS, drop32_doc},
    {"drop64", drop64, METH_VARARGS, drop64_doc},
    {NULL, NULL, 0, NULL},
};
static PyModuleDef pymt19937_module =
{
    PyModuleDef_HEAD_INIT,
    "mt19937",
    "Python API for a C implementation of MT19937 "
    "(see https://github.com/tfpf/mersenne-twister/blob/main/doc for the full documentation)",
    -1,
    pymt19937_methods,
};


PyMODINIT_FUNC
PyInit_mt19937(void)
{
    return PyModule_Create(&pymt19937_module);
}
