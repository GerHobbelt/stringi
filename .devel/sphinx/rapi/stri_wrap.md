# stri_wrap: Word Wrap Text to Format Paragraphs

## Description

This function breaks text paragraphs into lines, of total width (if it is possible) at most given `width`.

## Usage

``` r
stri_wrap(
  str,
  width = floor(0.9 * getOption("width")),
  cost_exponent = 2,
  simplify = TRUE,
  normalize = TRUE,
  normalise = normalize,
  indent = 0,
  exdent = 0,
  prefix = "",
  initial = prefix,
  whitespace_only = FALSE,
  use_length = FALSE,
  locale = NULL
)
```

## Arguments

|                     |                                                                                                                                                                                                                                                                |
|---------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `str`               | character vector of strings to reformat                                                                                                                                                                                                                        |
| `width`             | single integer giving the suggested maximal total width/number of code points per line                                                                                                                                                                         |
| `cost_exponent`     | single numeric value, values not greater than zero will select a greedy word-wrapping algorithm; otherwise this value denotes the exponent in the cost function of a (more aesthetic) dynamic programming-based algorithm (values in \[2, 3\] are recommended) |
| `simplify`          | single logical value, see Value                                                                                                                                                                                                                                |
| `normalize`         | single logical value, see Details                                                                                                                                                                                                                              |
| `normalise`         | alias of `normalize`                                                                                                                                                                                                                                           |
| `indent`            | single non-negative integer; gives the indentation of the first line in each paragraph                                                                                                                                                                         |
| `exdent`            | single non-negative integer; specifies the indentation of subsequent lines in paragraphs                                                                                                                                                                       |
| `prefix`, `initial` | single strings; `prefix` is used as prefix for each line except the first, for which `initial` is utilized                                                                                                                                                     |
| `whitespace_only`   | single logical value; allow breaks only at white-spaces? if `FALSE`, <span class="pkg">ICU</span>\'s line break iterator is used to split text into words, which is suitable for natural language processing                                                   |
| `use_length`        | single logical value; should the number of code points be used instead of the total code point width (see [`stri_width`](stri_width.md))?                                                                                                                      |
| `locale`            | `NULL` or `''` for text boundary analysis following the conventions of the default locale, or a single string with locale identifier, see [stringi-locale](about_locale.md)                                                                                    |

## Details

Vectorized over `str`.

If `whitespace_only` is `FALSE`, then <span class="pkg">ICU</span>\'s line-`BreakIterator` is used to determine text boundaries where a line break is possible. This is a locale-dependent operation. Otherwise, the breaks are only at white-spaces.

Note that Unicode code points may have various widths when printed on the console and that this function, by default, takes that into account. By changing the state of the `use_length` argument, this function starts to act as if each code point was of width 1.

If `normalize` is `FALSE`, then multiple white spaces between the word boundaries are preserved within each wrapped line. In such a case, none of the strings can contain `\r`, `\n`, or other new line characters, otherwise you will get an error. You should split the input text into lines or, for example, substitute line breaks with spaces before applying this function.

If `normalize` is `TRUE`, then all consecutive white space (ASCII space, horizontal TAB, CR, LF) sequences are replaced with single ASCII spaces before actual string wrapping. Moreover, [`stri_split_lines`](stri_split_lines.md) and [`stri_trans_nfc`](stri_trans_nf.md) is called on the input character vector. This is for compatibility with [`strwrap`](https://stat.ethz.ch/R-manual/R-devel/library/base/html/strwrap.html).

The greedy algorithm (for `cost_exponent` being non-positive) provides a very simple way for word wrapping. It always puts as many words in each line as possible. This method -- contrary to the dynamic algorithm -- does not minimize the number of space left at the end of every line. The dynamic algorithm (a.k.a. Knuth\'s word wrapping algorithm) is more complex, but it returns text wrapped in a more aesthetic way. This method minimizes the squared (by default, see `cost_exponent`) number of spaces (raggedness) at the end of each line, so the text is mode arranged evenly. Note that the cost of printing the last line is always zero.

## Value

If `simplify` is `TRUE`, then a character vector is returned. Otherwise, you will get a list of `length(str)` character vectors.

## Author(s)

[Marek Gagolewski](https://www.gagolewski.com/) and other contributors

## References

D.E. Knuth, M.F. Plass, Breaking paragraphs into lines, *Software: Practice and Experience* 11(11), 1981, pp. 1119--1184.

## See Also

The official online manual of <span class="pkg">stringi</span> at <https://stringi.gagolewski.com/>

Gagolewski M., <span class="pkg">stringi</span>: Fast and portable character string processing in R, *Journal of Statistical Software* 103(2), 2022, 1-59, [doi:10.18637/jss.v103.i02](https://doi.org/10.18637/jss.v103.i02)

Other locale_sensitive: [`%s<%()`](+25s+3C+25.md), [`about_locale`](about_locale.md), [`about_search_boundaries`](about_search_boundaries.md), [`about_search_coll`](about_search_coll.md), [`stri_compare()`](stri_compare.md), [`stri_count_boundaries()`](stri_count_boundaries.md), [`stri_duplicated()`](stri_duplicated.md), [`stri_enc_detect2()`](stri_enc_detect2.md), [`stri_extract_all_boundaries()`](stri_extract_boundaries.md), [`stri_locate_all_boundaries()`](stri_locate_boundaries.md), [`stri_opts_collator()`](stri_opts_collator.md), [`stri_order()`](stri_order.md), [`stri_rank()`](stri_rank.md), [`stri_sort_key()`](stri_sort_key.md), [`stri_sort()`](stri_sort.md), [`stri_split_boundaries()`](stri_split_boundaries.md), [`stri_trans_tolower()`](stri_trans_casemap.md), [`stri_unique()`](stri_unique.md)

Other text_boundaries: [`about_search_boundaries`](about_search_boundaries.md), [`about_search`](about_search.md), [`stri_count_boundaries()`](stri_count_boundaries.md), [`stri_extract_all_boundaries()`](stri_extract_boundaries.md), [`stri_locate_all_boundaries()`](stri_locate_boundaries.md), [`stri_opts_brkiter()`](stri_opts_brkiter.md), [`stri_split_boundaries()`](stri_split_boundaries.md), [`stri_split_lines()`](stri_split_lines.md), [`stri_trans_tolower()`](stri_trans_casemap.md)

## Examples




```r
s <- stri_paste(
   'Lorem ipsum dolor sit amet, consectetur adipisicing elit. Proin ',
   'nibh augue, suscipit a, scelerisque sed, lacinia in, mi. Cras vel ',
   'lorem. Etiam pellentesque aliquet tellus.')
cat(stri_wrap(s, 20, 0.0), sep='\n') # greedy
```

```
## Lorem ipsum dolor
## sit amet,
## consectetur
## adipisicing elit.
## Proin nibh augue,
## suscipit a,
## scelerisque sed,
## lacinia in, mi. Cras
## vel lorem. Etiam
## pellentesque aliquet
## tellus.
```

```r
cat(stri_wrap(s, 20, 2.0), sep='\n') # dynamic
```

```
## Lorem ipsum
## dolor sit amet,
## consectetur
## adipisicing elit.
## Proin nibh augue,
## suscipit a,
## scelerisque sed,
## lacinia in, mi. Cras
## vel lorem. Etiam
## pellentesque aliquet
## tellus.
```

```r
cat(stri_pad(stri_wrap(s), side='both'), sep='\n')
```

```
##   Lorem ipsum dolor sit amet, consectetur adipisicing elit. Proin nibh  
##   augue, suscipit a, scelerisque sed, lacinia in, mi. Cras vel lorem.   
##                    Etiam pellentesque aliquet tellus.
```
