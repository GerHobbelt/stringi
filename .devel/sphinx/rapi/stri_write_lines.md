# stri_write_lines: Write Text Lines to a Text File

## Description

Writes a text file is such a way that each element of a given character vector becomes a separate text line.

## Usage

``` r
stri_write_lines(
  str,
  con,
  encoding = "UTF-8",
  sep = ifelse(.Platform$OS.type == "windows", "\r\n", "\n"),
  fname = con
)
```

## Arguments

|            |                                                                            |
|------------|----------------------------------------------------------------------------|
| `str`      | character vector with data to write                                        |
| `con`      | name of the output file or a connection object (opened in the binary mode) |
| `encoding` | output encoding, `NULL` or `''` for the current default one                |
| `sep`      | newline separator                                                          |
| `fname`    | deprecated alias of `con`                                                  |

## Details

It is a substitute for the <span class="rlang">**R**</span> [`writeLines`](https://stat.ethz.ch/R-manual/R-devel/library/base/html/writeLines.html) function, with the ability to easily re-encode the output.

We suggest using the UTF-8 encoding for all text files: thus, it is the default one for the output.

## Value

This function returns nothing noteworthy.

## Author(s)

[Marek Gagolewski](https://www.gagolewski.com/) and other contributors

## See Also

The official online manual of <span class="pkg">stringi</span> at <https://stringi.gagolewski.com/>

Gagolewski M., <span class="pkg">stringi</span>: Fast and portable character string processing in R, *Journal of Statistical Software* 103(2), 2022, 1-59, [doi:10.18637/jss.v103.i02](https://doi.org/10.18637/jss.v103.i02)

Other files: [`stri_read_lines()`](stri_read_lines.md), [`stri_read_raw()`](stri_read_raw.md)
