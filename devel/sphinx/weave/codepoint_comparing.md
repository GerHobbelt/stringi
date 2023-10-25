(Sec:fixed)=
Code-Pointwise Comparing
========================



> This tutorial is based on the [paper on *stringi*](https://dx.doi.org/10.18637/jss.v103.i02) that was published the *Journal of Statistical Software*; see {cite}`stringi`. To learn more about R, check out Marek's recent open-access (free!) textbook [*Deep R Programming*](https://deepr.gagolewski.com/) {cite}`deepr`.


There are many settings where we are faced with testing whether two
strings (or parts thereof) consist of exactly the same Unicode code
points, in exactly the same order. These include, for instance, matching
a nucleotide sequence in a DNA profile and querying for system resources
based on file names or UUIDs. Such tasks, due to their simplicity, can
be performed very efficiently.

Testing for Equality of Strings
-------------------------------

To quickly test whether the corresponding strings in two character
vectors are identical (in a code-pointwise manner), we can use the
`%s===%` operator or, equivalently, the `stri_cmp_eq()` function.
Moreover, `%s!==%` and `stri_cmp_neq()` implement the not-equal-to
relation.


```r
"actg" %s===% c("ACTG", "actg", "act", "actga", NA)
## [1] FALSE  TRUE FALSE FALSE    NA
```

Due to recycling, the first string was compared against the five strings in
the 2nd operand. There is only one exact match.


(Sec:strsearch)=
Searching for Fixed Strings
---------------------------

For detecting if a string contains a given fixed substring
(code-pointwisely), the fast KMP {cite}`KnuthETAL1977:kmp` algorithm, with
worst time complexity of *O(n+p)* (where *n* is the length of the string
and *p* is the length of the pattern), has been implemented in *stringi*
(with numerous tweaks for even faster matching).

The table below lists the string search functions available
in *stringi*. Below we explain their behaviour in the context of fixed
pattern matching. Notably, their description is quite detailed because
-- as we shall soon find out -- the corresponding operations are
available for the two other search engines: based on regular expressions
and the *ICU* Collator, see {ref}`Sec:regex` and {ref}`Sec:collator`.

| Name(s)                                                             | Meaning                                                         |
| ------------------------------------------------------------------- | --------------------------------------------------------------- |
| `stri_count()`                                                      | count pattern matches                                           |
| `stri_detect()`                                                     | detect pattern matches                                          |
| `stri_endswith()`                                                   | \[all but `regex`\] detect pattern matches at end of string     |
| `stri_extract_all()`, `stri_extract_first()`, `stri_extract_last()` | extract pattern matches                                         |
| `stri_locate_all()`, `stri_locate_first()`, `stri_locate_last()`    | locate pattern matches                                          |
| `stri_match_all()`, `stri_match_first()`, `stri_match_last()`       | \[`regex` only\] extract matches to regex capture groups        |
| `stri_replace_all()`, `stri_replace_first()`, `stri_replace_last()` | substitute pattern matches with some replacement strings        |
| `stri_split()`                                                      | split up a string at pattern matches                            |
| `stri_startswith()`                                                 | \[all but `regex`\] detect pattern matches at start of string   |
| `stri_subset()`, `‘stri_subset<-‘()`                                | return or replace strings that contain pattern matches          |




Counting Matches
----------------

The `stri_count_fixed()` function counts the number of times a fixed
pattern occurs in a given string.


```r
stri_count_fixed("abcabcdefabcabcabdc", "abc")  # search pattern is "abc"
## [1] 4
```

Equivalently, we can call the more generic (see below) function
`stri_count()` with the `fixed=pattern` argument:


```r
stri_count("abcabcdefabcabcabdc", fixed="abc")
## [1] 4
```

Note that, unlike in the base R `grep()` function (and the like), the
pattern ("needle") is given by the second argument (here: "`abc`"). This
makes our function more pipe-operator-friendly, because "haystack" can
be forwarded as follows:


```r
c("abcabcdefabcabcabdc", "cba", NA) |> stri_count_fixed("abc")
## [1]  4  0 NA
```

Search Engine Options
---------------------

The pattern matching engine may be tuned up by passing further arguments
to the search functions (via "`...`"; they are redirected as-is to
`stri_opts_fixed()`). The table below gives the list of available options.

| Option             | Purpose                                                                                                                                                                             |
| ------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `case_insensitive` | logical; whether to enable the simple case-insensitive matching (defaults to `FALSE`)                                                                                               |
| `overlap`          | logical; whether to enable the detection of overlapping matches (defaults to `FALSE`); available in `stri_extract_all_fixed()`, `stri_locate_all_fixed()`, and `stri_count_fixed()` |



First, we may switch on the simplistic[^footfixedcase] case-insensitive matching.

[^footfixedcase]: Which is not suitable for real-world NLP tasks,
    as it assumes that
    changing the case of a single code point always produces one and
    only one item. This way, `"groß"` does not compare equal to
    `"GROSS"`, see {ref}`Sec:collator` (and to some extent
    {ref}`Sec:regex`) for a workaround.


```r
stri_count_fixed("ACTGACGacgggACg", "acg", case_insensitive=TRUE)
## [1] 3
```

Second, we can indicate our interest in detecting overlapping pattern
matches or whether searching should continue at the end of each match
(the latter being the default behaviour):


```r
stri_count_fixed("acatgacaca", "aca")  # overlap=FALSE (default)
## [1] 2
stri_count_fixed("acatgacaca", "aca", overlap=TRUE)
## [1] 3
```


Detecting and Subsetting Patterns
---------------------------------

A somewhat simplified version of the above search task asks
whether a pattern occurs in a string at all. Such an operation can be
performed with a call to `stri_detect_fixed()`.


```r
x <- c("abc", "abcd", "def", "xyzabc", "uabdc", "dab", NA, "abc")
stri_detect_fixed(x, "abc")
## [1]  TRUE  TRUE FALSE  TRUE FALSE FALSE    NA  TRUE
```

We can also indicate that a no-match is rather of our interest by
passing `negate=TRUE`. What is more, there is an option to stop
searching once a given number of matches has been found in the
`haystack` vector (as a whole), which can speed up the processing of
larger data sets:


```r
stri_detect_fixed(x, "abc", negate=TRUE, max_count=2)
## [1] FALSE FALSE  TRUE FALSE  TRUE    NA    NA    NA
```

This can be useful in scenarios such as "find the first 2 matching
resource IDs".

There are also functions that verify whether a string starts or ends[^footanchor]
with a pattern match:


```r
stri_startswith_fixed(x, "abc")  # from=1 - match at start
## [1]  TRUE  TRUE FALSE FALSE FALSE FALSE    NA  TRUE
stri_endswith_fixed(x, "abc")    # to=-1 - match at end
## [1]  TRUE FALSE FALSE  TRUE FALSE FALSE    NA  TRUE
```

[^footanchor]: Note that testing for a pattern match at the start or end of a
    string has not been implemented separately for `regex` patterns,
    which support `"^"` and `"$"` anchors that serve precisely this
    purpose; see {ref}`Sec:regex`.

Pattern detection is often performed in conjunction with character
vector subsetting. This is why we have a specialised (and hence slightly
faster) function that returns only the strings that match a given
pattern.


```r
stri_subset_fixed(x, "abc", omit_na=TRUE)
## [1] "abc"    "abcd"   "xyzabc" "abc"
```

The above is equivalent to `x[which(stri_detect_fixed(x, "abc"))]` (note
the argument responsible for the removal of missing values), but avoids
writing `x` twice. It is particularly convenient when `x` is
generated programmatically on the fly, using some complicated
expression. Also, it works well with the forward pipe operator, as we
can write "`x |> stri_subset_fixed("abc", omit_na=TRUE)`".

There is also a replacement version of this function:


```r
stri_subset_fixed(x, "abc") <- c("*****", "***")  # modifies x in-place
print(x)  # x has changed
## [1] "*****" "***"   "def"   "*****" "uabdc" "dab"   NA      "***"
```

Locating and Extracting Patterns
--------------------------------

The functions from the `stri_locate()` family aim to pinpoint the
positions of pattern matches. First, we may be interested in getting to
know the location of the first or the last pattern occurrence:


```r
x <- c("aga", "actg", NA, "AGagaGAgaga")
stri_locate_first_fixed(x, "aga")
##      start end
## [1,]     1   3
## [2,]    NA  NA
## [3,]    NA  NA
## [4,]     3   5
stri_locate_last_fixed(x, "aga", get_length=TRUE)
##      start length
## [1,]     1      3
## [2,]    -1     -1
## [3,]    NA     NA
## [4,]     9      3
```

In both examples, we obtain a two-column matrix with the number of rows
determined by the recycling rule (here: the length of `x`). In the
former case, we get a "from--to" matrix (`get_length=FALSE`; the
default) where missing values correspond to either missing inputs or
no-matches. The latter gives a "from--length"-type matrix, where
negative lengths correspond to the not-founds.

Second, we may be yearning for the locations of all the matching
substrings. As the number of possible answers may vary from string to
string, the result is a list of index matrices.


```r
stri_locate_all_fixed(x, "aga", overlap=TRUE, case_insensitive=TRUE)
## [[1]]
##      start end
## [1,]     1   3
## 
## [[2]]
##      start end
## [1,]    NA  NA
## 
## [[3]]
##      start end
## [1,]    NA  NA
## 
## [[4]]
##      start end
## [1,]     1   3
## [2,]     3   5
## [3,]     5   7
## [4,]     7   9
## [5,]     9  11
```

Note again that a no-match is indicated by a single-row matrix with two
missing values (or with negative length if `get_length=TRUE`). This
behaviour can be changed by setting the `omit_no_match` argument to
`TRUE`.

Let us recall that "from--to" and "from--length" matrices of the above
kind constitute particularly fine inputs to `stri_sub()` and
`stri_sub_all()`. However, if merely the extraction of the matching
substrings is needed, it will be more convenient to rely on the
functions from the `stri_extract()` family:


```r
stri_extract_first_fixed(x, "aga", case_insensitive=TRUE)
## [1] "aga" NA    NA    "AGa"
stri_extract_all_fixed(x, "aga",
  overlap=TRUE, case_insensitive=TRUE, omit_no_match=TRUE)
## [[1]]
## [1] "aga"
## 
## [[2]]
## character(0)
## 
## [[3]]
## [1] NA
## 
## [[4]]
## [1] "AGa" "aga" "aGA" "Aga" "aga"
```

Replacing Pattern Occurrences
-----------------------------

In order to replace each match with a corresponding replacement string,
we can refer to `stri_replace_all()`:


```r
x <- c("aga", "actg", NA, "ggAGAGAgaGAca", "agagagaga")
stri_replace_all_fixed(x, "aga", "~", case_insensitive=TRUE)
## [1] "~"         "actg"      NA          "gg~G~GAca" "~g~ga"
```

Note that the inputs that are not part of any match are left unchanged.
The input object is left unchanged because it is not a replacement
function per se.

The operation is vectorised with respect to all the three arguments
(haystack, needle, replacement string), with the usual recycling
behaviour if necessary. If a different arguments' vectorisation scheme
is required, we can set the `vectorise_all` argument of
`stri_replace_all()` to `FALSE`. Compare the following:


```r
stri_replace_all_fixed("The quick brown fox jumped over the lazy dog.",
  c("quick", "brown",      "fox", "lazy",    "dog"),
  c("slow",  "yellow-ish", "hen", "spamity", "llama"))
## [1] "The slow brown fox jumped over the lazy dog."      
## [2] "The quick yellow-ish fox jumped over the lazy dog."
## [3] "The quick brown hen jumped over the lazy dog."     
## [4] "The quick brown fox jumped over the spamity dog."  
## [5] "The quick brown fox jumped over the lazy llama."
stri_replace_all_fixed("The quick brown fox jumped over the lazy dog.",
  c("quick", "brown",      "fox", "lazy", "dog"),
  c("slow",  "yellow-ish", "hen", "spamity", "llama"),
  vectorise_all=FALSE)
## [1] "The slow yellow-ish hen jumped over the spamity llama."
```

Here, for every string in the `haystack`, we observe the vectorisation
independently over the `needles` and replacement strings. Each
occurrence of the 1st needle is superseded by the 1st replacement
string, then the search is repeated for the 2nd needle so as to replace
it with the 2nd corresponding replacement string, and so forth.

Moreover, `stri_replace_first()` and `stri_replace_last()` can identify
and replace the first and the last match, respectively.

Splitting
---------

To split each element in the `haystack` into substrings, where the
`needles` define the delimiters that separate the inputs into tokens, we
call `stri_split()`:


```r
x <- c("a,b,c,d", "e", "", NA, "f,g,,,h,i,,j,")
stri_split_fixed(x, ",", omit_empty=TRUE)
## [[1]]
## [1] "a" "b" "c" "d"
## 
## [[2]]
## [1] "e"
## 
## [[3]]
## character(0)
## 
## [[4]]
## [1] NA
## 
## [[5]]
## [1] "f" "g" "h" "i" "j"
```

The result is a list of character vectors, as each string in the
`haystack` might be split into a possibly different number of tokens.

There is also an option to limit the number of tokens (parameter `n`).
