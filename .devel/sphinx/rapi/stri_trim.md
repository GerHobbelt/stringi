# stri_trim: Trim Characters from the Left and/or Right Side of a String

## Description

These functions may be used, e.g., to remove unnecessary white-spaces from strings. Trimming ends at the first or starts at the last `pattern` match.

## Usage

``` r
stri_trim_both(str, pattern = "\\P{Wspace}", negate = FALSE)

stri_trim_left(str, pattern = "\\P{Wspace}", negate = FALSE)

stri_trim_right(str, pattern = "\\P{Wspace}", negate = FALSE)

stri_trim(
  str,
  side = c("both", "left", "right"),
  pattern = "\\P{Wspace}",
  negate = FALSE
)
```

## Arguments

|           |                                                                                                                                                                                              |
|-----------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `str`     | a character vector of strings to be trimmed                                                                                                                                                  |
| `pattern` | a single pattern, specifying the class of characters (see [stringi-search-charclass](about_search_charclass.md)) to to be preserved (if `negate` is `FALSE`; default) or trimmed (otherwise) |
| `negate`  | either `TRUE` or `FALSE`; see `pattern`                                                                                                                                                      |
| `side`    | character \[`stri_trim` only\]; defaults to `'both'`                                                                                                                                         |

## Details

Vectorized over `str` and `pattern`.

`stri_trim` is a convenience wrapper over `stri_trim_left` and `stri_trim_right`.

Contrary to many other string processing libraries, our trimming functions are universal. The class of characters to be retained or trimmed can be adjusted.

For replacing pattern matches with an arbitrary replacement string, see [`stri_replace`](stri_replace.md).

Trimming can also be used where you would normally rely on regular expressions. For instance, you may get `'23.5'` out of `'total of 23.5 bitcoins'`.

For trimming white-spaces, please note the difference between Unicode binary property \'`\p{Wspace}`\' (more universal) and general character category \'`\p{Z}`\', see [stringi-search-charclass](about_search_charclass.md).

## Value

All functions return a character vector.

## Author(s)

[Marek Gagolewski](https://www.gagolewski.com/) and other contributors

## See Also

The official online manual of <span class="pkg">stringi</span> at <https://stringi.gagolewski.com/>

Gagolewski M., <span class="pkg">stringi</span>: Fast and portable character string processing in R, *Journal of Statistical Software* 103(2), 2022, 1-59, [doi:10.18637/jss.v103.i02](https://doi.org/10.18637/jss.v103.i02)

Other search_replace: [`about_search`](about_search.md), [`stri_replace_all()`](stri_replace.md), [`stri_replace_rstr()`](stri_replace_rstr.md)

Other search_charclass: [`about_search_charclass`](about_search_charclass.md), [`about_search`](about_search.md)

## Examples




```r
stri_trim_left('               aaa')
```

```
## [1] "aaa"
```

```r
stri_trim_right('r-project.org/', '\\P{P}')
```

```
## [1] "r-project.org"
```

```r
stri_trim_both(' Total of 23.5 bitcoins. ', '\\p{N}')
```

```
## [1] "23.5"
```

```r
stri_trim_both(' Total of 23.5 bitcoins. ', '\\P{N}', negate=TRUE)
```

```
## [1] "23.5"
```
