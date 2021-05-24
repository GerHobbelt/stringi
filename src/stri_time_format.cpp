/* This file is part of the 'stringi' project.
 * Copyright (c) 2013-2021, Marek Gagolewski <https://www.gagolewski.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "stri_stringi.h"
#include "stri_container_utf16.h"
#include "stri_container_utf8.h"
#include "stri_container_double.h"
#include "stri_container_integer.h"
#include <unicode/calendar.h>
#include <unicode/gregocal.h>
#include <unicode/smpdtfmt.h>


/**
 * Get date format
 *
 * @version 1.6.3 (Marek Gagolewski, 2021-05-24)
 *    refactor from stri_datetime_parse
 */
DateFormat* stri__get_date_format(
    const char* format_val, const char* locale_val, UErrorCode status
) {
    DateFormat* fmt = NULL;

    // "format" may be one of:
    const char* format_opts[] = {
        "date_full", "date_long", "date_medium", "date_short",
        "date_relative_full", "date_relative_long", "date_relative_medium", "date_relative_short",
        "time_full", "time_long", "time_medium", "time_short",
        "time_relative_full", "time_relative_long", "time_relative_medium", "time_relative_short",
        "datetime_full", "datetime_long", "datetime_medium", "datetime_short",
        "datetime_relative_full", "datetime_relative_long", "datetime_relative_medium", "datetime_relative_short",
        NULL
    };
    int format_cur = stri__match_arg(format_val, format_opts);

    if (format_cur >= 0) {
        DateFormat::EStyle style = DateFormat::kNone;
        switch (format_cur % 8) {
        case 0:
            style = DateFormat::kFull;
            break;
        case 1:
            style = DateFormat::kLong;
            break;
        case 2:
            style = DateFormat::kMedium;
            break;
        case 3:
            style = DateFormat::kShort;
            break;
        case 4:
            style = DateFormat::kFullRelative;
            break;
        case 5:
            style = DateFormat::kLongRelative;
            break;
        case 6:
            style = DateFormat::kMediumRelative;
            break;
        case 7:
            style = DateFormat::kShortRelative;
            break;
        default:
            style = DateFormat::kNone;
            break;
        }

        /* ICU 54.1: Relative time styles are not currently supported.  */
        switch (format_cur / 8) {
        case 0:
            fmt = DateFormat::createDateInstance(
                style, Locale::createFromName(locale_val)
            );
            break;

        case 1:
            fmt = DateFormat::createTimeInstance(
                (DateFormat::EStyle)(style & ~DateFormat::kRelative),
                Locale::createFromName(locale_val)
            );
            break;

        case 2:
            fmt = DateFormat::createDateTimeInstance(
                style,
                (DateFormat::EStyle)(style & ~DateFormat::kRelative),
                Locale::createFromName(locale_val)
            );
            break;

        default:
            fmt = NULL;
            break;

        }
    }
    else {
        UnicodeString format_str(format_val);
        fmt = new SimpleDateFormat(
            format_str, Locale::createFromName(locale_val), status
        );
    }

    return fmt;
}


/**
 * Format date-time objects
 *
 * @param time
 * @param format
 * @param tz
 * @param locale
 *
 * @return character vector
 *
 * @version 0.5-1 (Marek Gagolewski, 2015-01-05)
 * @version 0.5-1 (Marek Gagolewski, 2015-02-22) use tz
 * @version 1.6.3 (Marek Gagolewski, 2021-05-24) #434: vectorise wrt format
 */
SEXP stri_datetime_format(SEXP time, SEXP format, SEXP tz, SEXP locale) {
    const char* locale_val = stri__prepare_arg_locale(locale, "locale", true);
    PROTECT(time = stri__prepare_arg_POSIXct(time, "time"));
    PROTECT(format = stri__prepare_arg_string(format, "format"));

    R_len_t vectorize_length = stri__recycling_rule(true, 2, LENGTH(time), LENGTH(format));
    if (vectorize_length <= 0) {
        UNPROTECT(2);
        return Rf_allocVector(STRSXP, 0);
    }

    TimeZone* tz_val = stri__prepare_arg_timezone(tz, "tz", true/*allowdefault*/);
    Calendar* cal = NULL;
    DateFormat* fmt = NULL;

    STRI__ERROR_HANDLER_BEGIN(2)
    StriContainerDouble time_cont(time, vectorize_length);
    StriContainerUTF8 format_cont(format, vectorize_length);

    UErrorCode status = U_ZERO_ERROR;
    cal = Calendar::createInstance(locale_val, status);
    STRI__CHECKICUSTATUS_THROW(status, {/* do nothing special on err */})

    cal->adoptTimeZone(tz_val);
    tz_val = NULL; /* The Calendar takes ownership of the TimeZone. */

    SEXP ret;
    STRI__PROTECT(ret = Rf_allocVector(STRSXP, vectorize_length));

    const String8* format_last = NULL; // this will allow for formatter reuse
    for (R_len_t i = format_cont.vectorize_init();
            i != format_cont.vectorize_end();
            i = format_cont.vectorize_next(i))
    {
        if (time_cont.isNA(i) || format_cont.isNA(i)) {
            SET_STRING_ELT(ret, i, NA_STRING);
            continue;
        }

        const String8* format_cur = &(format_cont.get(i));
        if (format_cur != format_last) {
            // well, no reuse possible - resetting
            format_last = format_cur;

            if (fmt) {
                delete fmt;
                fmt = NULL;
            }

            status = U_ZERO_ERROR;
            fmt = stri__get_date_format(format_cur->c_str(), locale_val, status);
            STRI__CHECKICUSTATUS_THROW(status, {/* do nothing special on err */})
        }

        status = U_ZERO_ERROR;
        cal->setTime((UDate)(time_cont.get(i)*1000.0), status);
        STRI__CHECKICUSTATUS_THROW(status, {/* do nothing special on err */})

        FieldPosition pos;
        UnicodeString out;
        fmt->format(*cal, out, pos);

        std::string s;
        out.toUTF8String(s);
        SET_STRING_ELT(ret, i, Rf_mkCharLenCE(s.c_str(), (int)s.length(), (cetype_t)CE_UTF8));
    }

    if (tz_val) {
        delete tz_val;
        tz_val = NULL;
    }
    if (fmt) {
        delete fmt;
        fmt = NULL;
    }
    if (cal) {
        delete cal;
        cal = NULL;
    }
    STRI__UNPROTECT_ALL
    return ret;
    STRI__ERROR_HANDLER_END({
        if (tz_val) {
            delete tz_val;
            tz_val = NULL;
        }
        if (fmt) {
            delete fmt;
            fmt = NULL;
        }
        if (cal) {
            delete cal;
            cal = NULL;
        }
    })
}


/**
 * Parse date-time objects
 *
 * @param str
 * @param format
 * @param tz
 * @param lenient
 * @param locale
 *
 * @return character vector
 *
 * @version 0.5-1 (Marek Gagolewski, 2015-01-08)
 * @version 0.5-1 (Marek Gagolewski, 2015-01-11) lenient arg added
 * @version 0.5-1 (Marek Gagolewski, 2015-02-22) use tz
 * @version 0.5-1 (Marek Gagolewski, 2015-03-01) set tzone attrib on retval
 * @version 1.6.3 (Marek Gagolewski, 2021-05-24) #434: vectorise wrt format
 */
SEXP stri_datetime_parse(SEXP str, SEXP format, SEXP lenient, SEXP tz, SEXP locale) {
    const char* locale_val = stri__prepare_arg_locale(locale, "locale", true);
    PROTECT(str = stri__prepare_arg_string(str, "str"));
    PROTECT(format = stri__prepare_arg_string(format, "format"));
    bool lenient_val = stri__prepare_arg_logical_1_notNA(lenient, "lenient");
    if (!isNull(tz)) PROTECT(tz = stri__prepare_arg_string_1(tz, "tz"));
    else             PROTECT(tz); /* needed to set tzone attrib */

    R_len_t vectorize_length = stri__recycling_rule(true, 2, LENGTH(str), LENGTH(format));
    if (vectorize_length <= 0) {
        UNPROTECT(3);
        return Rf_allocVector(REALSXP, 0);
    }

    TimeZone* tz_val = stri__prepare_arg_timezone(tz, "tz", true/*allowdefault*/);
    Calendar* cal = NULL;
    DateFormat* fmt = NULL;
    STRI__ERROR_HANDLER_BEGIN(3)
    StriContainerUTF16 str_cont(str, vectorize_length);
    StriContainerUTF8 format_cont(format, vectorize_length);

    UErrorCode status = U_ZERO_ERROR;
    cal = Calendar::createInstance(locale_val, status);
    STRI__CHECKICUSTATUS_THROW(status, {/* do nothing special on err */})

    cal->adoptTimeZone(tz_val);
    tz_val = NULL; /* The Calendar takes ownership of the TimeZone. */

    cal->setLenient(lenient_val);

    SEXP ret;
    STRI__PROTECT(ret = Rf_allocVector(REALSXP, vectorize_length));

    const String8* format_last = NULL; // this will allow for formatter reuse
    for (R_len_t i = format_cont.vectorize_init();
            i != format_cont.vectorize_end();
            i = format_cont.vectorize_next(i))
    {
        if (str_cont.isNA(i) || format_cont.isNA(i)) {
            REAL(ret)[i] = NA_REAL;
            continue;
        }

        const String8* format_cur = &(format_cont.get(i));
        if (format_cur != format_last) {
            // well, no reuse possible - resetting
            format_last = format_cur;

            if (fmt) {
                delete fmt;
                fmt = NULL;
            }

            status = U_ZERO_ERROR;
            fmt = stri__get_date_format(format_cur->c_str(), locale_val, status);
            STRI__CHECKICUSTATUS_THROW(status, {/* do nothing special on err */})
        }

        status = U_ZERO_ERROR;
        ParsePosition pos;
        fmt->parse(str_cont.get(i), *cal, pos);

        if (pos.getErrorIndex() >= 0)
            REAL(ret)[i] = NA_REAL;
        else {
            status = U_ZERO_ERROR;
            REAL(ret)[i] = ((double)cal->getTime(status))/1000.0;
            if (U_FAILURE(status)) REAL(ret)[i] = NA_REAL;
        }
    }


    if (!isNull(tz)) Rf_setAttrib(ret, Rf_ScalarString(Rf_mkChar("tzone")), tz);
    stri__set_class_POSIXct(ret);
    if (tz_val) {
        delete tz_val;
        tz_val = NULL;
    }
    if (fmt) {
        delete fmt;
        fmt = NULL;
    }
    if (cal) {
        delete cal;
        cal = NULL;
    }
    STRI__UNPROTECT_ALL
    return ret;
    STRI__ERROR_HANDLER_END({
        if (tz_val) {
            delete tz_val;
            tz_val = NULL;
        }
        if (fmt) {
            delete fmt;
            fmt = NULL;
        }
        if (cal) {
            delete cal;
            cal = NULL;
        }
    })
}
