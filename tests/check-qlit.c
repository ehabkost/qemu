/*
 * QLit unit-tests.
 *
 * Copyright (C) 2017 Red Hat Inc.
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"

#include "qapi/qmp/qbool.h"
#include "qapi/qmp/qdict.h"
#include "qapi/qmp/qlist.h"
#include "qapi/qmp/qlit.h"
#include "qapi/qmp/qnum.h"
#include "qapi/qmp/qstring.h"

static QLitObject qlit = QLIT_QDICT(((QLitDictEntry[]) {
    { "foo", QLIT_QNUM_INT(42) },
    { "bar", QLIT_QSTR("hello world") },
    { "baz", QLIT_QNULL },
    { "bee", QLIT_QLIST(((QLitObject[]) {
        QLIT_QNUM_INT(43),
        QLIT_QNUM_INT(44),
        QLIT_QBOOL(true),
        { },
    }))},
    { },
}));

static QObject *make_qobject(void)
{
    QDict *qdict = qdict_new();
    QList *list = qlist_new();

    qdict_put_int(qdict, "foo", 42);
    qdict_put_str(qdict, "bar", "hello world");
    qdict_put_null(qdict, "baz");

    qlist_append_int(list, 43);
    qlist_append_int(list, 44);
    qlist_append_bool(list, true);
    qdict_put(qdict, "bee", list);

    return QOBJECT(qdict);
}

static void qlit_equal_qobject_test(void)
{
    /* Each entry in the values[] array should be different from the others */
    QLitObject values[] = {
        QLIT_QNULL,
        QLIT_QBOOL(false),
        QLIT_QBOOL(true),
        QLIT_QNUM_INT(-1),
        QLIT_QNUM_INT(0),
        QLIT_QNUM_INT(1),
        QLIT_QNUM_INT(INT64_MIN),
        QLIT_QNUM_INT(INT64_MAX),
        QLIT_QNUM_UINT(UINT64_MAX),
        /* Larger than UINT64_MAX: */
        QLIT_QNUM_DOUBLE(18446744073709552e3),
        /* Smaller than INT64_MIN: */
        QLIT_QNUM_DOUBLE(-92233720368547758e2),
        QLIT_QSTR(""),
        QLIT_QSTR("foo"),
        qlit,
        QLIT_QDICT(((QLitDictEntry[]) {
            { "foo", QLIT_QNUM_INT(42) },
            { },
        })),
        QLIT_QLIST(((QLitObject[]){
            QLIT_QNUM_INT(-1),
            QLIT_QNUM_INT(0),
            QLIT_QNUM_INT(1),
            { },
        })),
    };
    int i;
    QObject *qobj = make_qobject();

    g_assert(qlit_equal_qobject(&qlit, qobj));

    qdict_put(qobject_to(QDict, qobj), "bee", qlist_new());
    g_assert(!qlit_equal_qobject(&qlit, qobj));

    qobject_unref(qobj);

    for (i = 0; i < ARRAY_SIZE(values); i++) {
        int j;
        QObject *o = qobject_from_qlit(&values[i]);
        for (j = 0; j < ARRAY_SIZE(values); j++) {
            g_assert(qlit_equal_qobject(&values[j], o) == (i == j));
        }
        qobject_unref(o);
    }

}

static void qlit_equal_large_qnum_test(void)
{
    /* 2^32-1 */
    QNum *large = qnum_from_uint(9223372036854775807LL);
    /* 2^32 */
    QNum *too_large = qnum_from_uint(9223372036854775808ULL);
    QNum *dbl = qnum_from_double(9223372036854775808.0);
    QLitObject qlit_large = QLIT_QNUM_INT(9223372036854775807LL);

    g_assert(qlit_equal_qobject(&qlit_large, QOBJECT(large)));
    g_assert(!qlit_equal_qobject(&qlit_large, QOBJECT(too_large)));
    g_assert(!qlit_equal_qobject(&qlit_large, QOBJECT(dbl)));

    qobject_unref(dbl);
    qobject_unref(large);
    qobject_unref(too_large);
}

static void qobject_from_qlit_test(void)
{
    QObject *obj, *qobj = qobject_from_qlit(&qlit);
    QDict *qdict;
    QList *bee;

    qdict = qobject_to(QDict, qobj);
    g_assert_cmpint(qdict_get_int(qdict, "foo"), ==, 42);
    g_assert_cmpstr(qdict_get_str(qdict, "bar"), ==, "hello world");
    g_assert(qobject_type(qdict_get(qdict, "baz")) == QTYPE_QNULL);

    bee = qdict_get_qlist(qdict, "bee");
    obj = qlist_pop(bee);
    g_assert_cmpint(qnum_get_int(qobject_to(QNum, obj)), ==, 43);
    qobject_unref(obj);
    obj = qlist_pop(bee);
    g_assert_cmpint(qnum_get_int(qobject_to(QNum, obj)), ==, 44);
    qobject_unref(obj);
    obj = qlist_pop(bee);
    g_assert(qbool_get_bool(qobject_to(QBool, obj)));
    qobject_unref(obj);

    qobject_unref(qobj);
}

int main(int argc, char **argv)
{
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/qlit/equal_qobject", qlit_equal_qobject_test);
    g_test_add_func("/qlit/equal_large_qnum", qlit_equal_large_qnum_test);
    g_test_add_func("/qlit/qobject_from_qlit", qobject_from_qlit_test);

    return g_test_run();
}
