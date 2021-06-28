/*
 * Copyright 2011      Sven Verdoolaege
 * Copyright 2012-2013 Ecole Normale Superieure
 *
 * Use of this software is governed by the MIT license
 *
 * Written by Sven Verdoolaege,
 * Ecole Normale Superieure, 45 rue d'Ulm, 75230 Paris, France
 */

#include <isl/space.h>
#include <isl_val_private.h>

#include <isl_multi_macro.h>

/* Add "multi2" to "multi1" and return the result.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),add)(__isl_take MULTI(BASE) *multi1,
	__isl_take MULTI(BASE) *multi2)
{
	return FN(MULTI(BASE),bin_op)(multi1, multi2, &FN(EL,add));
}

/* Subtract "multi2" from "multi1" and return the result.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),sub)(__isl_take MULTI(BASE) *multi1,
	__isl_take MULTI(BASE) *multi2)
{
	return FN(MULTI(BASE),bin_op)(multi1, multi2, &FN(EL,sub));
}

/* Depending on "fn", multiply or divide the elements of "multi" by "v" and
 * return the result.
 */
static __isl_give MULTI(BASE) *FN(MULTI(BASE),scale_val_fn)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_val *v,
	__isl_give EL *(*fn)(__isl_take EL *el, __isl_take isl_val *v))
{
	if (!multi || !v)
		goto error;

	if (isl_val_is_one(v)) {
		isl_val_free(v);
		return multi;
	}

	if (!isl_val_is_rat(v))
		isl_die(isl_val_get_ctx(v), isl_error_invalid,
			"expecting rational factor", goto error);

	return FN(MULTI(BASE),fn_val)(multi, fn, v);
error:
	isl_val_free(v);
	return FN(MULTI(BASE),free)(multi);
}

/* Multiply the elements of "multi" by "v" and return the result.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),scale_val)(__isl_take MULTI(BASE) *multi,
	__isl_take isl_val *v)
{
	return FN(MULTI(BASE),scale_val_fn)(multi, v, &FN(EL,scale_val));
}

/* Divide the elements of "multi" by "v" and return the result.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),scale_down_val)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_val *v)
{
	if (!v)
		goto error;
	if (isl_val_is_zero(v))
		isl_die(isl_val_get_ctx(v), isl_error_invalid,
			"cannot scale down by zero", goto error);
	return FN(MULTI(BASE),scale_val_fn)(multi, v, &FN(EL,scale_down_val));
error:
	isl_val_free(v);
	return FN(MULTI(BASE),free)(multi);
}

#undef TYPE
#define TYPE	MULTI(BASE)
#include "isl_type_check_match_range_multi_val.c"

/* Multiply the elements of "multi" by the corresponding element of "mv"
 * and return the result.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),scale_multi_val)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_multi_val *mv)
{
	int i;

	if (FN(MULTI(BASE),check_match_range_multi_val)(multi, mv) < 0)
		goto error;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi)
		goto error;

	for (i = 0; i < multi->n; ++i) {
		isl_val *v;

		v = isl_multi_val_get_val(mv, i);
		multi->u.p[i] = FN(EL,scale_val)(multi->u.p[i], v);
		if (!multi->u.p[i])
			goto error;
	}

	isl_multi_val_free(mv);
	return multi;
error:
	isl_multi_val_free(mv);
	return FN(MULTI(BASE),free)(multi);
}

/* Divide the elements of "multi" by the corresponding element of "mv"
 * and return the result.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),scale_down_multi_val)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_multi_val *mv)
{
	int i;

	if (FN(MULTI(BASE),check_match_range_multi_val)(multi, mv) < 0)
		goto error;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi)
		return NULL;

	for (i = 0; i < multi->n; ++i) {
		isl_val *v;

		v = isl_multi_val_get_val(mv, i);
		multi->u.p[i] = FN(EL,scale_down_val)(multi->u.p[i], v);
		if (!multi->u.p[i])
			goto error;
	}

	isl_multi_val_free(mv);
	return multi;
error:
	isl_multi_val_free(mv);
	return FN(MULTI(BASE),free)(multi);
}

/* Compute the residues of the elements of "multi" modulo
 * the corresponding element of "mv" and return the result.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),mod_multi_val)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_multi_val *mv)
{
	int i;

	if (FN(MULTI(BASE),check_match_range_multi_val)(multi, mv) < 0)
		goto error;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi)
		goto error;

	for (i = 0; i < multi->n; ++i) {
		isl_val *v;

		v = isl_multi_val_get_val(mv, i);
		multi->u.p[i] = FN(EL,mod_val)(multi->u.p[i], v);
		if (!multi->u.p[i])
			goto error;
	}

	isl_multi_val_free(mv);
	return multi;
error:
	isl_multi_val_free(mv);
	return FN(MULTI(BASE),free)(multi);
}

/* Return the opposite of "multi".
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),neg)(__isl_take MULTI(BASE) *multi)
{
	int i;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi)
		return NULL;

	for (i = 0; i < multi->n; ++i) {
		multi->u.p[i] = FN(EL,neg)(multi->u.p[i]);
		if (!multi->u.p[i])
			return FN(MULTI(BASE),free)(multi);
	}

	return multi;
}
