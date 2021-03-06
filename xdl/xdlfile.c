﻿/***********************************************************************
	Easily xdl v5.5

	(c) 2013-2016 JianDe LiFang Technology Corporation.  All Rights Reserved.

	@author ZhangWenQuan, JianDe HangZhou ZheJiang China, Mail: powersuite@hotmaol.com

	@doc file document

	@module	xdlfile.c | xdl file implement file

	@devnote 张文权 2005.01 - 2007.12	v3.0
	@devnote 张文权 2008.01 - 2009.12	v3.5
	@devnote 张文权 2012.01 - 2015.12	v4.0
	@devnote 张文权 2016.01 - 2016.12	v4.5
	@devnote 张文权 2017.01 - 2017.12	v5.0
	@devnote 张文权 2018.01 - 2018.12	v5.5
***********************************************************************/

/**********************************************************************
This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
LICENSE.GPL3 for more details.
***********************************************************************/

#include "xdlfile.h"
#include "xdlimp.h"
#include "xdloem.h"
#include "xdlstd.h"
#include "xdlnet.h"
#include "xdldoc.h"


xhand_t	xfile_bio(file_t fh)
{
	if_bio_t* pfn = (if_bio_t*)fh;

	XDL_ASSERT(fh);

	return pfn->bio;
}

file_t xfile_open(const secu_desc_t* psd, const tchar_t* file, dword_t mode)
{
	if_bio_t* pfn;
	int proto;

	proto = parse_proto(file);
	if (proto < 0)
		return NULL;

	pfn = (if_bio_t*)xmem_alloc(sizeof(if_bio_t));

	if (IS_INET_FILE(proto))
		pfn->bio = inet_open_file(psd, file, mode);
	else
		pfn->bio = xunc_open_file(psd, file, mode);

	if (!pfn->bio)
	{
		xmem_free(pfn);
		return NULL;
	}

	if (IS_INET_FILE(proto))
	{
		pfn->pf_close = inet_close_file;
		pfn->pf_read = inet_read_file;
		pfn->pf_read_range = inet_read_file_range;
		pfn->pf_write = inet_write_file;
		pfn->pf_write_range = inet_write_file_range;
		pfn->pf_flush = NULL;
		pfn->pf_set_time = inet_set_filetime;
		pfn->pf_set_since = inet_set_filesince;
	}
	else
	{
		pfn->pf_close = xunc_close_file;
		pfn->pf_read = xunc_read_file;
		pfn->pf_read_range = xunc_read_file_range;
		pfn->pf_write = xunc_write_file;
		pfn->pf_write_range = xunc_write_file_range;
		pfn->pf_flush = xunc_flush_file;
		pfn->pf_set_time = xunc_set_filetime;
		pfn->pf_set_since = NULL;
	}

	return (file_t)pfn;
}

bool_t	xfile_read(file_t fh, byte_t* buf, dword_t size)
{
	if_bio_t* pfn = (if_bio_t*)fh;
	dword_t nbys, npos = 0;
	bool_t rt = 1;

	XDL_ASSERT(fh && pfn->pf_read);
	
	while (npos < size)
	{
		nbys = size - npos;

		rt = (*pfn->pf_read)(pfn->bio, (void*)(buf + npos), &nbys);

		if (!rt || !nbys)
			break;

		npos += nbys;
	}

	return rt;
}

bool_t xfile_read_range(file_t fh, dword_t hoff, dword_t loff, byte_t* buf, dword_t size)
{
	if_bio_t* pfn = (if_bio_t*)fh;
	dword_t nbys, npos = 0;
	bool_t rt = 1;

	long long ll;

	XDL_ASSERT(fh && pfn->pf_read_range);

	while (npos < size)
	{
		nbys = size - npos;

		rt = (*pfn->pf_read_range)(pfn->bio, hoff, loff, (buf + npos), &nbys);

		if (!rt || !nbys)
			break;

		npos += nbys;

		ll = MAKELONGLONG(loff, hoff);
		ll += nbys;
		hoff = GETHDWORD(ll);
		loff = GETLDWORD(ll);
	}

	return rt;
}

bool_t xfile_write(file_t fh, const byte_t* buf, dword_t size)
{
	if_bio_t* pfn = (if_bio_t*)fh;
	dword_t nbys, npos = 0;
	bool_t rt = 1;

	XDL_ASSERT(fh);

	while (npos < size)
	{
		nbys = size - npos;

		rt = (*pfn->pf_write)(pfn->bio, (void*)(buf + npos), &nbys);

		if (!rt || !nbys)
			break;

		npos += nbys;
	}

	return rt;
}

bool_t	xfile_write_range(file_t fh, dword_t hoff, dword_t loff, const byte_t* buf, dword_t size)
{
	if_bio_t* pfn = (if_bio_t*)fh;
	dword_t nbys, npos = 0;
	bool_t rt = 1;

	long long ll;

	XDL_ASSERT(fh);

	while (npos < size)
	{
		nbys = size - npos;

		rt = (*pfn->pf_write_range)(pfn->bio, hoff, loff, (buf + npos), &nbys);

		if (!rt || !nbys)
			break;

		npos += nbys;

		ll = MAKELONGLONG(loff, hoff);
		ll += nbys;
		hoff = GETHDWORD(ll);
		loff = GETLDWORD(ll);
	}

	return rt;
}

bool_t xfile_flush(file_t fh)
{
	if_bio_t* pfn = (if_bio_t*)fh;

	XDL_ASSERT(fh);

	if (pfn->pf_flush)
	{
		return (*pfn->pf_flush)(pfn->bio);
	}
	else
	{
		return 1;
	}
}

void xfile_set_since(file_t fh, int fs)
{
	if_bio_t* pfn = (if_bio_t*)fh;

	XDL_ASSERT(fh);

	if (pfn->pf_set_since)
	{
		(*pfn->pf_set_since)(pfn->bio, fs);
	}
}

void xfile_settime(file_t fh, const tchar_t* ftime)
{
	if_bio_t* pfn = (if_bio_t*)fh;

	XDL_ASSERT(fh);

	if (pfn->pf_set_time)
	{
		(*pfn->pf_set_time)(pfn->bio, ftime);
	}
}

void xfile_close(file_t fh)
{
	if_bio_t* pfn = (if_bio_t*)fh;

	XDL_ASSERT(fh && pfn->pf_close);

	if (pfn->bio)
	{
		(*pfn->pf_close)(pfn->bio);
	}

	xmem_free(pfn);
}

bool_t xfile_info(const secu_desc_t* psd, const tchar_t* fname, tchar_t* ftime, tchar_t* fsize, tchar_t* fetag, tchar_t* fencode)
{
	int proto;

	if(is_null(fname))
		return 0;

	proto = parse_proto(fname);

	if (IS_INET_FILE(proto))
	{
#if defined(XDK_SUPPORT_SOCK) && defined(XDL_SUPPORT_DOC)
		return inet_file_info(psd, fname, ftime, fsize, fetag, fencode);
#else
		return 0;
#endif
	}
	if (!IS_INET_FILE(proto))
	{
#if defined(XDK_SUPPORT_FILE)
		return xunc_file_info(psd, fname, ftime, fsize, fetag, fencode);
#else
		return 0;
#endif
	}

	return 0;
}

bool_t xfile_delete(const secu_desc_t* psd, const tchar_t* fname)
{
	int proto;

	if (is_null(fname))
		return 0;

	proto = parse_proto(fname);

	if (IS_INET_FILE(proto))
	{
#if defined(XDK_SUPPORT_SOCK) && defined(XDL_SUPPORT_DOC)
		return inet_delete_file(psd, fname);
#else
		return 0;
#endif
	}
	if (!IS_INET_FILE(proto))
	{
#if defined(XDK_SUPPORT_FILE)
		return xunc_delete_file(psd, fname);
#else
		return 0;
#endif
	}

	return 0;
}

bool_t	xfile_rename(const secu_desc_t* psd, const tchar_t* fname, const tchar_t* nname)
{
	int proto;

	if (is_null(fname))
		return 0;

	proto = parse_proto(fname);

	if (IS_INET_FILE(proto))
	{
		return 0;
	}
	if (!IS_INET_FILE(proto))
	{
#if defined(XDK_SUPPORT_FILE)
		return xunc_rename_file(psd, fname, nname);
#else
		return 0;
#endif
	}

	return 0;
}

XDL_API bool_t xfile_mkdir(const secu_desc_t* psd, const tchar_t* path)
{
	int proto;

	if (is_null(path))
		return 0;

	proto = parse_proto(path);

	if (IS_INET_FILE(proto))
	{
		return 0;
	}
	if (!IS_INET_FILE(proto))
	{
#if defined(XDK_SUPPORT_FILE)
		return xunc_create_directory(psd, path);
#endif
	}
	return 0;
}

XDL_API bool_t xfile_rmdir(const secu_desc_t* psd, const tchar_t* path)
{
	int proto;

	if (is_null(path))
		return 0;

	proto = parse_proto(path);

	if (IS_INET_FILE(proto))
		return 0;

#if defined(XDK_SUPPORT_FILE)
	if (!IS_INET_FILE(proto))
		return xunc_remove_directory(psd, path);
	else
		return 0;
#endif
}

bool_t xfile_copy(const secu_desc_t* psd, const tchar_t* srcfile, const tchar_t* destfile, int fsin)
{
	tchar_t fsize[NUM_LEN + 1] = { 0 };
	tchar_t ftime[DATE_LEN + 1] = { 0 };
	tchar_t fsince[DATE_LEN + 1] = { 0 };
	xdate_t dt_since, dt_time;

	dword_t len;
	bool_t rt;
	bool_t b_range = 0;
	dword_t hoff, loff;
	dword_t max = 2 * XHTTP_ZIPED_SIZE;
	long long l_bytes, l_total = 0;

	byte_t* d_buf = NULL;
	file_t d_src = NULL;
	file_t d_dest = NULL;

	if (!xfile_info(psd, srcfile, ftime, fsize, NULL, NULL))
		return 0;

	if (is_zero_size(fsize))
	{
		set_last_error(_T("-1"), _T("not support zero file copy"), -1);
		return 0;
	}

	if (fsin)
	{
		xfile_info(psd, destfile, fsince, NULL, NULL, NULL);

		parse_gmttime(&dt_since, fsince);
		parse_gmttime(&dt_time, ftime);

		if (compare_datetime(&dt_since, &dt_time) >= 0)
		{
			set_last_error(_T("-1"), _T("file not modified"), -1);
			return 0;
		}
	}

	parse_longlong(&hoff, &loff, fsize);
	b_range = (hoff || loff > max) ? 1 : 0;
	l_total = xstoll(fsize);

	d_src = xfile_open(psd, srcfile, 0);
	if (!d_src)
		return 0;

	d_dest = xfile_open(psd, destfile, FILE_OPEN_CREATE);
	if (!d_dest)
	{
		xfile_close(d_src);
		return 0;
	}
		
	if (b_range)
	{
		hoff = loff = 0;
		while (l_total)
		{
			if ((long long)MAKELONGLONG(max, 0) > l_total)
				len = (dword_t)l_total;
			else
				len = max;

			d_buf = (byte_t*)xmem_alloc(len);
			rt = xfile_read_range(d_src, hoff, loff, d_buf, len);
			if (rt)
			{
				//last set filetime
				if (len == (dword_t)l_total)
				{
					xfile_settime(d_dest, ftime);
				}

				rt = xfile_write_range(d_dest, hoff, loff, d_buf, len);
			}
			xmem_free(d_buf);

			if (!rt)
				break;

			l_bytes = (long long)MAKELONGLONG(loff, hoff) + len;
			hoff = GETHDWORD(l_bytes);
			loff = GETLDWORD(l_bytes);

			l_total -= len;
		}

		xfile_close(d_src);
		d_src = NULL;
	}
	else
	{
		len = xstol(fsize);
		d_buf = (byte_t*)xmem_alloc(len);

		rt = xfile_read(d_src, d_buf, len);

		xfile_close(d_src);
		d_src = NULL;

		if (rt)
		{
			xfile_settime(d_dest, ftime);

			rt = xfile_write(d_dest, d_buf, len);
		}

		xmem_free(d_buf);
		d_buf = NULL;
	}

	rt = xfile_flush(d_dest);

	xfile_close(d_dest);
	d_dest = NULL;

	return rt;
}
