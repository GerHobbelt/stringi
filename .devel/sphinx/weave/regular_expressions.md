

(Sec:regex)=
Regular Expressions
===================



> This tutorial is based on the [paper on *stringi*](https://dx.doi.org/10.18637/jss.v103.i02) that was published in the *Journal of Statistical Software*; see {cite}`stringi`. To learn more about R, check out Marek's open-access (free!) textbook [*Deep R Programming*](https://deepr.gagolewski.com/) {cite}`deepr`.


Regular expressions (regexes) provide concise grammar for
defining systematic patterns that can be sought in character strings.
Examples of such patterns include: specific fixed substrings, emojis of
any kind, stand-alone sequences of lower-case Latin letters ("words"),
substrings that can be interpreted as real numbers (with or without
fractional parts, also in scientific notation), telephone numbers, email
addresses, or URLs.

Theoretically, the concept of regular pattern matching dates back to the
so-called regular languages and finite state automata {cite}`kleene`, see
also {cite}`hopcroftullman` and {cite}`automata`. Regexes in the form
as we know it today
have already been present in one of the pre-Unix implementations of the
command-line text editor *qed* ({cite}`qed`; the predecessor of the
well-known *sed*).

Base R gives access to two different regex matching engines (via
functions such as `gregexpr()` and `grep()`):

-   ERE (extended regular expressions that conform to the
    POSIX.2-1992 standard;
    via the [*TRE*](https://github.com/laurikari/tre/) library);
    used by default,

-   PCRE (Perl-compatible regular expressions;
    via the [*PCRE2*](https://www.pcre.org/) library);
    activated when `perl=TRUE` is set.

Other matchers are implemented in the [*ore*](https://CRAN.R-project.org/package=ore), [*re2r*](https://github.com/qinwf/re2r),
and [*re2*](https://github.com/girishji/re2/) packages.



*Stringi*, on the other hand, provides access to the regex engine
implemented in [*ICU*](https://icu.unicode.org/),
which was inspired by Java's *util.regex* in
*JDK 1.4*. Their syntax is mostly compatible with that of *PCRE*, although
certain more advanced facets might not be supported (e.g., recursive
patterns). On the other hand, *ICU* regexes fully conform to the
[Unicode Technical Standard \#18](https://www.unicode.org/reports/tr18/)
and hence provide comprehensive support for Unicode.

It is worth noting that most programming languages and advanced
text editors and development environments
(including [*Kate*](https://kate-editor.org/),
[*Eclipse*](https://www.eclipse.org/ide/),
[*VSCode*](https://code.visualstudio.com/),
and [*RStudio*](https://www.rstudio.com/products/rstudio/)) support finding or replacing patterns with
regexes. Therefore, they should be amongst the instruments at every data
scientist's disposal. One general introduction to regexes is {cite}`friedl`.
The *ICU* flavour is summarised at
<https://unicode-org.github.io/icu/userguide/strings/regexp.html>.

Below we provide a concise yet comprehensive introduction to the topic
from the perspective of the *stringi* package users. This time we will
use the pattern search routines whose names end with the `*_regex()`
suffix. Apart from `stri_detect_regex()`, `stri_locate_all_regex()`, and
so forth, in {ref}`Sec:Capturing` we introduce `stri_match_all_regex()`.
Moreover, the table below lists the available options for the regex
engine.

| Option                     | Purpose                                                                                                                                                                                                                    |
| -------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `case_insensitive`         | logical; defaults to `FALSE`; whether to enable (full) case-insensitive matching                                                                                                                                           |
| `comments`                 | logical; defaults to `FALSE`; whether to allow white spaces and comments within patterns                                                                                                                                   |
| `dot_all`                  | logical; defaults to `FALSE`; if set, "`.`" matches line terminators; otherwise its matching stops at a line end                                                                                                           |
| `literal`                  | logical; defaults to `FALSE`; whether to treat the entire pattern as a literal string; note that in most cases the code-pointwise string search facilities (`*_fixed()` functions described in {ref}`Sec:fixed` are faster |
| `multi_line`               | logical; defaults to `FALSE`; if set, "`$`" and "`^`" recognise line terminators within a string; otherwise, they match only at start and end of the input                                                                 |
| `unix_lines`               | logical; defaults to `FALSE`; when enabled, only the Unix line ending, i.e., U+000A, is honoured as a terminator by "`.`", "`$`", and "`^`"                                                                                |
| `uword`                    | logical; defaults to `FALSE`; whether to use the Unicode definition of word boundaries (see {ref}`Sec:BoundaryAnalysis`), which are quite different from the traditional regex word boundaries                             |
| `error_on_unknown_escapes` | logical; defaults to `FALSE`; whether unrecognised backslash-escaped characters trigger an error; by default, unknown backslash-escaped ASCII letters represent themselves                                                 |
| `time_limit`               | integer; processing time limit for match operations in ~milliseconds (depends on the CPU speed); 0 for no limit (the default)                                                                                              |
| `stack_limit`              | integer; maximal size, in bytes, of the heap storage available for the matcher's backtracking stack; setting a limit is desirable if poorly written regexes are expected on input; 0 for no limit (the default)            |


(Sec:RegexIndividualChars)=
Matching Individual Characters
------------------------------

We begin by discussing different ways to define character sets. In this
part, determining the length of all matching substrings will be quite
straightforward.

The following characters have special meaning to the regex engine:

> `.`
> `\`
> `|`
> `(`
> `)`
> `[`
> `{`
> `}`
> `^`
> `$`
> `*`
> `+`
> `?`

Any regular expression that doesn't contain the above behaves like a
fixed pattern:



```r
stri_count_regex("spam, eggs, spam, bacon, sausage, and spam", "spam")
## [1] 3
```

There are hence 3 occurrences of a pattern that is comprised of 4 code
points, "`s`" followed by "`p`", then by "`a`", and ending with "`m`".

However, this time the case insensitive mode fully supports Unicode
matching[^footequivalent]:



```r
stri_detect_regex("groß", "GROSS", case_insensitive=TRUE)
## [1] TRUE
```

[^footequivalent]: This does not mean, though, that it considers canonically
    equivalent strings as equal, see {ref}`Sec:Equivalence` for
    discussion and a workaround.

If we wish to include a special character as part of a regular
expression -- so that it is treated literally -- we'll need to escape
it with a backslash, "\\". Yet, the backlash itself has a special
meaning to R, see `help("Quotes")`. Therefore it needs to be preceded by
another backslash.



```r
stri_count_regex("spam...", "\\.")   # "\\" is a way to input a single \
## [1] 3
```

In other words, the R string `"\\."` is seen by the regex engine as
"`\.`" and interpreted as the dot character (literally). Alternatively,
since R 4.0 we can also input the so-called literal strings like
`r"(\.)"`.


### Matching Any Character

The (unescaped) dot, "`.`", matches any code point except the newline.



```r
x <- "Spam, ham,\njam, SPAM, eggs, and spam"
stri_extract_all_regex(x, "..am", case_insensitive=TRUE)
## [[1]]
## [1] "Spam" " ham" "SPAM" "spam"
```

The above matches non-overlapping length-4 substrings that end with
"`am`".

The dot's insensitivity to the newline character is motivated by the
need to maintain compatibility with tools such as *grep* (when
searching within text files in a line-by-line manner). This behaviour
can be altered by setting the `dot_all` option to `TRUE`.



```r
stri_extract_all_regex(x, "..am", dot_all=TRUE, case_insensitive=TRUE)
## [[1]]
## [1] "Spam"  " ham"  "\njam" "SPAM"  "spam"
```

### Defining Character Sets

Sets of characters can be introduced by enumerating their members within
a pair of square brackets. For instance, "`[abc]`" denotes the set
$\{\mathtt{a},\mathtt{b},\mathtt{c}\}$ -- such a regular expression
matches one (and only one) symbol from this set. Moreover, in:



```r
stri_extract_all_regex(x, "[hj]am")
## [[1]]
## [1] "ham" "jam"
```

the "`[hj]am`" regex matches: "`h`" or "`j`", followed by "`a`",
followed by "`m`". In other words, `"ham"` and `"jam"` are the only two
strings that are matched by this pattern (unless matching is done
case-insensitively).

The following characters, if used within square brackets, may be treated
non-literally:

> `\`
> `[`
> `]`
> `^`
> `-`
> `&`

Therefore, to include them as-is in a character set, the
backslash-escape must be used. For example, "`[\[\]\\]`" matches a
backslash or a square bracket.

### Complementing Sets

Including "`^`" after the opening square bracket denotes the set
complement. Hence, "`[^abc]`" matches any code point except "`a`",
"`b`", and "`c`". Here is an example where we seek any substring that
consists of 3 non-spaces.



```r
x <- "Nobody expects the Spanish Inquisition!"
stri_extract_all_regex(x, "[^ ][^ ][^ ]")
## [[1]]
##  [1] "Nob" "ody" "exp" "ect" "the" "Spa" "nis" "Inq" "uis" "iti" "on!"
```

### Defining Code Point Ranges

Each Unicode code point can be referenced by its unique numeric
identifier; see {ref}`Sec:codepoints` for more details. For instance, "`a`" is
assigned code U+0061, and "`z`" is mapped to U+007A. In the pre-Unicode
era (mostly with regards to the ASCII codes, ≤ U+007F, representing
English letters, decimal digits, some punctuation characters, and a few
control characters), we were used to relying on specific code ranges;
e.g., "`[a-z]`" denotes the set comprised of all characters with codes
between U+0061 and U+007A, i.e., lowercase letters of the English
(Latin) alphabet.



```r
stri_extract_all_regex("In 2020, Gągolewski had fun once.", "[0-9A-Za-z]")
## [[1]]
##  [1] "I" "n" "2" "0" "2" "0" "G" "g" "o" "l" "e" "w" "s" "k" "i" "h" "a" "d"
## [19] "f" "u" "n" "o" "n" "c" "e"
```

The above pattern denotes a union of 3 code ranges: digits and ASCII
upper- and lowercase letters.

Nowadays, when processing text in natural languages, this notation
should be avoided. Note the missing "`ą`" (Polish "`a`" with
ogonek) in the result.

### Using Predefined Character Sets

Each code point is assigned a unique general category, which can be
thought of as a character's class, see
[Unicode Standard Annex \#44: {U}nicode Character Database](https://unicode.org/reports/tr44/). Sets of characters
from each category can be referred to, amongst others, by using the
"`\p{category}`" (or, equivalently, "`[\p{category}]`") syntax:



```r
x <- "aąbßÆAĄB你123,.;'! \t-+=[]©←→”„²³¾"
p <- c("\\p{L}", "\\p{Ll}", "\\p{Lu}", "\\p{N}", "\\p{P}", "\\p{S}")
structure(stri_extract_all_regex(x, p), names=p)
## $`\\p{L}`
## [1] "a"  "ą"  "b"  "ß"  "Æ"  "A"  "Ą"  "B"  "你"
## 
## $`\\p{Ll}`
## [1] "a" "ą" "b" "ß"
## 
## $`\\p{Lu}`
## [1] "Æ" "A" "Ą" "B"
## 
## $`\\p{N}`
## [1] "1" "2" "3" "²" "³" "¾"
## 
## $`\\p{P}`
##  [1] "," "." ";" "'" "!" "-" "[" "]" "”" "„"
## 
## $`\\p{S}`
## [1] "+" "=" "©" "←" "→"
```

The above yield a match to: arbitrary letters, lowercase letters,
uppercase letters, numbers, punctuation marks, and symbols,
respectively.

Characters' binary properties and scripts can also be referenced in a
similar manner. Some other noteworthy classes include:



```r
p <- c("\\w", "\\d", "\\s")
structure(stri_extract_all_regex(x, p), names=p)
## $`\\w`
##  [1] "a"  "ą"  "b"  "ß"  "Æ"  "A"  "Ą"  "B"  "你" "1"  "2"  "3" 
## 
## $`\\d`
## [1] "1" "2" "3"
## 
## $`\\s`
## [1] " "  "\t"
```

These give: word characters, decimal digits ("`\p{Nd}`"), and spaces
("`[\t\n\f\r\p{Z}]`"), in this order.

Moreover, e.g., the upper-cased "`\P{category}`" and "`\W`" are
equivalent to "`[^\p{category}]`" and "`[^\w]`", respectively, i.e.,
denote their complements.


### Avoiding POSIX Classes

The use of the POSIX-like character classes should be avoided because
they are generally not well-defined.

In particular, in POSIX-like regex engines, "`[:punct:]`" stands for the
character class corresponding to the `ispunct()` function in C (see
"`man 3 ispunct`" on Unix-like systems). According to ISO/IEC 9899:1990
(ISO C90), `ispunct()` tests for any printing character except for the
space or a character for which `isalnum()` is true.

Base R with *PCRE* yields on the current author's machine:



```r
x <- ",./|\\<>?;:'\"[]{}-=_+()*&^%$€#@!`~×‒„”"
regmatches(x, gregexpr("[[:punct:]]", x, perl=TRUE))  # base R
## [[1]]
##  [1] ","  "."  "/"  "|"  "\\" "<"  ">"  "?"  ";"  ":"  "'"  "\"" "["  "]" 
## [15] "{"  "}"  "-"  "="  "_"  "+"  "("  ")"  "*"  "&"  "^"  "%"  "$"  "#" 
## [29] "@"  "!"  "`"  "~"
```

However, the details of the characters' belongingness to this class
depend on the current locale. Therefore, the reader might obtain
different results when calling the above.

*ICU*, on the other hand, always gives:



```r
stri_extract_all_regex(x, "[[:punct:]]")    # equivalently: \p{P}
## [[1]]
##  [1] ","  "."  "/"  "\\" "?"  ";"  ":"  "'"  "\"" "["  "]"  "{"  "}"  "-" 
## [15] "_"  "("  ")"  "*"  "&"  "%"  "#"  "@"  "!"  "‒"  "„"  "”"
```

Here, `[:punct:]` is merely a synonym for `\p{P}`. Further, `\p{S}`
captures symbols:



```r
stri_extract_all_regex(x, "\\p{S}")         # symbols
## [[1]]
##  [1] "|" "<" ">" "=" "+" "^" "$" "€" "`" "~" "×"
```

We strongly recommend, wherever possible, using the portable
"`[\p{P}\p{S}]`" as an alternative to the *PCRE*'s "`[:punct:]`".


Alternating and Grouping Subexpressions
---------------------------------------

### Alternation Operator

The alternation operator, "`|`", matches either its left or its right
branch, for instance:



```r
x <- "spam, egg, ham, jam, algae, and an amalgam of spam, all al dente"
stri_extract_all_regex(x, "spam|ham")
## [[1]]
## [1] "spam" "ham"  "spam"
```

"`|`" has very low precedence. Therefore, if we wish to introduce an
alternative of subexpressions, we need to group them, e.g., between
round brackets. For instance, "`(sp|h)am`" matches either "`spam`"
or "`ham`".

Note that this has the side-effect of creating new capturing groups;
see {ref}`Sec:Capturing`.



### Grouping Subexpressions

Also, matching is always done left-to-right, on a first-come,
first-served basis. So, if the left branch is a subset of the right
one, the latter will never be matched. In particular,
"`(al|alga|algae)`" can only match "`al`". To fix this, we can write
"`(algae|alga|al)`".

### Non-grouping Parentheses

Some parenthesised subexpressions -- those in which the opening bracket
is followed by the question mark -- have a distinct meaning. In
particular, "`(?#...)`" denotes a free-format comment that is ignored by
the regex parser:



```r
stri_extract_all_regex(x,
  "(?# match 'sp' or 'h')(sp|h)(?# and 'am')am|(?# or match 'egg')egg")
## [[1]]
## [1] "spam" "egg"  "ham"  "spam"
```

Nevertheless, constructing more sophisticated regexes by concatenating
subfragments thereof may sometimes be more readable:



```r
stri_extract_all_regex(x,
  stri_join(
      "(sp|h)",   # match either 'sp' or 'h'
      "am",       # followed by 'am'
    "|",            # ... or ...
      "egg"       # just match 'egg'
))
## [[1]]
## [1] "spam" "egg"  "ham"  "spam"
```

What is more, e.g., "`(?i)`" enables the `case_insensitive` mode.



```r
stri_count_regex("Spam spam SPAMITY spAm", "(?i)spam")
## [1] 4
```




Quantifiers
-----------

More often than not, a variable number of instances of the same
subexpression needs to be captured, or its presence should be
optional. This can be achieved by means of the following quantifiers:

-   "`?`" matches 0 or 1 times;

-   "`*`" matches 0 or more times;

-   "`+`" matches 1 or more times;

-   "`{n,m}`" matches between `n` and `m` times;

-   "`{n,}`" matches at least `n` times;

-   "`{n}`" matches exactly `n` times.

These operators are applied to the preceding atoms. For example, "`ba+`"
captures `"ba"`, `"baa"`, `"baaa"`, etc., but neither `"b"` alone
nor `"bababa"` altogether.

By default, the quantifiers are greedy -- they match the repeated
subexpression as many times as possible. The "`?`" suffix (hence,
quantifiers such as "`??`", "`*?`", "`+?`", and so forth) tries with as
few occurrences as possible (to obtain a match still).



```r
x <- "sp(AM)(maps)(SP)am"
stri_extract_all_regex(x,
  c("\\(.+\\)",    # [[1]] greedy
    "\\(.+?\\)",   # [[2]] lazy
    "\\([^)]+\\)"  # [[3]] greedy (but clever)
))
## [[1]]
## [1] "(AM)(maps)(SP)"
## 
## [[2]]
## [1] "(AM)"   "(maps)" "(SP)"  
## 
## [[3]]
## [1] "(AM)"   "(maps)" "(SP)"
```

The first regex is greedy: it matches an opening bracket, then as many
characters as possible (including "`)`") that are followed by a closing
bracket. The two other patterns terminate as soon as the first closing
bracket is found.



```r
stri_extract_first_regex("spamamamnomnomnomammmmmmmmm",
  c("sp(am|nom)+",             "sp(am|nom)+?",
    "sp(am|nom)+?m*",          "sp(am|nom)+?m+"))
## [1] "spamamamnomnomnomam"         "spam"                       
## [3] "spam"                        "spamamamnomnomnomammmmmmmmm"
```

Let's stress that the quantifier is applied to the subexpression that
stands directly before it. Grouping parentheses can be used in case they
are needed.



```r
stri_extract_all_regex("12, 34.5, 678.901234, 37...629, ...",
  c("\\d+\\.\\d+", "\\d+(\\.\\d+)?"))
## [[1]]
## [1] "34.5"       "678.901234"
## 
## [[2]]
## [1] "12"         "34.5"       "678.901234" "37"         "629"
```

Here, the first regex matches digits, a dot, and another series of
digits. The second one finds digits that are possibly (but not
necessarily) followed by a dot and a digit sequence.

### Performance Notes

*ICU*, just like *PCRE*, uses a nondeterministic finite automaton-type
algorithm. Hence, due to backtracking, some ill-defined regexes can lead
to exponential matching times (e.g., "`(a+)+b`" applied on
`"aaaa...aaaaac"`). If such patterns are expected, setting the
`time_limit` or `stack_limit` option is recommended.



```r
system.time(tryCatch({
  stri_detect_regex("a" %s*% 1000 %s+% "c", "(a+)+b", time_limit=1e5)
}, error=function(e) cat("stopped.")))
## stopped.
##    user  system elapsed 
##  11.135   0.000  11.135
```

Nevertheless, oftentimes such regexes can be naturally reformulated to
fix the underlying issue. The *ICU* User Guide on Regular Expressions
also recommends using possessive quantifiers ("`?+`", "`*+`", "`++`",
and so on), which match as many times as possible but, contrary to the
plain-greedy ones, never backtrack when they happen to consume too much
data.

See also the documentation of
the [*re2r*](https://github.com/qinwf/re2r)
and [*re2*](https://github.com/girishji/re2/)
(wrappers around the [*RE2*](https://github.com/google/re2)
library) packages and the references therein for a discussion.




(Sec:Capturing)=
Capture Groups and References Thereto
-------------------------------------

Round-bracketed subexpressions carry one additional characteristic: they
form the so-called capture groups that can be extracted separately or be
referred to in other parts of the same regex.

### Extracting Capture Group Matches

The above is evident when we use the versions of `stri_extract()` that
are sensitive to the presence of capture groups:



```r
x <- "name='Sir Launcelot', quest='Seek the Grail', favecolour='blue'"
stri_match_all_regex(x, "(\\w+)='(.+?)'")
## [[1]]
##      [,1]                     [,2]         [,3]            
## [1,] "name='Sir Launcelot'"   "name"       "Sir Launcelot" 
## [2,] "quest='Seek the Grail'" "quest"      "Seek the Grail"
## [3,] "favecolour='blue'"      "favecolour" "blue"
```

The findings are presented in a matrix form. The first column gives the
complete matches, the second column stores the matches to the first
capture group, and so forth.

If we just need the grouping part of "`(...)`", i.e., without the
capturing feature, "`(?:...)`" can be applied. Also, named capture
groups defined like "`(?<name>...)`" are fully supported since version
1.7.1 of our package (for historical notes see {cite}`namedCapture`).



```r
stri_match_all_regex(x, "(?:\\w+)='(?<value>.+?)'")
## [[1]]
##                               value           
## [1,] "name='Sir Launcelot'"   "Sir Launcelot" 
## [2,] "quest='Seek the Grail'" "Seek the Grail"
## [3,] "favecolour='blue'"      "blue"
```

### Locating Capture Group Matches

The `capture_groups` attribute in `stri_locate__regex` enables us to
pinpoint the matches to the parenthesised subexpressions as well:



```r
stri_locate_all_regex(x, "(?<key>\\w+)='(?<value>.+?)'",
  capture_groups=TRUE, get_length=TRUE)
## [[1]]
##      start length
## [1,]     1     20
## [2,]    23     22
## [3,]    47     17
## attr(,"capture_groups")
## attr(,"capture_groups")$key
##      start length
## [1,]     1      4
## [2,]    23      5
## [3,]    47     10
## 
## attr(,"capture_groups")$value
##      start length
## [1,]     7     13
## [2,]    30     14
## [3,]    59      4
```

Note that each item in the resulting list is equipped with a
`"capture_groups"` attribute. For instance,
`attr(result[[1]], "capture_groups")[[2]]` extracts the locations of the
matches to the 2nd capture group in the first input string.

### Replacing with Capture Group Matches

Matches to particular capture groups can be recalled in replacement
strings when using `stri_replace()`. Here, the match in its entirety is
denoted with "`$0`", then "`$1`" stores whatever was caught by the first
capture group, "`$2`" is the match to the second capture group, etc.
Moreover, "`\$`" gives the dollar-sign.



```r
stri_replace_all_regex(x, "(\\w+)='(.+?)'", "$2 is a $1")
## [1] "Sir Launcelot is a name, Seek the Grail is a quest, blue is a favecolour"
```

Named capture groups can be referred to too:



```r
stri_replace_all_regex(x, "(?<key>\\w+)='(?<value>.+?)'",
  "${value} is a ${key}")
## [1] "Sir Launcelot is a name, Seek the Grail is a quest, blue is a favecolour"
```

### Back-Referencing

Matches to capture groups can also be part of the regexes themselves.
For example, "`\1`" denotes whatever has been consumed by the first
capture group.

Even though, in general, parsing HTML code with regexes is not
recommended, let us consider the following examples:



```r
stri_extract_all_regex("<strong><em>spam</em></strong><code>eggs</code>",
  c("<[a-z]+>.*?</[a-z]+>", "<([a-z]+)>.*?</\\1>"))
## [[1]]
## [1] "<strong><em>spam</em>" "<code>eggs</code>"    
## 
## [[2]]
## [1] "<strong><em>spam</em></strong>" "<code>eggs</code>"
```

The second regex guarantees that the match will include all characters
between the opening `<tag>` and the corresponding (not: any) closing
`</tag>`. Named capture groups can be referenced using the `\k<name>`
syntax (the angle brackets are part of the token), as in, e.g.,
"`<(?<tagname>[a-z]+)>.*?</\k<tagname>>`".

Anchoring
---------

Lastly, let's mention how to match a pattern at a given abstract
position within a string.


### Matching at the Beginning or End of a String

"`^`" and "`$`" match, respectively, the start and the end of the string (or
each line within a string, if the `multi_line` option is set to `TRUE`).



```r
x <- c("spam egg", "bacon spam", "spam", "egg spam bacon", "sausage")
p <- c("spam", "^spam", "spam$", "spam$|^spam", "^spam$")
structure(outer(x, p, stri_detect_regex), dimnames=list(x, p))
##                 spam ^spam spam$ spam$|^spam ^spam$
## spam egg        TRUE  TRUE FALSE        TRUE  FALSE
## bacon spam      TRUE FALSE  TRUE        TRUE  FALSE
## spam            TRUE  TRUE  TRUE        TRUE   TRUE
## egg spam bacon  TRUE FALSE FALSE       FALSE  FALSE
## sausage        FALSE FALSE FALSE       FALSE  FALSE
```

The 5 regular expressions match "`spam`", respectively, anywhere within
the string, at the beginning, at the end, at the beginning or end, and
in strings that are equal to the pattern itself.


### Matching at Word Boundaries

Furthermore, "`\b`" matches at a "word boundary", e.g., near spaces,
punctuation marks, or at the start/end of a string (i.e., wherever there
is a transition between a word, "`\w`", and a non-word character,
"`\W`", or vice versa).

In the following example, we match all stand-alone numbers
(this regular expression is provided for didactic purposes only):



```r
stri_extract_all_regex("12, 34.5, J23, 37.629cm", "\\b\\d+(\\.\\d+)?+\\b")
## [[1]]
## [1] "12"   "34.5"
```


Note the possessive quantifier, "`?+`": try matching a dot and a
sequence of digits, and if it's present but not followed by a word
boundary, do not retry by matching a word boundary only.


### Looking Behind and Ahead

There are also ways to guarantee that a pattern occurrence begins or
ends with a match to some subexpression: "`(?<=...)...`" is the
so-called look-behind, whereas "`...(?=...)`" denotes the look-ahead.
Moreover, "`(?<!...)...`" and "`...(?!...)`" are their negated
("negative look-behind/ahead") versions.



```r
stri_extract_all_regex("I like spam, spam, eggs, and spam.",
  c("\\w+(?=[,.])", "\\w++(?![,.])"))
## [[1]]
## [1] "spam" "spam" "eggs" "spam"
## 
## [[2]]
## [1] "I"    "like" "and"
```

The first regex captures words that end with "`,`" or "`.`". The second
one matches words that end neither with "`,`" nor "`.`".
