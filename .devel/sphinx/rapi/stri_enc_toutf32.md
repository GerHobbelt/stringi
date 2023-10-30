# stri_enc_toutf32: Convert Strings To UTF-32

## Description

UTF-32 is a 32-bit encoding where each Unicode code point corresponds to exactly one integer value. This function converts a character vector to a list of integer vectors so that, e.g., individual code points may be easily accessed, changed, etc.

## Usage

``` r
stri_enc_toutf32(str)
```

## Arguments

|       |                                                                |
|-------|----------------------------------------------------------------|
| `str` | a character vector (or an object coercible to) to be converted |

## Details

See [`stri_enc_fromutf32`](stri_enc_fromutf32.md) for a dual operation.

This function is roughly equivalent to a vectorized call to [`utf8ToInt(enc2utf8(str))`](https://stat.ethz.ch/R-manual/R-devel/library/base/html/utf8Conversion.html). If you want a list of raw vectors on output, use [`stri_encode`](stri_encode.md).

Unlike `utf8ToInt`, if ill-formed UTF-8 byte sequences are detected, a corresponding element is set to NULL and a warning is generated. To deal with such issues, use, e.g., [`stri_enc_toutf8`](stri_enc_toutf8.md).

## Value

Returns a list of integer vectors. Missing values are converted to `NULL`s.

## Author(s)

[Marek Gagolewski](https://www.gagolewski.com/) and other contributors

## See Also

The official online manual of <span class="pkg">stringi</span> at <https://stringi.gagolewski.com/>

Gagolewski M., <span class="pkg">stringi</span>: Fast and portable character string processing in R, *Journal of Statistical Software* 103(2), 2022, 1-59, [doi:10.18637/jss.v103.i02](https://doi.org/10.18637/jss.v103.i02)

Other encoding_conversion: [`about_encoding`](about_encoding.md), [`stri_enc_fromutf32()`](stri_enc_fromutf32.md), [`stri_enc_toascii()`](stri_enc_toascii.md), [`stri_enc_tonative()`](stri_enc_tonative.md), [`stri_enc_toutf8()`](stri_enc_toutf8.md), [`stri_encode()`](stri_encode.md)
