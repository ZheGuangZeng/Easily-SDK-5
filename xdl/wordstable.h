﻿/***********************************************************************
	Easily xdl v5.5

	(c) 2013-2016 JianDe LiFang Technology Corporation.  All Rights Reserved.

	@author ZhangWenQuan, JianDe HangZhou ZheJiang China, Mail: powersuite@hotmaol.com

	@doc wordstable document

	@module	wordstable.h | wordstable interface file

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

#ifndef _WORDSTABLE_H
#define _WORDSTABLE_H

#include "xdldef.h"

/********************************************************************************/
#define set_words_item_hidden(ilk, b)		set_words_item_delta(ilk, (var_long)b)

#define get_words_item_hidden(ilk)			(bool_t)get_words_item_delta(ilk)
/*********************************************************************************/

typedef enum{
	ORDER_NONE = 0,
	ORDER_ASCEND = 1,
	ORDER_DESCEND = -1
}SORT_ORDER;

#ifdef	__cplusplus
extern "C" {
#endif

XDL_API link_t_ptr create_words_table(int scend);

XDL_API void destroy_words_table(link_t_ptr ptr);

XDL_API void clear_words_table(link_t_ptr ptr);

XDL_API bool_t is_words_table(link_t_ptr ptr);

XDL_API bool_t is_words_item(link_t_ptr ptr, link_t_ptr ilk);

XDL_API int get_words_item_count(link_t_ptr ptr);

XDL_API link_t_ptr insert_words_item(link_t_ptr ptr, const tchar_t* val, int len);

XDL_API link_t_ptr get_words_item(link_t_ptr ptr, const tchar_t* val, int len);

XDL_API void delete_words_item(link_t_ptr ptr, link_t_ptr pos);

XDL_API const tchar_t* get_words_item_text_ptr(link_t_ptr ilk);

XDL_API void set_words_item_delta(link_t_ptr ilk, var_long data);

XDL_API var_long get_words_item_delta(link_t_ptr ilk);

XDL_API link_t_ptr get_words_next_item(link_t_ptr ptr,link_t_ptr pos);

XDL_API link_t_ptr get_words_prev_item(link_t_ptr ptr,link_t_ptr pos);

XDL_API link_t_ptr get_words_item_at(link_t_ptr ptr, int index);

XDL_API int get_words_item_index(link_t_ptr ptr, link_t_ptr ilk);

XDL_API link_t_ptr get_words_next_visible_item(link_t_ptr ptr, link_t_ptr pos);

XDL_API link_t_ptr get_words_prev_visible_item(link_t_ptr ptr, link_t_ptr pos);

XDL_API int get_words_visible_item_count(link_t_ptr ptr);

XDL_API void words_table_parse_tokens(link_t_ptr ptr,const tchar_t* tokens,int len,tchar_t feed);

XDL_API int words_table_format_tokens(link_t_ptr ptr,tchar_t* buf,int max,tchar_t feed);

#ifdef	__cplusplus
}
#endif

#endif //_WORDSTABLE_H