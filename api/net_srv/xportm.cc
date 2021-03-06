
#include "xportm.h"

void* STDCALL wait_thread(void* param)
{
	xportm_param_t* pxm = (xportm_param_t*)param;
	stream_t stm;
	dword_t size;
	byte_t* buf;

	xdl_thread_init();

	while (pxm->act)
	{
		if(!xpipe_listen(pxm->pip, 500))
		{
			xthread_sleep(10);
			continue;
		}

		stm = stream_alloc(pxm->pip);
		if (!stm)
		{
			break;
		}

		stream_set_mode(stm, CHUNK_OPERA);

		while (stream_read_chunk_size(stm, &size))
		{
			if (!size)
				break;

			buf = (byte_t*)xmem_alloc(size);
			stream_read_bytes(stm, buf, &size);

			if (size)
			{
				if (pxm->cr)
				{
					xcriti_enter(pxm->cr);
				}

				stream_write_bytes(pxm->stm, buf, size);

				if (pxm->cr)
				{
					xcriti_leave(pxm->cr);
				}
			}

			xmem_free(buf);
			buf = NULL;
		}

		stream_free(stm);
		stm = NULL;

		xpipe_stop(pxm->pip);
	}

	xevent_sign(pxm->ev, 1);

	xdl_thread_uninit(0);

	xthread_end();

	return 0;
}

void _xportm_start(xportm_param_t* pxm)
{
	dword_t dw;

	XDL_ASSERT(pxm != NULL);

	TRY_CATCH;

	if (pxm->pip)
	{
		raise_user_error(_T("-1"), _T("service already started"));
	}
    
    pxm->std = xcons_alloc();
    if (!pxm->std)
    {
        raise_user_error(_T("-1"), _T("service alloc console failed"));
    }
    
    pxm->stm = stream_alloc(pxm->std);
    if (pxm->stm)
    {
        stream_set_encode(pxm->stm, DEF_MBS);
    }

	pxm->pip = xpipe_srv(pxm->pname, FILE_OPEN_READ | FILE_OPEN_OVERLAP);
	if (!pxm->pip)
	{
		raise_user_error(_T("-1"), _T("service create pipe failed"));
	}

	pxm->ev = xevent_create();
	pxm->cr = xcriti_create();

	pxm->act = 1;
	xthread_begin(NULL, (PF_THREADFUNC)wait_thread, (void*)pxm);

	if (pxm->stm)
	{
		stream_write(pxm->stm, _T("xportm start monitoring...\n"), -1, &dw);
	}

	END_CATCH;

	return;

ONERROR:

	if (pxm->ev)
	{
		xevent_sign(pxm->ev, 1);
	}

    if (pxm->ev)
		xevent_destroy(pxm->ev);
	pxm->ev = NULL;

	if (pxm->cr)
		xcriti_destroy(pxm->cr);
	pxm->ev = NULL;

	if (pxm->pip)
		xpipe_free(pxm->pip);
    
    if (pxm->stm)
        stream_free(pxm->stm);
    pxm->stm = NULL;
    
    if (pxm->std)
        xcons_free(pxm->std);
    pxm->std = NULL;

	return;
}

void _xportm_stop(xportm_param_t* pxm)
{
	if (pxm->stm)
	{
		if (pxm->cr)
		{
			xcriti_enter(pxm->cr);
		}

		stream_write(pxm->stm, _T("xportm try to stop...\n"), -1, NULL);

		if (pxm->cr)
		{
			xcriti_leave(pxm->cr);
		}
	}

	pxm->act = 0;

	if (pxm->ev)
	{
		xevent_wait(pxm->ev, -1);
	}

	if (pxm->std)
		xcons_free(pxm->std);
	pxm->std = NULL;

	if (pxm->stm)
		stream_free(pxm->stm);
	pxm->stm = NULL;

	if (pxm->ev)
		xevent_destroy(pxm->ev);
	pxm->ev = NULL;

	if (pxm->cr)
		xcriti_destroy(pxm->cr);
	pxm->cr = NULL;

	if (pxm->pip)
		xpipe_free(pxm->pip);
	pxm->pip = NULL;
}
