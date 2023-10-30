# stri_compare: Compare Strings with or without Collation

## Description

These functions may be used to determine if two strings are equal, canonically equivalent (this is performed in a much more clever fashion than when testing for equality), or to check whether they are in a specific lexicographic order.

## Usage

``` r
stri_compare(e1, e2, ..., opts_collator = NULL)

stri_cmp(e1, e2, ..., opts_collator = NULL)

stri_cmp_eq(e1, e2)

stri_cmp_neq(e1, e2)

stri_cmp_equiv(e1, e2, ..., opts_collator = NULL)

stri_cmp_nequiv(e1, e2, ..., opts_collator = NULL)

stri_cmp_lt(e1, e2, ..., opts_collator = NULL)

stri_cmp_gt(e1, e2, ..., opts_collator = NULL)

stri_cmp_le(e1, e2, ..., opts_collator = NULL)

stri_cmp_ge(e1, e2, ..., opts_collator = NULL)
```

## Arguments

|                 |                                                                                                                                                                  |
|-----------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `e1`, `e2`      | character vectors or objects coercible to character vectors                                                                                                      |
| `...`           | additional settings for `opts_collator`                                                                                                                          |
| `opts_collator` | a named list with <span class="pkg">ICU</span> Collator\'s options, see [`stri_opts_collator`](stri_opts_collator.md), `NULL` for the default collation options. |

## Details

All the functions listed here are vectorized over `e1` and `e2`.

`stri_cmp_eq` tests whether two corresponding strings consist of exactly the same code points, while `stri_cmp_neq` allows to check whether there is any difference between them. These are locale-independent operations: for natural language processing, where the notion of canonical equivalence is more valid, this might not be exactly what you are looking for, see Examples. Please note that <span class="pkg">stringi</span> always silently removes UTF-8 BOMs from input strings, therefore, e.g., `stri_cmp_eq` does not take BOMs into account while comparing strings.

`stri_cmp_equiv` tests for canonical equivalence of two strings and is locale-dependent. Additionally, the <span class="pkg">ICU</span>\'s Collator may be tuned up so that, e.g., the comparison is case-insensitive. To test whether two strings are not canonically equivalent, call `stri_cmp_nequiv`.

`stri_cmp_le` tests whether the elements in the first vector are less than or equal to the corresponding elements in the second vector, `stri_cmp_ge` tests whether they are greater or equal, `stri_cmp_lt` if less, and `stri_cmp_gt` if greater, see also, e.g., [`%s<%`](+25s+3C+25.md).

`stri_compare` is an alias to `stri_cmp`. They both perform exactly the same locale-dependent operation. Both functions provide a C library\'s `strcmp()` look-and-feel, see Value for details.

For more information on <span class="pkg">ICU</span>\'s Collator and how to tune its settings refer to [`stri_opts_collator`](stri_opts_collator.md). Note that different locale settings may lead to different results (see the examples below).

## Value

The `stri_cmp` and `stri_compare` functions return an integer vector representing the comparison results: `-1` if `e1[...] < e2[...]`, `0` if they are canonically equivalent, and `1` if greater.

All the other functions return a logical vector that indicates whether a given relation holds between two corresponding elements in `e1` and `e2`.

## Author(s)

[Marek Gagolewski](https://www.gagolewski.com/) and other contributors

## References

*Collation* -- ICU User Guide, <https://unicode-org.github.io/icu/userguide/collation/>

## See Also

The official online manual of <span class="pkg">stringi</span> at <https://stringi.gagolewski.com/>

Gagolewski M., <span class="pkg">stringi</span>: Fast and portable character string processing in R, *Journal of Statistical Software* 103(2), 2022, 1-59, [doi:10.18637/jss.v103.i02](https://doi.org/10.18637/jss.v103.i02)

Other locale_sensitive: [`%s<%()`](+25s+3C+25.md), [`about_locale`](about_locale.md), [`about_search_boundaries`](about_search_boundaries.md), [`about_search_coll`](about_search_coll.md), [`stri_count_boundaries()`](stri_count_boundaries.md), [`stri_duplicated()`](stri_duplicated.md), [`stri_enc_detect2()`](stri_enc_detect2.md), [`stri_extract_all_boundaries()`](stri_extract_boundaries.md), [`stri_locate_all_boundaries()`](stri_locate_boundaries.md), [`stri_opts_collator()`](stri_opts_collator.md), [`stri_order()`](stri_order.md), [`stri_rank()`](stri_rank.md), [`stri_sort_key()`](stri_sort_key.md), [`stri_sort()`](stri_sort.md), [`stri_split_boundaries()`](stri_split_boundaries.md), [`stri_trans_tolower()`](stri_trans_casemap.md), [`stri_unique()`](stri_unique.md), [`stri_wrap()`](stri_wrap.md)

## Examples




```r
# in Polish, ch < h:
stri_cmp_lt('hladny', 'chladny', locale='pl_PL')
```

```
## [1] FALSE
```

```r
# in Slovak, ch > h:
stri_cmp_lt('hladny', 'chladny', locale='sk_SK')
```

```
## [1] TRUE
```

```r
# < or > (depends on locale):
stri_cmp('hladny', 'chladny')
```

```
## [1] 1
```

```r
# ignore case differences:
stri_cmp_equiv('hladny', 'HLADNY', strength=2)
```

```
## [1] TRUE
```

```r
# also ignore diacritical differences:
stri_cmp_equiv('hladn\u00FD', 'hladny', strength=1, locale='sk_SK')
```

```
## [1] TRUE
```

```r
marios <- c('Mario', 'mario', 'M\\u00e1rio', 'm\\u00e1rio')
stri_cmp_equiv(marios, 'mario', case_level=TRUE, strength=2L)
```

```
## [1] FALSE  TRUE FALSE FALSE
```

```r
stri_cmp_equiv(marios, 'mario', case_level=TRUE, strength=1L)
```

```
## [1] FALSE  TRUE FALSE FALSE
```

```r
stri_cmp_equiv(marios, 'mario', strength=1L)
```

```
## [1]  TRUE  TRUE FALSE FALSE
```

```r
stri_cmp_equiv(marios, 'mario', strength=2L)
```

```
## [1]  TRUE  TRUE FALSE FALSE
```

```r
# non-Unicode-normalized vs normalized string:
stri_cmp_equiv(stri_trans_nfkd('\u0105'), '\u105')
```

```
## [1] TRUE
```

```r
# note the difference:
stri_cmp_eq(stri_trans_nfkd('\u0105'), '\u105')
```

```
## [1] FALSE
```

```r
# ligatures:
stri_cmp_equiv('\ufb00', 'ff', strength=2)
```

```
## [1] TRUE
```

```r
# phonebook collation
stri_cmp_equiv('G\u00e4rtner', 'Gaertner', locale='de_DE@collation=phonebook', strength=1L)
```

```
## [1] TRUE
```

```r
stri_cmp_equiv('G\u00e4rtner', 'Gaertner', locale='de_DE', strength=1L)
```

```
## [1] FALSE
```
