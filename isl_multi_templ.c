/*
 * Copyright 2011      Sven Verdoolaege
 * Copyright 2012      Ecole Normale Superieure
 *
 * Use of this software is governed by the MIT license
 *
 * Written by Sven Verdoolaege,
 * Ecole Normale Superieure, 45 rue d’Ulm, 75230 Paris, France
 */

#define xCAT(A,B) A ## B
#define CAT(A,B) xCAT(A,B)
#undef EL
#define EL CAT(isl_,BASE)
#define xFN(TYPE,NAME) TYPE ## _ ## NAME
#define FN(TYPE,NAME) xFN(TYPE,NAME)
#define xMULTI(BASE) isl_multi_ ## BASE
#define MULTI(BASE) xMULTI(BASE)
#define MULTI_NAME(BASE) "isl_multi_" #BASE
#define xLIST(EL) EL ## _list
#define LIST(EL) xLIST(EL)

isl_ctx *FN(MULTI(BASE),get_ctx)(__isl_keep MULTI(BASE) *multi)
{
	return multi ? isl_space_get_ctx(multi->space) : NULL;
}

__isl_give isl_space *FN(MULTI(BASE),get_space)(__isl_keep MULTI(BASE) *multi)
{
	return multi ? isl_space_copy(multi->space) : NULL;
}

__isl_give isl_space *FN(MULTI(BASE),get_domain_space)(
	__isl_keep MULTI(BASE) *multi)
{
	return multi ? isl_space_domain(isl_space_copy(multi->space)) : NULL;
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),alloc)(__isl_take isl_space *space)
{
	isl_ctx *ctx;
	int n;
	MULTI(BASE) *multi;

	if (!space)
		return NULL;

	ctx = isl_space_get_ctx(space);
	n = isl_space_dim(space, isl_dim_out);
	multi = isl_calloc(ctx, MULTI(BASE),
			 sizeof(MULTI(BASE)) + (n - 1) * sizeof(struct EL *));
	if (!multi)
		goto error;

	multi->space = space;
	multi->n = n;
	multi->ref = 1;
	return multi;
error:
	isl_space_free(space);
	return NULL;
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),dup)(__isl_keep MULTI(BASE) *multi)
{
	int i;
	MULTI(BASE) *dup;

	if (!multi)
		return NULL;

	dup = FN(MULTI(BASE),alloc)(isl_space_copy(multi->space));
	if (!dup)
		return NULL;

	for (i = 0; i < multi->n; ++i)
		dup = FN(FN(MULTI(BASE),set),BASE)(dup, i,
						    FN(EL,copy)(multi->p[i]));

	return dup;
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),cow)(__isl_take MULTI(BASE) *multi)
{
	if (!multi)
		return NULL;

	if (multi->ref == 1)
		return multi;

	multi->ref--;
	return FN(MULTI(BASE),dup)(multi);
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),copy)(__isl_keep MULTI(BASE) *multi)
{
	if (!multi)
		return NULL;

	multi->ref++;
	return multi;
}

void *FN(MULTI(BASE),free)(__isl_take MULTI(BASE) *multi)
{
	int i;

	if (!multi)
		return NULL;

	if (--multi->ref > 0)
		return NULL;

	isl_space_free(multi->space);
	for (i = 0; i < multi->n; ++i)
		FN(EL,free)(multi->p[i]);
	free(multi);

	return NULL;
}

unsigned FN(MULTI(BASE),dim)(__isl_keep MULTI(BASE) *multi,
	enum isl_dim_type type)
{
	return multi ? isl_space_dim(multi->space, type) : 0;
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),set_dim_name)(
	__isl_take MULTI(BASE) *multi,
	enum isl_dim_type type, unsigned pos, const char *s)
{
	int i;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi)
		return NULL;

	multi->space = isl_space_set_dim_name(multi->space, type, pos, s);
	if (!multi->space)
		return FN(MULTI(BASE),free)(multi);

	if (type == isl_dim_out)
		return multi;
	for (i = 0; i < multi->n; ++i) {
		multi->p[i] = FN(EL,set_dim_name)(multi->p[i], type, pos, s);
		if (!multi->p[i])
			return FN(MULTI(BASE),free)(multi);
	}

	return multi;
}

const char *FN(MULTI(BASE),get_tuple_name)(__isl_keep MULTI(BASE) *multi,
	enum isl_dim_type type)
{
	return multi ? isl_space_get_tuple_name(multi->space, type) : NULL;
}

__isl_give EL *FN(FN(MULTI(BASE),get),BASE)(__isl_keep MULTI(BASE) *multi,
	int pos)
{
	isl_ctx *ctx;

	if (!multi)
		return NULL;
	ctx = FN(MULTI(BASE),get_ctx)(multi);
	if (pos < 0 || pos >= multi->n)
		isl_die(ctx, isl_error_invalid,
			"index out of bounds", return NULL);
	return FN(EL,copy)(multi->p[pos]);
}

__isl_give MULTI(BASE) *FN(FN(MULTI(BASE),set),BASE)(
	__isl_take MULTI(BASE) *multi, int pos, __isl_take EL *el)
{
	isl_space *multi_space = NULL;
	isl_space *el_space = NULL;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi || !el)
		goto error;

	multi_space = FN(MULTI(BASE),get_space)(multi);
	el_space = FN(EL,get_space)(el);

	if (!isl_space_match(multi_space, isl_dim_param,
			    el_space, isl_dim_param))
		isl_die(FN(MULTI(BASE),get_ctx)(multi), isl_error_invalid,
			"parameters don't match", goto error);
	if (!isl_space_tuple_match(multi_space, isl_dim_in,
			    el_space, isl_dim_in))
		isl_die(FN(MULTI(BASE),get_ctx)(multi), isl_error_invalid,
			"domains don't match", goto error);

	if (pos < 0 || pos >= multi->n)
		isl_die(FN(MULTI(BASE),get_ctx)(multi), isl_error_invalid,
			"index out of bounds", goto error);

	FN(EL,free)(multi->p[pos]);
	multi->p[pos] = el;

	isl_space_free(multi_space);
	isl_space_free(el_space);

	return multi;
error:
	FN(MULTI(BASE),free)(multi);
	FN(EL,free)(el);
	isl_space_free(multi_space);
	isl_space_free(el_space);
	return NULL;
}

/* Reset the space of "multi".  This function is called from isl_pw_templ.c
 * and doesn't know if the space of an element object is represented
 * directly or through its domain.  It therefore passes along both,
 * which we pass along to the element function since we don't how
 * that is represented either.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),reset_space_and_domain)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_space *space,
	__isl_take isl_space *domain)
{
	int i;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi || !space || !domain)
		goto error;

	for (i = 0; i < multi->n; ++i) {
		multi->p[i] = FN(EL,reset_space_and_domain)(multi->p[i],
				 isl_space_copy(space), isl_space_copy(domain));
		if (!multi->p[i])
			goto error;
	}
	isl_space_free(domain);
	isl_space_free(multi->space);
	multi->space = space;

	return multi;
error:
	isl_space_free(domain);
	isl_space_free(space);
	FN(MULTI(BASE),free)(multi);
	return NULL;
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),reset_domain_space)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_space *domain)
{
	isl_space *space;

	space = isl_space_extend_domain_with_range(isl_space_copy(domain),
						isl_space_copy(multi->space));
	return FN(MULTI(BASE),reset_space_and_domain)(multi, space, domain);
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),reset_space)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_space *space)
{
	isl_space *domain;

	domain = isl_space_domain(isl_space_copy(space));
	return FN(MULTI(BASE),reset_space_and_domain)(multi, space, domain);
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),set_tuple_id)(
	__isl_keep MULTI(BASE) *multi, enum isl_dim_type type,
	__isl_take isl_id *id)
{
	isl_space *space;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi)
		return isl_id_free(id);

	space = FN(MULTI(BASE),get_space)(multi);
	space = isl_space_set_tuple_id(space, type, id);

	return FN(MULTI(BASE),reset_space)(multi, space);
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),realign_domain)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_reordering *exp)
{
	int i;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi || !exp)
		return NULL;

	for (i = 0; i < multi->n; ++i) {
		multi->p[i] = FN(EL,realign_domain)(multi->p[i],
						isl_reordering_copy(exp));
		if (!multi->p[i])
			goto error;
	}

	multi = FN(MULTI(BASE),reset_domain_space)(multi,
						    isl_space_copy(exp->dim));

	isl_reordering_free(exp);
	return multi;
error:
	isl_reordering_free(exp);
	FN(MULTI(BASE),free)(multi);
	return NULL;
}

/* Align the parameters of "multi" to those of "model".
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),align_params)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_space *model)
{
	isl_ctx *ctx;

	if (!multi || !model)
		goto error;

	ctx = isl_space_get_ctx(model);
	if (!isl_space_has_named_params(model))
		isl_die(ctx, isl_error_invalid,
			"model has unnamed parameters", goto error);
	if (!isl_space_has_named_params(multi->space))
		isl_die(ctx, isl_error_invalid,
			"input has unnamed parameters", goto error);
	if (!isl_space_match(multi->space, isl_dim_param,
			     model, isl_dim_param)) {
		isl_reordering *exp;

		model = isl_space_params(model);
		exp = isl_parameter_alignment_reordering(multi->space, model);
		exp = isl_reordering_extend_space(exp,
				    FN(MULTI(BASE),get_domain_space)(multi));
		multi = FN(MULTI(BASE),realign_domain)(multi, exp);
	}

	isl_space_free(model);
	return multi;
error:
	isl_space_free(model);
	FN(MULTI(BASE),free)(multi);
	return NULL;
}

static __isl_give MULTI(BASE) *align_params_multi_set_and(
	__isl_take MULTI(BASE) *multi, __isl_take isl_set *set,
	__isl_give MULTI(BASE) *(*fn)(__isl_take MULTI(BASE) *multi,
					__isl_take isl_set *set))
{
	isl_ctx *ctx;

	if (!multi || !set)
		goto error;
	if (isl_space_match(multi->space, isl_dim_param,
			    set->dim, isl_dim_param))
		return fn(multi, set);
	ctx = FN(MULTI(BASE),get_ctx)(multi);
	if (!isl_space_has_named_params(multi->space) ||
	    !isl_space_has_named_params(set->dim))
		isl_die(ctx, isl_error_invalid,
			"unaligned unnamed parameters", goto error);
	multi = FN(MULTI(BASE),align_params)(multi, isl_set_get_space(set));
	set = isl_set_align_params(set, FN(MULTI(BASE),get_space)(multi));
	return fn(multi, set);
error:
	FN(MULTI(BASE),free)(multi);
	isl_set_free(set);
	return NULL;
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),gist_aligned)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_set *context)
{
	int i;

	if (!multi || !context)
		goto error;

	for (i = 0; i < multi->n; ++i) {
		multi->p[i] = FN(EL,gist)(multi->p[i], isl_set_copy(context));
		if (!multi->p[i])
			goto error;
	}

	isl_set_free(context);
	return multi;
error:
	isl_set_free(context);
	FN(MULTI(BASE),free)(multi);
	return NULL;
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),gist)(__isl_take MULTI(BASE) *multi,
	__isl_take isl_set *context)
{
	return align_params_multi_set_and(multi, context,
						&FN(MULTI(BASE),gist_aligned));
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),gist_params)(
	__isl_take MULTI(BASE) *multi, __isl_take isl_set *context)
{
	isl_space *space = FN(MULTI(BASE),get_domain_space)(multi);
	isl_set *dom_context = isl_set_universe(space);
	dom_context = isl_set_intersect_params(dom_context, context);
	return FN(MULTI(BASE),gist)(multi, dom_context);
}

__isl_give MULTI(BASE) *FN(FN(MULTI(BASE),from),LIST(BASE))(
	__isl_take isl_space *space, __isl_take LIST(EL) *list)
{
	int i;
	int n;
	isl_ctx *ctx;
	MULTI(BASE) *multi;

	if (!space || !list)
		goto error;

	ctx = isl_space_get_ctx(space);
	n = FN(FN(LIST(EL),n),BASE)(list);
	if (n != isl_space_dim(space, isl_dim_out))
		isl_die(ctx, isl_error_invalid,
			"invalid number of elements in list", goto error);

	multi = FN(MULTI(BASE),alloc)(isl_space_copy(space));
	for (i = 0; i < n; ++i) {
		multi = FN(FN(MULTI(BASE),set),BASE)(multi, i,
					FN(FN(LIST(EL),get),BASE)(list, i));
	}

	isl_space_free(space);
	FN(LIST(EL),free)(list);
	return multi;
error:
	isl_space_free(space);
	FN(LIST(EL),free)(list);
	return NULL;
}

/* Create a multi expression in the given space that maps each
 * input dimension to the corresponding output dimension.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),identity)(__isl_take isl_space *space)
{
	int i, n;
	isl_local_space *ls;
	MULTI(BASE) *multi;

	if (!space)
		return NULL;

	if (isl_space_is_set(space))
		isl_die(isl_space_get_ctx(space), isl_error_invalid,
			"expecting map space", goto error);

	n = isl_space_dim(space, isl_dim_out);
	if (n != isl_space_dim(space, isl_dim_in))
		isl_die(isl_space_get_ctx(space), isl_error_invalid,
			"number of input and output dimensions needs to be "
			"the same", goto error);

	multi = FN(MULTI(BASE),alloc)(isl_space_copy(space));

	if (!n) {
		isl_space_free(space);
		return multi;
	}

	space = isl_space_domain(space);
	ls = isl_local_space_from_space(space);

	for (i = 0; i < n; ++i) {
		EL *el;
		el = FN(EL,var_on_domain)(isl_local_space_copy(ls),
						isl_dim_set, i);
		multi = FN(FN(MULTI(BASE),set),BASE)(multi, i, el);
	}

	isl_local_space_free(ls);

	return multi;
error:
	isl_space_free(space);
	return NULL;
}

/* Construct a multi expression in the given space with value zero in
 * each of the output dimensions.
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),zero)(__isl_take isl_space *space)
{
	int n;
	MULTI(BASE) *multi;

	if (!space)
		return NULL;

	n = isl_space_dim(space , isl_dim_out);
	multi = FN(MULTI(BASE),alloc)(isl_space_copy(space));

	if (!n)
		isl_space_free(space);
	else {
		int i;
		isl_local_space *ls;
		EL *el;

		space = isl_space_domain(space);
		ls = isl_local_space_from_space(space);
		el = FN(EL,zero_on_domain)(ls);

		for (i = 0; i < n; ++i)
			multi = FN(FN(MULTI(BASE),set),BASE)(multi, i,
							    FN(EL,copy)(el));

		FN(EL,free)(el);
	}

	return multi;
}

__isl_give MULTI(BASE) *FN(MULTI(BASE),drop_dims)(
	__isl_take MULTI(BASE) *multi,
	enum isl_dim_type type, unsigned first, unsigned n)
{
	int i;
	unsigned dim;

	multi = FN(MULTI(BASE),cow)(multi);
	if (!multi)
		return NULL;

	dim = FN(MULTI(BASE),dim)(multi, type);
	if (first + n > dim || first + n < first)
		isl_die(FN(MULTI(BASE),get_ctx)(multi), isl_error_invalid,
			"index out of bounds",
			return FN(MULTI(BASE),cow)(multi));

	multi->space = isl_space_drop_dims(multi->space, type, first, n);
	if (!multi->space)
		return FN(MULTI(BASE),cow)(multi);

	if (type == isl_dim_out) {
		for (i = 0; i < n; ++i)
			FN(EL,free)(multi->p[first + i]);
		for (i = first; i + n < multi->n; ++i)
			multi->p[i] = multi->p[i + n];
		multi->n -= n;

		return multi;
	}

	for (i = 0; i < multi->n; ++i) {
		multi->p[i] = FN(EL,drop_dims)(multi->p[i], type, first, n);
		if (!multi->p[i])
			return FN(MULTI(BASE),cow)(multi);
	}

	return multi;
}

/* Given two MULTI(BASE)s A -> B and C -> D,
 * construct a MULTI(BASE) (A * C) -> (B, D).
 */
__isl_give MULTI(BASE) *FN(MULTI(BASE),flat_range_product)(
	__isl_take MULTI(BASE) *multi1, __isl_take MULTI(BASE) *multi2)
{
	int i, n1, n2;
	EL *el;
	isl_space *space;
	MULTI(BASE) *res;

	if (!multi1 || !multi2)
		goto error;

	space = isl_space_range_product(FN(MULTI(BASE),get_space)(multi1),
					FN(MULTI(BASE),get_space)(multi2));
	space = isl_space_flatten_range(space);
	res = FN(MULTI(BASE),alloc)(space);

	n1 = FN(MULTI(BASE),dim)(multi1, isl_dim_out);
	n2 = FN(MULTI(BASE),dim)(multi2, isl_dim_out);

	for (i = 0; i < n1; ++i) {
		el = FN(FN(MULTI(BASE),get),BASE)(multi1, i);
		res = FN(FN(MULTI(BASE),set),BASE)(res, i, el);
	}

	for (i = 0; i < n2; ++i) {
		el = FN(FN(MULTI(BASE),get),BASE)(multi2, i);
		res = FN(FN(MULTI(BASE),set),BASE)(res, n1 + i, el);
	}

	FN(MULTI(BASE),free)(multi1);
	FN(MULTI(BASE),free)(multi2);
	return res;
error:
	FN(MULTI(BASE),free)(multi1);
	FN(MULTI(BASE),free)(multi2);
	return NULL;
}
